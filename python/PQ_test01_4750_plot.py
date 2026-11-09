import os
import numpy as np
from matplotlib.collections import LineCollection

from config import COLORS, plt


def load_pq_test01(filepath):
    """
    Wczytuje dane P,Q z pliku test01.c (pq.dat)
    Format: # komentarze
            n  p  q
    """
    data = []
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            parts = line.split()
            if len(parts) >= 3:
                n, p, q = float(parts[0]), float(parts[1]), float(parts[2])
                data.append((n, p, q))
    
    data = np.array(data)
    n = data[:, 0]
    p = data[:, 1]
    q = data[:, 2]
    
    return n, p, q


def extract_info_from_header(filepath):
    """Wyciąga informacje o c i K z nagłówka pliku pq.dat"""
    c_value = None
    Kc_value = None
    K_value = None
    
    with open(filepath, 'r') as f:
        for line in f:
            if line.startswith('# c ='):
                parts = line.split()
                for i, part in enumerate(parts):
                    if part == 'c' and i+2 < len(parts):
                        c_value = float(parts[i+2])
                    if part == 'K_c' and i+2 < len(parts):
                        Kc_value = float(parts[i+2])
                    if part == 'K' and i+2 < len(parts):
                        K_value = float(parts[i+2])
                break
    
    return c_value, Kc_value, K_value


def window_centers(p, q, window, step):
    """Środki (średnia P i Q) w kolejnych oknach"""
    centers = []
    for start in range(0, len(p) - window + 1, step):
        end = start + window
        centers.append((np.mean(p[start:end]), np.mean(q[start:end])))
    return np.array(centers)


def plot_pq_trajectory(ax, p, q, draw_trajectory_line=True, draw_points=False,
                      draw_window_path=False, window=50, step=50):
    """Wykres trajektorii P-Q"""
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
    if len(centers) > 0 and draw_window_path:
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
    if len(centers) > 0 and draw_window_path:
        ax.legend(loc='best')


def main():
    # Dane z test01_dynamic dla danych promotora (fin_y_266_11_od_0_4.75000.out)
    pq_file = "extern_C/my_test_4750_promotor/pq.dat"
    
    print("Generowanie wykresu dla test01_dynamic (dane promotora 4750)...")
    
    n, p, q = load_pq_test01(pq_file)
    c_value, Kc_value, K_value = extract_info_from_header(pq_file)
    
    fig, ax = plt.subplots(figsize=(10, 10))
    
    plot_pq_trajectory(ax, p, q, draw_trajectory_line=True, draw_points=False, 
                      draw_window_path=False, window=50, step=50)
    
    title = r'\textbf{Trajektoria P-Q (test01\_dynamic na danych promotora)}'
    if c_value is not None:
        title += f'\n$c = {c_value:.4f}$'
    if K_value is not None:
        title += f', $K = {K_value:.4f}$'
    if Kc_value is not None:
        title += f', $K_c = {Kc_value:.4f}$'
    
    ax.set_title(title)
    fig.tight_layout()
    
    os.makedirs('plots', exist_ok=True)
    output_path = 'plots/PQ_4750_test01_promotor_data.png'
    fig.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    
    print(f"Zapisano: {output_path}")
    print(f"  N = {len(p)} punktów")
    if c_value is not None:
        print(f"  c = {c_value:.6f}")
    if K_value is not None:
        print(f"  K = {K_value:.6f}")


if __name__ == '__main__':
    main()
