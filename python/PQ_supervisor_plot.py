import os
import numpy as np
from matplotlib.collections import LineCollection

from config import COLORS, plt


def load_pq_supervisor(filepath):
    """
    Wczytuje dane P,Q z pliku pg_y1.dat (format promotora)
    Format: p  q  (2 kolumny, bez nagłówka)
    """
    data = np.loadtxt(filepath)
    p = data[:, 0]
    q = data[:, 1]
    return p, q


def extract_s_from_filename(filepath):
    """Wyciąga wartość s z nazwy pliku"""
    import re
    match = re.search(r'od_0_(\d+\.\d+)', os.path.basename(filepath))
    if match:
        return float(match.group(1))
    return None


def window_centers(p, q, window, step):
    """Środki (średnia P i Q) w kolejnych oknach"""
    centers = []
    for start in range(0, len(p) - window + 1, step):
        end = start + window
        centers.append((np.mean(p[start:end]), np.mean(q[start:end])))
    return np.array(centers)


def plot_pq_trajectory(ax, p, q, draw_trajectory_line=True, draw_points=False,
                      draw_window_path=False, window=50, step=50):
    """
    Wykres trajektorii P-Q z ewentualnymi środkami okien
    
    draw_trajectory_line: czy łączyć punkty trajektorii linią (gradient)
    draw_points: czy rysować punkty (jako scatter gdy brak linii)
    draw_window_path: czy łączyć środki okien linią (czerwona)
    """
    if draw_trajectory_line or draw_points:
        points = np.array([p, q]).T.reshape(-1, 1, 2)
        segments = np.concatenate([points[:-1], points[1:]], axis=1)
        n = len(p)
        colors_grad = plt.cm.viridis(np.linspace(0, 1, n - 1))
        
        if draw_trajectory_line:
            lc = LineCollection(segments, colors=colors_grad, linewidths=1.5, alpha=0.7)
            ax.add_collection(lc)
        
        if draw_points and not draw_trajectory_line:
            colors_scatter = plt.cm.viridis(np.linspace(0, 1, len(p)))
            ax.scatter(p, q, c=colors_scatter, s=8, alpha=0.7,
                      edgecolors='white', linewidths=0.2, zorder=1)

    centers = window_centers(p, q, window, step)
    if len(centers) > 0 and (draw_window_path or True):
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


def plot_pq_supervisor(pq_filepath, output_dir='plots/supervisor', 
                       draw_trajectory_line=True, draw_points=False,
                       draw_window_path=False, window=50, step=50):
    """
    Tworzy wykres P-Q z pliku promotora (pg_y1.dat)
    
    Args:
        pq_filepath: ścieżka do pliku pg_y1.dat
        output_dir: katalog dla wyników
        draw_trajectory_line: czy rysować linię trajektorii
        draw_points: czy rysować punkty
        draw_window_path: czy rysować linię środków okien
        window: szerokość okna dla środków
        step: krok okna
    """
    p, q = load_pq_supervisor(pq_filepath)
    s_value = extract_s_from_filename(pq_filepath)
    
    fig, ax = plt.subplots(figsize=(10, 10))
    
    plot_pq_trajectory(ax, p, q, draw_trajectory_line, draw_points, 
                      draw_window_path, window, step)
    
    title = r'\textbf{Trajektoria P-Q (dane promotora)}'
    if s_value is not None:
        title += f'\n$s = {s_value:.5f}$'
    
    ax.set_title(title)
    
    fig.tight_layout()
    
    os.makedirs(output_dir, exist_ok=True)
    
    basename = os.path.basename(pq_filepath).replace('.dat', '')
    suffix = ''
    if draw_trajectory_line:
        suffix += '_line'
    if draw_points:
        suffix += '_points'
    if draw_window_path:
        suffix += f'_win{window}'
    
    output_path = os.path.join(output_dir, f'PQ_supervisor_{basename}{suffix}.png')
    fig.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    
    print(f"Zapisano: {output_path}")
    print(f"  N = {len(p)} punktów")
    if s_value is not None:
        print(f"  s = {s_value:.5f}")


def plot_comparison_all(test01_path, cpp_path, supervisor_path, output_dir='plots/comparison'):
    """
    Porównuje 3 źródła danych na jednym wykresie (3 subploty)
    
    Args:
        test01_path: ścieżka do pq.dat z test01.c (extern_C/my_test/pq.dat)
        cpp_path: ścieżka do p_q_theta_d_*.dat z C++
        supervisor_path: ścieżka do pg_y1.dat od promotora
        output_dir: katalog dla wyników
    """
    from PQ_test01_plot import load_pq_test01, extract_info_from_header
    
    # Wczytaj dane test01
    n1, p1, q1 = load_pq_test01(test01_path)
    c1, Kc1, K1 = extract_info_from_header(test01_path)
    
    # Wczytaj dane C++
    data_cpp = np.loadtxt(cpp_path)
    p2 = data_cpp[:, 0]
    q2 = data_cpp[:, 1]
    
    # Wczytaj dane promotora
    p3, q3 = load_pq_supervisor(supervisor_path)
    
    fig, axes = plt.subplots(1, 3, figsize=(24, 8))
    
    # Lewy: test01.c
    ax = axes[0]
    plot_pq_trajectory(ax, p1, q1, True, False, False, 50, 50)
    title = r'\textbf{test01.c}'
    if c1 is not None:
        title += f'\n$c={c1:.4f}$, $K={K1:.4f}$'
    ax.set_title(title)
    
    # Środek: C++
    ax = axes[1]
    plot_pq_trajectory(ax, p2, q2, True, False, False, 50, 50)
    ax.set_title(r'\textbf{C++ (Twój kod)}')
    
    # Prawy: Promotor
    ax = axes[2]
    plot_pq_trajectory(ax, p3, q3, True, False, False, 50, 50)
    ax.set_title(r'\textbf{Dane promotora (pg\_y1)}')
    
    fig.suptitle('Porównanie: test01.c vs C++ vs Promotor', y=0.98, fontsize=14, weight='bold')
    fig.tight_layout()
    
    output_path = os.path.join(output_dir, 'PQ_comparison_all_3sources.png')
    os.makedirs(output_dir, exist_ok=True)
    fig.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    
    print(f"Zapisano porównanie: {output_path}")
    print(f"  test01.c:  N = {len(p1)}")
    print(f"  C++:       N = {len(p2)}")
    print(f"  Promotor:  N = {len(p3)}")


def main():
    # Wykres dla danych promotora
    supervisor_file = "extern_C/dane/y_266_11_od_0_4.75000_7500_pg_y1.dat"
    
    print("Generowanie wykresu dla danych promotora...")
    
    # Linia trajektorii - zapisz do plots/PQ_4750_promotor_data.png
    p, q = load_pq_supervisor(supervisor_file)
    s_value = extract_s_from_filename(supervisor_file)
    
    fig, ax = plt.subplots(figsize=(10, 10))
    
    plot_pq_trajectory(ax, p, q, draw_trajectory_line=True, draw_points=False, 
                      draw_window_path=False, window=50, step=50)
    
    title = r'\textbf{Trajektoria P-Q (dane promotora)}'
    if s_value is not None:
        title += f'\n$s = {s_value:.5f}$'
    
    ax.set_title(title)
    fig.tight_layout()
    
    os.makedirs('plots', exist_ok=True)
    output_path = 'plots/PQ_4750_promotor_data.png'
    fig.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    
    print(f"Zapisano: {output_path}")
    print(f"  N = {len(p)} punktów")
    if s_value is not None:
        print(f"  s = {s_value:.5f}")


if __name__ == '__main__':
    main()
