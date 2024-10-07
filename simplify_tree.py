import json

JSON_FILE = "proof_tree.json"
OUTPUT_FILE = "simplified_tree.json"
PROOF_ONLY = True

with open(JSON_FILE, "r") as f:
    tree = json.load(f)

def simplify_tree(tree):
    tree["children"] = [child for child in tree["children"] if child["children"]]
    for child in tree["children"]:
        simplify_tree(child)

def simplify_to_proof(tree):
    for child in tree["children"]:
        simplify_to_proof(child)
    tree["children"] = [child for child in tree["children"] if child.get("completed", False)]

if PROOF_ONLY:
    simplify_to_proof(tree)
else:
    simplify_tree(tree)

with open(OUTPUT_FILE, "w") as f:
    json.dump(tree, f, indent=2)
