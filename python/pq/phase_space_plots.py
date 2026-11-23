import os
import numpy as np

from config.style import COLORS, plt, save_me

CASES = ["4750", "6000", "7500", "8390"]
N_COLS = 5
DATA_DIR = "data"


def load_meta(filepath):
    with open(filepath) as f:
        line = f.readline().strip().lstrip("# ")
    items = line.replace("=", " ").split()
    parts = dict(zip(items[::2], items[1::2]))
    return float(parts["c"]), float(parts["K_c"]), float(parts["K"])


def load_pq(filepath):
    data = np.loadtxt(filepath, comments="#")
    return data[:, 0], data[:, 1]


def load_angle_distance(filepath):
    data = np.loadtxt(filepath, comments="#")
    theta_deg = np.degrees(data[:, 0]) % 360
    d = data[:, 1]
    return theta_deg, d


def _meta_title(case, col, c, kc, k):
    return (rf'$s={int(case)/1000:.3f}$,\quad $\varphi=y_{{{col}}}$,'
            rf'\quad $c={c:.4f}$,\quad $K={k:.4f}$')


def _save(fig, case, name):
    os.makedirs(f"plots/{case}", exist_ok=True)
    path = f"plots/{case}/{name}"
    fig.savefig(path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    print(f"Zapisano: {path}")


# ==================== Funkcje rysujące ====================

def plot_pq(ax, p, q, title=""):
    n = len(p)
    sc = ax.scatter(p, q, c=np.arange(n), cmap='viridis', s=8, alpha=0.7,
                    edgecolors='white', linewidths=0.2, zorder=1)
    plt.colorbar(sc, ax=ax, fraction=0.046, pad=0.04)
    margin = max(np.ptp(p), np.ptp(q)) * 0.1
    ax.set_xlim(p.min() - margin, p.max() + margin)
    ax.set_ylim(q.min() - margin, q.max() + margin)
    ax.set_xlabel(r'$P$')
    ax.set_ylabel(r'$Q$')
    ax.set_box_aspect(1)
    ax.grid(True, alpha=0.3)
    ax.set_title(title)


def plot_dist_vs_angle(ax, theta_deg, d, title=""):
    ax.scatter(theta_deg, d, c=np.arange(len(theta_deg)), cmap='viridis',
               s=8, alpha=0.7, edgecolors='white', linewidths=0.2, zorder=1)
    ax.set_xlabel(r'Kąt $\theta$ $[^\circ]$')
    ax.set_ylabel(r'Odległość $d$')
    ax.set_xlim(0, 360)
    ax.set_xticks(np.arange(0, 361, 45))
    ax.grid(True, alpha=0.3)
    ax.set_title(title)


def plot_angle_dist(ax, theta_deg, title=""):
    unique, counts = np.unique(np.round(theta_deg, 1), return_counts=True)
    ax.scatter(unique, counts / len(theta_deg), c=COLORS[1], s=20, alpha=0.8,
               edgecolors='white', linewidths=0.3)
    ax.set_xlabel(r'Kąt $\theta$ $[^\circ]$')
    ax.set_ylabel(r'Prawdopodobieństwo')
    ax.set_xlim(0, 360)
    ax.set_xticks(np.arange(0, 361, 45))
    ax.grid(True, alpha=0.3)
    ax.set_title(title)


def plot_distance_dist(ax, d, title=""):
    unique, counts = np.unique(np.round(d, 3), return_counts=True)
    mean_d, std_d = np.mean(d), np.std(d)
    ax.scatter(unique, counts / len(d), c=COLORS[2], s=20, alpha=0.8,
               edgecolors='white', linewidths=0.3,
               label=rf'$\langle d \rangle = {mean_d:.3f} \pm {std_d:.3f}$')
    ax.set_xlabel(r'Odległość $d$')
    ax.set_ylabel(r'Prawdopodobieństwo')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best')
    ax.set_title(title)


# ==================== Wykresy pojedyncze i 2x2 ====================

mode_dir_name = "PQ_DTheta_center"

def make_pq_plot(case, col):
    filepath = os.path.join(DATA_DIR, case, mode_dir_name, f"pq_col_{col}.dat")
    c, kc, k = load_meta(filepath)
    p, q = load_pq(filepath)
    fig, ax = plt.subplots(figsize=(9, 9))
    plot_pq(ax, p, q, title=_meta_title(case, col, c, kc, k))
    fig.tight_layout()
    _save(fig, case, f"pq_col{col}.png")


def make_dist_vs_angle_plot(case, col):
    filepath = os.path.join(DATA_DIR, case, mode_dir_name, f"angle_distance_col_{col}.dat")
    c, kc, k = load_meta(filepath)
    theta_deg, d = load_angle_distance(filepath)
    fig, ax = plt.subplots(figsize=(9, 6))
    plot_dist_vs_angle(ax, theta_deg, d, title=_meta_title(case, col, c, kc, k))
    fig.tight_layout()
    _save(fig, case, f"dist_vs_angle_col{col}.png")


def make_angle_dist_plot(case, col):
    filepath = os.path.join(DATA_DIR, case, mode_dir_name, f"angle_distance_col_{col}.dat")
    c, kc, k = load_meta(filepath)
    theta_deg, _ = load_angle_distance(filepath)
    fig, ax = plt.subplots(figsize=(9, 6))
    plot_angle_dist(ax, theta_deg, title=_meta_title(case, col, c, kc, k))
    fig.tight_layout()
    _save(fig, case, f"angle_dist_col{col}.png")


def make_distance_dist_plot(case, col):
    filepath = os.path.join(DATA_DIR, case, mode_dir_name, f"angle_distance_col_{col}.dat")
    c, kc, k = load_meta(filepath)
    _, d = load_angle_distance(filepath)
    fig, ax = plt.subplots(figsize=(9, 6))
    plot_distance_dist(ax, d, title=_meta_title(case, col, c, kc, k))
    fig.tight_layout()
    _save(fig, case, f"distance_dist_col{col}.png")


def make_2x2_plot(case, col):
    pq_path = os.path.join(DATA_DIR, case, mode_dir_name, f"pq_col_{col}.dat")
    ad_path = os.path.join(DATA_DIR, case, mode_dir_name, f"angle_distance_col_{col}.dat")
    c, kc, k = load_meta(pq_path)
    p, q = load_pq(pq_path)
    theta_deg, d = load_angle_distance(ad_path)

    fig, axes = plt.subplots(2, 2, figsize=(16, 14))
    title = _meta_title(case, col, c, kc, k)
    plot_pq(axes[0, 0], p, q, title=r'\textbf{Trajektoria} $P$-$Q$')
    plot_dist_vs_angle(axes[0, 1], theta_deg, d, title=r'\textbf{Odległość vs Kąt}')
    plot_angle_dist(axes[1, 0], theta_deg, title=r'\textbf{Rozkład kątów}')
    plot_distance_dist(axes[1, 1], d, title=r'\textbf{Rozkład odległości}')
    fig.suptitle(title)
    fig.tight_layout()
    _save(fig, case, f"analysis_2x2_col{col}.png")


# ==================== Main ====================

def main():
    for case in CASES:
        for col in range(1, N_COLS + 1):
            make_2x2_plot(case, col)
            # make_pq_plot(case, col)
            # make_dist_vs_angle_plot(case, col)
            # make_angle_dist_plot(case, col)
            # make_distance_dist_plot(case, col)


if __name__ == '__main__':
    main()
