import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

df = pd.read_csv('benchmark.csv')
df.sort_values(by=['dfs_nodes_expanded'], inplace=True)

def nodes_expanded_bar_chart():
    number_of_algos = 3
    bar_width = 1 / (number_of_algos + 1)
    xs = np.arange(len(df['example']))
    plt.bar(xs, df['dfs_nodes_expanded'], bar_width, color='blue', label='DFS')
    plt.bar(xs + bar_width, df['bfs_nodes_expanded'], bar_width, color='red', label='BFS')
    plt.bar(xs + 2 * bar_width, df['best_nodes_expanded'], bar_width, color='green', label='Best')
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
    number_of_algos = 2
    bar_width = 1 / (number_of_algos + 1)
    xs = np.arange(len(df['example']))
    dfs_time = df['dfs_time'].copy()
    dfs_time[dfs_time == 10] = 0
    plt.bar(xs, dfs_time, bar_width, color='blue', label='DFS')
    bfs_time = df['bfs_time'].copy()
    bfs_time[bfs_time == 10] = 0
    plt.bar(xs + bar_width, bfs_time, bar_width, color='red', label='BFS')
    bfs_time = df['best_time'].copy()
    bfs_time[bfs_time == 10] = 0
    plt.bar(xs + 2 * bar_width, bfs_time, bar_width, color='green', label='Best')
    plt.xticks(xs, df['example'], rotation=90)
    plt.yscale('log')
    plt.ylabel('Nodes expanded (fewer is faster)')
    plt.title('Nodes expanded for synthesis')
    plt.legend()
    plt.tight_layout()
    plt.show()

times_bar_chart()
