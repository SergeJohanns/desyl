import re
import json
import itertools
import pandas as pd
import tensorflow as tf
import tensorflow_decision_forests as tfdf

GOAL_REGEX = re.compile(r"{(.*);} -> {(.*);}")
ASSERTION_REGEX = re.compile(r"(.*); (.*)")
POINTER_REGEX = re.compile(r"<\w+, \d> -> .")
ARRAY_REGEX = re.compile(r"[\w+, \d]")
PREDICATE_REGEX = re.compile(r"(\w+)\((.*)\)")

TREE_FILE = "proof_tree.json"

with open(TREE_FILE, "r") as f:
    tree = json.load(f)


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


def get_rule_node_data(goal: str, node: dict) -> list[dict]:
    row = {
        **get_goal_features(goal),
        "rule": node["rule"],
        "score": int(
            node.get("completed", False)
        ),  # 0 or 1, system will infer probability of leading to completion
    }
    child_rows = list(
        itertools.chain.from_iterable(
            get_goal_node_data(child) for child in node["children"]
        )
    )
    return [row] + child_rows


def get_goal_node_data(node: dict) -> list[dict]:
    if "goal" not in node:
        return list(
            itertools.chain.from_iterable(
                get_goal_node_data(child) for child in node["children"]
            )
        )
    else:
        return list(
            itertools.chain.from_iterable(
                get_rule_node_data(node["goal"], child) for child in node["children"]
            )
        )


def precision(y_true, y_pred):
    true_positives = tf.math.reduce_sum(y_true * y_pred)
    predicted_positives = tf.math.reduce_sum(y_pred)
    return true_positives / (predicted_positives + tf.keras.backend.epsilon())


def recall(y_true, y_pred):
    true_positives = tf.math.reduce_sum(y_true * y_pred)
    actual_positives = tf.math.reduce_sum(y_true)
    return true_positives / (actual_positives + tf.keras.backend.epsilon())


row_list = get_goal_node_data(tree)
train_df = pd.DataFrame(row_list)
train_ds = tfdf.keras.pd_dataframe_to_tf_dataset(
    train_df, label="score", task=tfdf.keras.Task.REGRESSION
)
model = tfdf.keras.RandomForestModel(task=tfdf.keras.Task.REGRESSION)
model.fit(train_ds)
model.compile(metrics=["accuracy", precision, recall])
model.evaluate(train_ds, return_dict=True)
model.save("model")
