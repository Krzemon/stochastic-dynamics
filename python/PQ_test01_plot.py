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
                # Format: # c = 0.123  K_c = 0.456  K = 0.789
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


def plot_pq_trajectory(ax, p, q, draw_trajectory_line=True, draw_points=True,
                      draw_window_path=True, window=50, step=50):
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


def plot_pq_test01(pq_filepath, output_dir='plots', 
                   draw_trajectory_line=True, draw_points=False,
                   draw_window_path=False, window=50, step=50):
    """
    Tworzy wykres P-Q z pliku test01.c (pq.dat)
    
    Args:
        pq_filepath: ścieżka do pliku pq.dat
        output_dir: katalog dla wyników
        draw_trajectory_line: czy rysować linię trajektorii
        draw_points: czy rysować punkty
        draw_window_path: czy rysować linię środków okien
        window: szerokość okna dla środków
        step: krok okna
    """
    n, p, q = load_pq_test01(pq_filepath)
    c_value, Kc_value, K_value = extract_info_from_header(pq_filepath)
    
    fig, ax = plt.subplots(figsize=(10, 10))
    
    plot_pq_trajectory(ax, p, q, draw_trajectory_line, draw_points, 
                      draw_window_path, window, step)
    
    # Tytuł z informacjami o c i K
    title = r'\textbf{Trajektoria P-Q (test01.c)}'
    if c_value is not None:
        title += f'\n$c = {c_value:.4f}$'
    if K_value is not None:
        title += f', $K = {K_value:.4f}$'
    if Kc_value is not None:
        title += f', $K_c = {Kc_value:.4f}$'
    
    ax.set_title(title)
    
    fig.tight_layout()
    
    os.makedirs(output_dir, exist_ok=True)
    
    # Nazwa pliku wyjściowego
    basename = os.path.basename(pq_filepath).replace('.dat', '')
    suffix = ''
    if draw_trajectory_line:
        suffix += '_line'
    if draw_points:
        suffix += '_points'
    if draw_window_path:
        suffix += f'_win{window}'
    
    output_path = os.path.join(output_dir, f'PQ_test01_{basename}{suffix}.png')
    fig.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    
    print(f"Zapisano: {output_path}")
    print(f"  N = {len(p)} punktów")
    if c_value is not None:
        print(f"  c = {c_value:.6f}")
    if K_value is not None:
        print(f"  K = {K_value:.6f}")


def plot_comparison(pq_test01_path, pq_cpp_path, output_dir='plots',
                   window=50, step=50):
    """
    Porównuje wyniki test01.c vs C++ na jednym wykresie (2 subploty)
    
    Args:
        pq_test01_path: ścieżka do pq.dat z test01.c
        pq_cpp_path: ścieżka do p_q_theta_d_*.dat z C++
        output_dir: katalog dla wyników
    """
    # Wczytaj dane test01
    n1, p1, q1 = load_pq_test01(pq_test01_path)
    c1, Kc1, K1 = extract_info_from_header(pq_test01_path)
    
    # Wczytaj dane C++
    data_cpp = np.loadtxt(pq_cpp_path)
    p2 = data_cpp[:, 0]
    q2 = data_cpp[:, 1]
    
    fig, axes = plt.subplots(1, 2, figsize=(18, 9))
    
    # Lewy: test01.c
    ax = axes[0]
    plot_pq_trajectory(ax, p1, q1, True, False, False, window, step)
    title = r'\textbf{test01.c}'
    if c1 is not None:
        title += f'\n$c = {c1:.4f}$, $K = {K1:.4f}$'
    ax.set_title(title)
    
    # Prawy: C++
    ax = axes[1]
    plot_pq_trajectory(ax, p2, q2, True, False, False, window, step)
    ax.set_title(r'\textbf{C++ (phase\_space\_analysis)}')
    
    fig.suptitle('Porównanie: test01.c vs C++', y=0.98, fontsize=14, weight='bold')
    fig.tight_layout()
    
    output_path = os.path.join(output_dir, 'PQ_comparison_test01_vs_cpp.png')
    os.makedirs(output_dir, exist_ok=True)
    fig.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    
    print(f"Zapisano porównanie: {output_path}")
    print(f"  test01.c: N = {len(p1)}")
    print(f"  C++:      N = {len(p2)}")


def main():
    # Przykład 1: Wykres dla pq.dat z test01.c
    # pq_file = "extern_C/my_test/pq.dat"

    # pq_file = "extern_C/my_test_4750_moje/pq.dat"
    pq_file = "extern_C/my_test_4750_promotor/pq.dat"
    
    
    # Różne warianty wykresu
    print("Generowanie wykresów dla test01.c...")
    
    # Tylko linia trajektorii
    # plot_pq_test01(pq_file, output_dir='plots/test01',
    #                draw_trajectory_line=True, draw_points=False, draw_window_path=False)
    
    # Tylko punkty
    # plot_pq_test01(pq_file, output_dir='plots/test01_4750_moje',
    #                draw_trajectory_line=False, draw_points=True, draw_window_path=False)
    
    plot_pq_test01(pq_file, output_dir='plots/test01_4750_promotor',
                   draw_trajectory_line=False, draw_points=True, draw_window_path=False)



    # Linia + środki okien
    # plot_pq_test01(pq_file, output_dir='plots/test01',
    #                draw_trajectory_line=True, draw_points=False, 
    #                draw_window_path=True, window=50, step=50)
    
    # Przykład 2: Porównanie test01.c vs C++
    # (odkomentuj jak będziesz miał oba pliki)
    # plot_comparison(
    #     "extern_C/my_test/pq.dat",
    #     "data/7889/p_q_theta_d_y0_last_7000_y_266_11_od_0_7.88900.dat",
    #     output_dir='plots/comparison'
    # )


if __name__ == '__main__':
    main()
