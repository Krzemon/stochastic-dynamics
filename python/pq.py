import os
import numpy as np

from config import COLORS, plt

CASES = ["4750", "6000", "7500", "8390"]
N_COLS = 5
WINDOW = 50
STEP = 50
DATA_DIR = "data"
PLOTS_DIR = "plots"
SHOW_CENTERS = True


def load_meta(filepath):
    with open(filepath) as f:
        line = f.readline().strip().lstrip("# ")
    items = line.replace("=", " ").split()
    parts = dict(zip(items[::2], items[1::2]))
    return float(parts["c"]), float(parts["K_c"])


def load_data(filepath):
    data = np.loadtxt(filepath, comments="#")
    return data[:, 1], data[:, 2]


def window_centers(p, q, window, step):
    centers = []
    for start in range(0, len(p) - window + 1, step):
        end = start + window
        centers.append((np.mean(p[start:end]), np.mean(q[start:end])))
    return np.array(centers)


def plot_pq_trajectory(ax, p, q, title=""):
    n = len(p)
    sc = ax.scatter(p, q, c=np.arange(n), cmap='viridis', s=8, alpha=0.7,
                    edgecolors='white', linewidths=0.2, zorder=1)
    plt.colorbar(sc, ax=ax, fraction=0.046, pad=0.04) # label=r'$n$'
    
    if SHOW_CENTERS:
        centers = window_centers(p, q, WINDOW, STEP)
        if len(centers) > 0:
            ax.plot(centers[:, 0], centers[:, 1], 'o', color='red', markersize=8, zorder=11,
                    markeredgecolor='white', markeredgewidth=1,
                    label=fr'Środki okien (okno={WINDOW}, krok={STEP})')
            ax.legend(loc='best')

    margin = max(np.ptp(p), np.ptp(q)) * 0.1
    ax.set_xlim(p.min() - margin, p.max() + margin)
    ax.set_ylim(q.min() - margin, q.max() + margin)
    ax.set_xlabel(r'$P$')
    ax.set_ylabel(r'$Q$')
    # ax.set_aspect('equal')
    ax.set_box_aspect(1)
    ax.grid(True, alpha=0.3)
    ax.set_title(title)


def main():
    # for case in CASES:
    for case in ["7889"]:
        for j in range(1, N_COLS + 1):
            filepath = os.path.join(DATA_DIR, case, f"pq_col_{j}.dat")
            p, q = load_data(filepath)

            c, kc = load_meta(filepath)
            title = fr'$\varphi = y_{{{j}}}$,\quad $c={c}$,\quad $K_c={kc}$'
            fig, ax = plt.subplots(figsize=(9, 9))
            plot_pq_trajectory(ax, p, q, title=title)
            fig.tight_layout()

            os.makedirs(f"{PLOTS_DIR}/{case}", exist_ok=True)
            filename = f"{PLOTS_DIR}/{case}/PQ_col{j}.png"
            fig.savefig(filename, dpi=300, bbox_inches='tight')
            plt.close(fig)
            print(f"Zapisano: {filename}")


if __name__ == '__main__':
    main()
