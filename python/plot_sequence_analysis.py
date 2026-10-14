import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
import os

from config import COLORS, plt


# ==================== Wzorce ====================

ALFA_PATTERNS = [
    [6, 6, 7, 6, 6, 6, 7],     # alfa_1
    [6, 6, 6, 7, 6, 6, 7],     # alfa_2
    [6, 6, 6, 7, 6, 6, 6, 7]   # alfa_3
]

BETA_PATTERNS = [
    [2]            + [3] * 14, # beta_1
    [3]      + [2] + [3] * 13, # beta_2
    [3, 3]   + [2] + [3] * 12, # beta_3
    [3] * 3  + [2] + [3] * 11, # beta_4
    [3] * 4  + [2] + [3] * 10, # beta_5
    [3] * 5  + [2] + [3] * 9,  # beta_6
    [3] * 6  + [2] + [3] * 8,  # beta_7
    [3] * 7  + [2] + [3] * 7,  # beta_8
    [3] * 8  + [2] + [3] * 6,  # beta_9
    [3] * 9  + [2] + [3] * 5,  # beta_10
    [3] * 10 + [2] + [3] * 4,  # beta_11
    [3] * 11 + [2] + [3] * 3,  # beta_12
    [3] * 12 + [2] + [3, 3],   # beta_13
    [3] * 13 + [2] + [3],      # beta_14
    [3] * 14 + [2],            # beta_15
    [3] * 15                   # beta_16
]


def load_data(s_value):
    s_int = int(s_value * 100)
    filename = f"data/{s_int}/alpha_seq_s_{s_value:.2f}.dat"
    data = np.loadtxt(filename)

    p = data[:, 0]
    q = data[:, 1]
    theta = data[:, 2]
    d = data[:, 3]

    theta_deg = np.degrees(theta) % 360

    return p, q, theta_deg, d


def identify_runs(theta_deg):
    runs = []
    current_run_length = 1

    for i in range(1, len(theta_deg)):
        diff = theta_deg[i] - theta_deg[i-1]

        if 0 < diff < 180:
            current_run_length += 1
        else:
            if current_run_length > 0:
                runs.append(current_run_length)
            current_run_length = 1

    if current_run_length > 0:
        runs.append(current_run_length)

    return runs


def convert_run_to_angle_index(runs, run_index):
    angle_idx = 0
    for i in range(run_index):
        angle_idx += runs[i]
    return angle_idx


def find_single_patterns(runs, patterns, pattern_names):
    found = {name: [] for name in pattern_names}

    for pattern, name in zip(patterns, pattern_names):
        i = 0
        while i <= len(runs) - len(pattern):
            if runs[i:i+len(pattern)] == pattern:
                run_start = i
                run_end = i + len(pattern)
                angle_start = convert_run_to_angle_index(runs, run_start)
                angle_end = convert_run_to_angle_index(runs, run_end)

                found[name].append((run_start, run_end, angle_start, angle_end))
                i += len(pattern)
            else:
                i += 1

    return found


# ==================== Wykresy ====================

def plot_single_block(theta_deg, runs, block_name, block_run_idx, block_len, context_angles=10):
    angle_start = convert_run_to_angle_index(runs, block_run_idx)
    angle_end = angle_start + block_len

    plot_start = max(0, angle_start - context_angles)
    plot_end = min(len(theta_deg), angle_end + context_angles)

    indices = np.arange(plot_start, plot_end)
    angles = theta_deg[plot_start:plot_end]

    colors = []
    for i in indices:
        if angle_start <= i < angle_end:
            colors.append(COLORS[0])
        else:
            colors.append('gray')

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.plot(indices, angles, '-', color='white', linewidth=0.8, alpha=0.3, zorder=1)
    ax.scatter(indices, angles, c=colors, s=50, alpha=0.9,
               edgecolors='white', linewidths=0.8, zorder=2)

    ax.axvline(angle_start, color='yellow', linewidth=1.5, linestyle='--', alpha=0.7)
    ax.axvline(angle_end, color='red', linewidth=1.5, linestyle='--', alpha=0.7)

    ax.set_xlim(plot_start - 1, plot_end + 1)
    ax.set_ylim(-10, 370)
    ax.set_xlabel(r'Indeks kąta')
    ax.set_ylabel(r'Kąt $\theta$ [°]')
    ax.grid(True, alpha=0.3)
    ax.set_yticks(np.arange(0, 361, 45))
    ax.axhline(0, color='white', linewidth=0.8, linestyle='--', alpha=0.3)
    ax.axhline(360, color='white', linewidth=0.8, linestyle='--', alpha=0.3)

    ax.set_title(f'Blok {block_name}')

    fig.tight_layout()
    return fig


