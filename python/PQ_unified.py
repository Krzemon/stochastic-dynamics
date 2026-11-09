import os
import re

import numpy as np
from matplotlib.collections import LineCollection

from config import COLORS, plt


def load_data(filepath):
    """Wczytuje dane z pliku: p, q, theta, d"""
    data = np.loadtxt(filepath)
    p = data[:, 0]
    q = data[:, 1]
    theta = data[:, 2]
    d = data[:, 3]
    theta_deg = np.degrees(theta) % 360
    return p, q, theta_deg, d


def case_and_s_from_path(filepath):
    """Wyciąga case i wartość s z nazwy pliku"""
    case = os.path.basename(os.path.dirname(filepath))
    match = re.search(r'od_0_(\d+\.\d+)', os.path.basename(filepath))
    s_value = float(match.group(1)) if match else float(case) / 1000
    return case, s_value


def window_centers(p, q, window, step):
    """Środki (średnia P i Q) w kolejnych oknach"""
    centers = []
    for start in range(0, len(p) - window + 1, step):
        end = start + window
        centers.append((np.mean(p[start:end]), np.mean(q[start:end])))
    return np.array(centers)


def plot_pq_trajectory(ax, p, q, draw_points=True, draw_trajectory_line=True, 
                      draw_window_path=True, window=50, step=50):
    """
    Wykres 1: Trajektoria P-Q z ewentualnymi środkami okien
    
    draw_points: czy rysować punkty trajektorii (jako gradient kolorów)
    draw_trajectory_line: czy łączyć punkty trajektorii linią (gradient)
    draw_window_path: czy łączyć środki okien linią (czerwona)
    """
    if draw_points or draw_trajectory_line:
        points = np.array([p, q]).T.reshape(-1, 1, 2)
        segments = np.concatenate([points[:-1], points[1:]], axis=1)
        n = len(p)
        colors_grad = plt.cm.viridis(np.linspace(0, 1, n - 1))
        
        if draw_trajectory_line:
            lc = LineCollection(segments, colors=colors_grad, linewidths=1.5, alpha=0.7)
            ax.add_collection(lc)
        
        if draw_points and not draw_trajectory_line:
            # Jeśli tylko punkty bez linii, rysuj scatter
            colors_scatter = plt.cm.viridis(np.linspace(0, 1, len(p)))
            ax.scatter(p, q, c=colors_scatter, s=8, alpha=0.7,
                      edgecolors='white', linewidths=0.2, zorder=1)

    centers = window_centers(p, q, window, step)
    if len(centers) > 0:
        if draw_window_path:
            ax.plot(centers[:, 0], centers[:, 1], '-', color='red',
                    linewidth=1.2, alpha=0.8, zorder=10)
        ax.plot(centers[:, 0], centers[:, 1], 'o', color='red', markersize=8, zorder=11,
                markeredgecolor='white', markeredgewidth=1,
                label=f'Środki okien (okno={window}, krok={step})')

    margin = max(np.ptp(p), np.ptp(q)) * 0.1
    ax.set_xlim(p.min() - margin, p.max() + margin)
    ax.set_ylim(q.min() - margin, q.max() + margin)
    ax.set_xlabel(r'$P$')
    ax.set_ylabel(r'$Q$')
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    if len(centers) > 0:
        ax.legend(loc='best')
    ax.set_title(r'\textbf{Trajektoria P-Q}')


def plot_distance_vs_angle(ax, theta_deg, d):
    """Wykres 2: Odległość vs Kąt"""
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


def plot_angle_distribution(ax, theta_deg):
    """Wykres 3: Rozkład kątów"""
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


def plot_distance_distribution(ax, d):
    """Wykres 4: Rozkład odległości"""
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


