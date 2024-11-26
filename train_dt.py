import os
import re
import json
import itertools
import pandas as pd
import ydf
from alive_progress import alive_it
from benchmark import Synthesizer, ResultType, TREEOUT
from simplify_tree import simplify_file

GOAL_REGEX = re.compile(r"{(.*);} -> {(.*);}")
ASSERTION_REGEX = re.compile(r"(.*); (.*)")
POINTER_REGEX = re.compile(r"<\w+, \d> -> .")
ARRAY_REGEX = re.compile(r"[\w+, \d]")
PREDICATE_REGEX = re.compile(r"(\w+)\((.*)\)")

EXAMPLES_DIR = "examples"
EXCLUDE = ["pick.sep"]
SIMPLIFY = False

ALL_RULES = {
    "PostInconsistent",
    "SetSize",
    "Emp",
    "NullNotLVal",
    "TrueElision",
    "Frame",
    "PureFrame",
    "Reflexive",
    "ExpandImplied",
    "Open",
    "Close",
    "HeapUnify",
    "Pick",
    "PureUnify",
    "SubstRight",
    "PostInvalid",
    "Read",
    "Write",
    "Free",
    "Alloc",
    "Call",
    "AbduceCall",
    "Branch",
}


def get_assertion_features(assertion: str) -> dict[str, int | str]:
    pure, heap = ASSERTION_REGEX.match(assertion).groups()
    return {
        "pures": pure.count("&&") + 1,
        "pointers": len(POINTER_REGEX.findall(heap)),
        "arrays": len(ARRAY_REGEX.findall(heap)),
        "predicates": len(PREDICATE_REGEX.findall(heap)),
    }


def get_goal_features(goal: str) -> dict[str, int | str]:
    precondition, postcondition = GOAL_REGEX.match(goal).groups()
    precondition_features = {
        "pre_" + k: v for k, v in get_assertion_features(precondition).items()
    }
    postcondition_features = {
        "post_" + k: v for k, v in get_assertion_features(postcondition).items()
    }
    return precondition_features | postcondition_features


def make_rule_node_data(goal: str, rule: dict, completed: bool) -> dict:
    return {
        **get_goal_features(goal),
        "rule": rule,
        "score": int(
            completed
        ),  # 0 or 1, system will infer probability of leading to completion
    }


def get_rule_node_data(goal: str, node: dict, add_missing_rules: bool) -> list[dict]:
    row = make_rule_node_data(goal, node["rule"], node.get("completed", False))
    child_rows = list(
        itertools.chain.from_iterable(
            get_goal_node_data(child, add_missing_rules) for child in node["children"]
        )
    )
    return [row] + child_rows


def get_goal_node_data(node: dict, add_missing_rules: bool) -> list[dict]:
    if "goal" not in node:
        return list(
            itertools.chain.from_iterable(
                get_goal_node_data(child, add_missing_rules)
                for child in node["children"]
            )
        )
    else:
        child_rows = list(
            itertools.chain.from_iterable(
                get_rule_node_data(node["goal"], child, add_missing_rules)
                for child in node["children"]
            )
        )
        if add_missing_rules:
            child_rows += [
                make_rule_node_data(node["goal"], rule, False)
                for rule in ALL_RULES - {child["rule"] for child in node["children"]}
            ]
        return child_rows


def precision(y_true, y_pred):
    true_positives = tf.math.reduce_sum(y_true * y_pred)
    predicted_positives = tf.math.reduce_sum(y_pred)
    return true_positives / (predicted_positives + tf.keras.backend.epsilon())


def recall(y_true, y_pred):
    true_positives = tf.math.reduce_sum(y_true * y_pred)
    actual_positives = tf.math.reduce_sum(y_true)
    return true_positives / (actual_positives + tf.keras.backend.epsilon())


row_list = []
synthesizer = Synthesizer(algo="tree", depth=10, print_tree=True)
bar = alive_it(
    [
        file
        for file in os.listdir(EXAMPLES_DIR)
        if file.endswith(".sep") and file not in EXCLUDE
    ]
)
for file in bar:
    bar.text(f"Running {file}")
    result_type, _, _ = synthesizer.synthesize(f"{EXAMPLES_DIR}/{file}", timeout=10)
    if result_type != ResultType.SOLVED:
        print(f"Failed to synthesize {file} due to {result_type.value}")
        continue

    simplified_file = f"simplified_{TREEOUT}"
    if SIMPLIFY:
        simplify_file(TREEOUT, simplified_file, proof_only=True)

    with open(simplified_file if SIMPLIFY else TREEOUT, "r") as f:
        tree = json.load(f)

    row_list.extend(get_goal_node_data(tree, add_missing_rules=SIMPLIFY))


if __name__ == "__main__":
    train_ds = pd.DataFrame(row_list)
    model = ydf.GradientBoostedTreesLearner(label="score").train(train_ds)
    # model.describe()
    # model.evaluate(train_ds)
    # model.predict(train_ds)
    # model.analyze(train_ds)
    # model.benchmark(train_ds)
    model.save("model")
