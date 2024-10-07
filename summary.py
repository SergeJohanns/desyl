import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

df = pd.read_csv('benchmark.csv')
df.sort_values(by=['dfs_nodes_expanded'], inplace=True)

ALGO_COLORS = [('dfs', 'blue'), ('bfs', 'red'), ('best', 'green'), ('rules', 'purple')]

def nodes_expanded_bar_chart():
    number_of_algos = 4
    bar_width = 1 / (number_of_algos + 1)
    xs = np.arange(len(df['example']))
    for i, (algo, color) in enumerate(ALGO_COLORS):
        plt.bar(xs + i * bar_width, df[f'{algo}_nodes_expanded'], bar_width, color=color, label=algo)
    plt.xticks(xs, df['example'], rotation=90)
    plt.yscale('log')
    plt.ylabel('Nodes expanded (fewer is faster)')
    # Use comma notation for y-axis
    plt.gca().yaxis.set_major_formatter(plt.matplotlib.ticker.StrMethodFormatter('{x:,.0f}'))
    plt.title('Nodes expanded for synthesis')
    plt.legend()
    plt.tight_layout()
    plt.show()

nodes_expanded_bar_chart()

def times_bar_chart():
    number_of_algos = 4
    bar_width = 1 / (number_of_algos + 1)
    xs = np.arange(len(df['example']))
    for i, (algo, color) in enumerate(ALGO_COLORS):
        algo_time = df[f'{algo}_time'].copy()
        algo_time[algo_time == 10] = 0
        plt.bar(xs + i * bar_width, algo_time, bar_width, color=color, label=algo)
    plt.xticks(xs, df['example'], rotation=90)
    plt.yscale('log')
    plt.ylabel('Time (s)')
    plt.title('Time taken for synthesis')
    plt.legend()
    plt.tight_layout()
    plt.show()

times_bar_chart()