def plot_2x2_analysis(p, q, theta_deg, d):
    fig, axes = plt.subplots(2, 2, figsize=(16, 14))

    # (0,0) Trajektoria P-Q
    ax = axes[0, 0]
    points = np.array([p, q]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)
    n = len(p)
    colors_grad = plt.cm.viridis(np.linspace(0, 1, n - 1))
    lc = LineCollection(segments, colors=colors_grad, linewidths=1.5, alpha=0.7)
    ax.add_collection(lc)

    center_x = np.mean(p)
    center_y = np.mean(q)
    ax.plot(center_x, center_y, 'o', color='red', markersize=10, zorder=11,
            markeredgecolor='white', markeredgewidth=1,
            label=f'Środek = ({center_x:.2f}, {center_y:.2f})')

    margin = max(np.ptp(p), np.ptp(q)) * 0.1
    ax.set_xlim(p.min() - margin, p.max() + margin)
    ax.set_ylim(q.min() - margin, q.max() + margin)
    ax.set_xlabel(r'$P$')
    ax.set_ylabel(r'$Q$')
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best')
    ax.set_title(r'\textbf{Trajektoria P-Q}')

    # (0,1) Odległości vs kąty
    ax = axes[0, 1]
    step = max(1, len(theta_deg) // 1000)
    for i in range(0, len(theta_deg) - 1, step):
        ax.plot([theta_deg[i], theta_deg[i+1]], [d[i], d[i+1]],
                color='gray', alpha=0.2, linewidth=0.5, zorder=1)

    colors_scatter = plt.cm.viridis(np.linspace(0, 1, len(theta_deg[::step])))
    ax.scatter(theta_deg[::step], d[::step], c=colors_scatter, s=20, alpha=0.7,
               zorder=2, edgecolors='white', linewidths=0.3)

    ax.set_xlabel(r'Kąt $\theta$ [°]')
    ax.set_ylabel(r'Odległość $d$')
    ax.set_xlim(0, 360)
    ax.grid(True, alpha=0.3)
    ax.set_title(r'\textbf{Odległość vs Kąt}')
    ax.set_xticks(np.arange(0, 361, 45))

    # (1,0) Rozkład kątów
    ax = axes[1, 0]
    unique_angles, counts_a = np.unique(np.round(theta_deg, 1), return_counts=True)
    probs = counts_a / len(theta_deg)

    ax.scatter(unique_angles, probs, c=COLORS[1], s=50, alpha=0.8,
               edgecolors='white', linewidths=0.5)

    ax.set_xlabel(r'Kąt $\theta$ [°]')
    ax.set_ylabel(r'Prawdopodobieństwo')
    ax.set_xlim(0, 360)
    ax.grid(True, alpha=0.3)
    ax.set_title(r'\textbf{Rozkład kątów}')
    ax.set_xticks(np.arange(0, 361, 45))

    # (1,1) Rozkład odległości
    ax = axes[1, 1]
    unique_dist, counts_d = np.unique(np.round(d, 2), return_counts=True)
    probs = counts_d / len(d)

    mean_d = np.mean(d)
    std_d = np.std(d)
    ax.scatter(unique_dist, probs, c=COLORS[2], s=50, alpha=0.8,
               edgecolors='white', linewidths=0.5,
               label=f'd = {mean_d:.3f} ± {std_d:.3f}')

    ax.set_xlabel(r'Odległość $d$')
    ax.set_ylabel(r'Prawdopodobieństwo')
    ax.grid(True, alpha=0.3)
    ax.set_title(r'\textbf{Rozkład odległości}')
    ax.legend(loc='best')

    fig.tight_layout()
    return fig


# ==================== Funkcje główne ====================

def search_single_patterns(s_value):
    s_int = int(s_value * 100)

    p, q, theta_deg, d = load_data(s_value)
    runs = identify_runs(theta_deg)

    alfa_names = ['alfa_1', 'alfa_2', 'alfa_3']
    found = find_single_patterns(runs, ALFA_PATTERNS, alfa_names)

    for name, occurrences in found.items():
        if occurrences:
            print(f"{name}: {len(occurrences)} wystąpień")

    os.makedirs(f"plots/{s_int}", exist_ok=True)

    fig_analysis = plot_2x2_analysis(p, q, theta_deg, d)
    fig_analysis.suptitle(f'Analiza dla $s = {s_value:.2f}$', y=0.995)
    filename_analysis = f"plots/{s_int}/PQ_analysis_{s_int}.png"
    fig_analysis.savefig(filename_analysis, dpi=300, bbox_inches='tight')
    plt.close(fig_analysis)
    print(f"Zapisano: {filename_analysis}")


def main():
    s_value = 83.90
    search_single_patterns(s_value)


if __name__ == '__main__':
    main()
