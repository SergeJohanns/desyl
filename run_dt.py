"""
This script is used to run the decision tree model in desyl.
"""

import sys
import json
import ydf

print(sys.argv[1], file=sys.stderr)
model = ydf.load_model("model")
dataset = {k: [v] for k, v in json.loads(sys.argv[1]).items()}
print(f"{1 - model.predict(dataset)[0]:f}")
