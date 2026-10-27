import os
import re

import numpy as np
from matplotlib.collections import LineCollection

from config import COLORS, plt


def load_data(filepath):
    data = np.loadtxt(filepath)

    p = data[:, 0]
    q = data[:, 1]
    theta = data[:, 2]
    d = data[:, 3]

    theta_deg = np.degrees(theta) % 360

    return p, q, theta_deg, d


def case_and_s_from_path(filepath):
    # data/7889/p_q_theta_d_y0_last_7000_y_266_11_od_0_7.88900.dat -> ("7889", 7.889)
    case = os.path.basename(os.path.dirname(filepath))
    match = re.search(r'od_0_(\d+\.\d+)', os.path.basename(filepath))
    s_value = float(match.group(1)) if match else float(case) / 1000
    return case, s_value


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

    # center_x = np.mean(p)
    # center_y = np.mean(q)
    # ax.plot(center_x, center_y, 'o', color='red', markersize=10, zorder=11, markeredgecolor='white', 
    #         markeredgewidth=1, label=f'Środek = ({center_x:.2f}, {center_y:.2f})')

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

    ax.set_xlabel(r'Kąt $\theta$ $[^\circ]$')
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

    ax.set_xlabel(r'Kąt $\theta$ $[^\circ]$')
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


def make_pq_analysis(filepath):
    case, s_value = case_and_s_from_path(filepath)

    p, q, theta_deg, d = load_data(filepath)

    os.makedirs(f"plots/{case}", exist_ok=True)

    fig_analysis = plot_2x2_analysis(p, q, theta_deg, d)
    fig_analysis.suptitle(f'Analiza dla $s = {s_value:.3f}$', y=0.995)
    filename_analysis = f"plots/{case}/PQ_analysis_{s_value:.5f}.png"
    fig_analysis.savefig(filename_analysis, dpi=300, bbox_inches='tight')
    plt.close(fig_analysis)
    print(f"Zapisano: {filename_analysis}")


def plot_PQ():
    pass


def main():
    # make_pq_analysis("data_out/7889/p_q_theta_d_y0_last_7000_y_266_11_od_0_7.88900.dat")
    plot_PQ("data_out/7889/p_q_theta_d_y0_last_7000_y_266_11_od_0_7.88900.dat")


if __name__ == '__main__':
    main()