import os
import subprocess
from alive_progress import alive_bar

EXAMPLES_DIR = 'examples'
SOLUTIONS_DIR = 'example-solutions'

CHECKMARK = u'\u2714'
CROSSMARK = u'\u2718'

solutions = os.listdir(SOLUTIONS_DIR)
solutions.sort()
with alive_bar(len(solutions), enrich_print=False) as bar:
    for file in solutions:
        bar.text(f'Running {file}')
        result = subprocess.run(
            ['./build/apps/desyl', '--file', f'{EXAMPLES_DIR}/{file}'],
            capture_output=True
        ).stdout.decode('utf-8')

        with open(f'{SOLUTIONS_DIR}/{file}', 'r') as f:
            correct = f.read()

        if result == correct:
            print(CHECKMARK, file, 'passed')
        else:
            print(CROSSMARK, file, 'failed')
            print(f'Expected:\n{correct}')
            print(f'Got:\n{result}')

        bar()