def make_single_plot(filepath, plot_type='trajectory', window=50, step=50, 
                     draw_points=True, draw_trajectory_line=True, draw_window_path=True):
    """
    Tworzy pojedynczy wykres.
    
    plot_type: 'trajectory', 'dist_vs_angle', 'angle_dist', 'distance_dist'
    
    Dla plot_type='trajectory':
        draw_points: czy rysować punkty
        draw_trajectory_line: czy łączyć punkty trajektorii linią
        draw_window_path: czy łączyć środki okien linią
    """
    case, s_value = case_and_s_from_path(filepath)
    p, q, theta_deg, d = load_data(filepath)

    fig, ax = plt.subplots(figsize=(9, 9))

    if plot_type == 'trajectory':
        plot_pq_trajectory(ax, p, q, draw_points, draw_trajectory_line, 
                          draw_window_path, window, step)
    elif plot_type == 'dist_vs_angle':
        plot_distance_vs_angle(ax, theta_deg, d)
    elif plot_type == 'angle_dist':
        plot_angle_distribution(ax, theta_deg)
    elif plot_type == 'distance_dist':
        plot_distance_distribution(ax, d)
    else:
        raise ValueError(f"Nieznany typ wykresu: {plot_type}")

    fig.suptitle(f'$s = {s_value:.3f}$', y=0.98)
    fig.tight_layout()

    os.makedirs(f"plots/{case}", exist_ok=True)
    filename = f"plots/{case}/PQ_{plot_type}_{s_value:.5f}.png"
    fig.savefig(filename, dpi=300, bbox_inches='tight')
    plt.close(fig)
    print(f"Zapisano: {filename}")


def make_2x2_plot(filepath, window=50, step=50, draw_points=True, 
                  draw_trajectory_line=True, draw_window_path=True):
    """
    Tworzy wykres 2x2 ze wszystkimi analizami
    
    draw_points: czy rysować punkty trajektorii
    draw_trajectory_line: czy łączyć punkty trajektorii linią
    draw_window_path: czy łączyć środki okien linią
    """
    case, s_value = case_and_s_from_path(filepath)
    p, q, theta_deg, d = load_data(filepath)

    fig, axes = plt.subplots(2, 2, figsize=(16, 14))

    plot_pq_trajectory(axes[0, 0], p, q, draw_points, draw_trajectory_line, 
                      draw_window_path, window, step)
    plot_distance_vs_angle(axes[0, 1], theta_deg, d)
    plot_angle_distribution(axes[1, 0], theta_deg)
    plot_distance_distribution(axes[1, 1], d)

    fig.suptitle(f'Analiza dla $s = {s_value:.3f}$', y=0.995)
    fig.tight_layout()

    os.makedirs(f"plots/{case}", exist_ok=True)
    filename = f"plots/{case}/PQ_analysis_2x2_{s_value:.5f}.png"
    fig.savefig(filename, dpi=300, bbox_inches='tight')
    plt.close(fig)
    print(f"Zapisano: {filename}")


def main():
    filepath = "data/4750/p_q_theta_d_y0_last_7000_y_266_11_od_0_4.75000.dat"
    
    # Przykłady pojedynczych wykresów trajektorii z różnymi opcjami
    # 1. Tylko linia trajektorii (gradient) - bez punktów
    # make_single_plot(filepath, 'trajectory', window=50, step=50, 
    #                 draw_points=False, draw_trajectory_line=True, draw_window_path=False)
    
    # 2. Tylko punkty - bez łączącej linii
    make_single_plot(filepath, 'trajectory', window=50, step=50, 
                     draw_points=True, draw_trajectory_line=False, draw_window_path=False)
    
    # 3. Pełna wersja: punkty + linia trajektorii + środki okien z linią
    # make_single_plot(filepath, 'trajectory', window=50, step=50, 
    #                 draw_points=True, draw_trajectory_line=True, draw_window_path=True)
    
    # Pozostałe wykresy
    # make_single_plot(filepath, 'dist_vs_angle')
    # make_single_plot(filepath, 'angle_dist')
    # make_single_plot(filepath, 'distance_dist')
    
    # Wykres 2x2
    # make_2x2_plot(filepath, window=50, step=50, 
    #               draw_points=True, draw_trajectory_line=False, draw_window_path=True)


if __name__ == '__main__':
    main()
