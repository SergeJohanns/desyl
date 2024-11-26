import json

JSON_FILE = "proof_tree.json"
OUTPUT_FILE = "simplified_tree.json"
PROOF_ONLY = True


def simplify_tree(tree):
    tree["children"] = [child for child in tree["children"] if child["children"]]
    for child in tree["children"]:
        simplify_tree(child)


def simplify_to_proof(tree):
    for child in tree["children"]:
        simplify_to_proof(child)
    tree["children"] = [
        child for child in tree["children"] if child.get("completed", False)
    ]


def simplify_file(tree_file, output_file, proof_only=True):
    with open(tree_file, "r") as f:
        tree = json.load(f)

    if proof_only:
        simplify_to_proof(tree)
    else:
        simplify_tree(tree)

    with open(output_file, "w") as f:
        json.dump(tree, f, indent=2)


if __name__ == "__main__":
    simplify_file(JSON_FILE, OUTPUT_FILE, PROOF_ONLY)
