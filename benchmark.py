import os
import csv
import json
import time
import itertools
from enum import Enum
from subprocess import STDOUT, check_output, TimeoutExpired
from alive_progress import alive_bar

EXAMPLES_DIR = 'examples'
SOLUTIONS_DIR = 'example-solutions'
TREEOUT = 'proof_tree.json'
NO_RESULT = "No value\n"

CHECKMARK = u'\u2714'
CROSSMARK = u'\u2718'


class ResultType(Enum):
    SOLVED = 'solved'
    TIMEOUT = 'timeout'
    NO_RESULT = 'no_result'


class Synthesizer:
    def __init__(self, algo: str = None, depth: int = None, print_tree: bool = False):
        self.algo = algo
        self.depth = depth
        self.print_tree = print_tree

    def synthesize(self, file_path: str, timeout: int = None) -> tuple[ResultType, str, float]:
        command = ['./build/apps/desyl', '--file', file_path]
        if self.algo is not None:
            command.extend(['--algo', self.algo])
        if self.print_tree:
            command.extend(['--treeout', TREEOUT])
        if self.depth is not None:
            command.extend(['--depth', str(self.depth)])
        try:
            start = time.time()
            result = check_output(command, stderr=STDOUT, timeout=timeout).decode('utf-8')
            duration = time.time() - start
            return (ResultType.NO_RESULT if result == NO_RESULT else ResultType.SOLVED, result, duration)
        except TimeoutExpired as e:
            return (ResultType.TIMEOUT, str(e), timeout)


def get_answer(file: str, result: str) -> str:
    with open(f'{SOLUTIONS_DIR}/{file}', 'r') as f:
        return f.read()


def check_all_examples():
    solutions = os.listdir(SOLUTIONS_DIR)
    solutions.sort()
    synthesizer = Synthesizer()
    
    with alive_bar(len(solutions), enrich_print=False) as bar:
        for file in solutions:
            bar.text(f'Running {file}')
            _, result, _ = synthesizer.synthesize(f'{EXAMPLES_DIR}/{file}')
            if (correct := get_answer(file, result)) == result:
                print(CHECKMARK, file, 'passed')
            else:
                print(CROSSMARK, file, 'failed')
                print(f'Expected:\n{correct}')
                print(f'Got:\n{result}')
            bar()


def get_csv_header(algorithms: list) -> list:
    header = ['example']
    for algo in algorithms:
        header.extend([f'{algo}_result', f'{algo}_time', f'{algo}_nodes_expanded'])
    return header


def count_expanded_nodes(subtree: dict) -> int:
    count = sum(count_expanded_nodes(child) for child in subtree['children'])
    if "expanded" in subtree:
        count += 1
    return count


def get_expanded_nodes() -> int:
    with open(TREEOUT, 'r') as f:
        tree = json.load(f)
        return count_expanded_nodes(tree)


def benchmark_all_algorithms(algorithms: list, depth: int = 12, timeout: int = 10):
    examples = [file for file in os.listdir(EXAMPLES_DIR) if file.endswith('.sep')]
    examples.sort()
    synthesizers = [Synthesizer(algo=algo, depth=depth, print_tree=True) for algo in algorithms]
    
    with alive_bar(len(examples), enrich_print=False) as bar, open('benchmark.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(get_csv_header(algorithms))
        for file in examples:
            print(f'Running {file}')
            results = [file]
            for algo, synthesizer in zip(algorithms, synthesizers):
                bar.text(f'Running {algo} algorithm')
                result_type, result, time = synthesizer.synthesize(f'{EXAMPLES_DIR}/{file}', timeout=timeout)
                expanded_nodes = get_expanded_nodes()
                results.extend([result_type.value, time, expanded_nodes])
            writer.writerow(results)
            bar()


benchmark_all_algorithms(["dfs", "bfs"])
