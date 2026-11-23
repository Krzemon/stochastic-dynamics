import os
import numpy as np

from config.style import COLORS, plt, save_me

CASES = ["4750", "6000", "7500", "8390"]
N_COLS = 5


def _load(path):
    if not os.path.exists(path):
        print(f"  Brak pliku: {path}")
        return None
    return np.loadtxt(path, comments="#")


def _save(fig, case, name):
    os.makedirs(f"plots/{case}", exist_ok=True)
    path = f"plots/{case}/{name}"
    fig.savefig(path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    print(f"Zapisano: {path}")


def plot_k_vs_c(case, nc=100):
    """K(c) dla danego Nc — wszystkie kolumny na jednym wykresie."""
    base = f"data/{case}/K_vs_Nc"
    fig, ax = plt.subplots()
    any_data = False

    for col in range(1, N_COLS + 1):
        path = os.path.join(base, f"K_vs_c_col_{col}_Nc{nc}.dat")
        data = _load(path)
        if data is None:
            continue
        c_vals, k_vals = data[:, 0], data[:, 1]
        median_k = np.median(k_vals)
        ax.scatter(c_vals, k_vals, s=12, color=COLORS[(col - 1) % len(COLORS)],
                   alpha=0.7, label=rf'$y_{{{col}}}$; med $K={median_k:+.3f}$')
        any_data = True

    if not any_data:
        plt.close(fig)
        return

    ax.axhline(0, color='gray', linestyle='--', alpha=0.5, linewidth=1)
    ax.set_xlabel(r'$c$')
    ax.set_ylabel(r'$K$')
    ax.set_title(rf'$K(c)$,\quad $N_c={nc}$,\quad $s={int(case)/1000:.3f}$')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best')
    fig.tight_layout()
    _save(fig, case, f"K_vs_c_Nc{nc}.png")


def plot_k_vs_nc(case):
    """Mediana K(Nc) — wszystkie kolumny na jednym wykresie."""
    base = f"data/{case}/K_vs_Nc"
    fig, ax = plt.subplots()
    any_data = False

    for col in range(1, N_COLS + 1):
        path = os.path.join(base, f"K_vs_Nc_col_{col}.dat")
        data = _load(path)
        if data is None:
            continue
        nc_vals, k_vals = data[:, 0], data[:, 1]
        ax.plot(nc_vals, k_vals, marker='s', markersize=7, linewidth=2.0,
                alpha=1.0, color=COLORS[(col - 1) % len(COLORS)], label=rf'$y_{{{col}}}$')
        any_data = True

    if not any_data:
        plt.close(fig)
        return

    ax.axhline(0, color='gray', linestyle='--', alpha=0.5, linewidth=1)
    ax.set_xlabel(r'$N_c$')
    ax.set_ylabel(r'mediana $K$')
    ax.set_title(rf'$K(N_c)$,\quad $s={int(case)/1000:.3f}$')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best')
    fig.tight_layout()
    _save(fig, case, "K_vs_Nc.png")


def plot_k_reg(case):
    """K(c) metodą regresji — wszystkie kolumny na jednym wykresie."""
    base = f"data/{case}/K_reg"
    fig, ax = plt.subplots()
    any_data = False

    for col in range(1, N_COLS + 1):
        path = os.path.join(base, f"K_reg_col_{col}.dat")
        data = _load(path)
        if data is None:
            continue
        c_vals, k_vals = data[:, 0], data[:, 1]
        median_k = np.median(k_vals)
        ax.scatter(c_vals, k_vals, s=12, color=COLORS[(col - 1) % len(COLORS)],
                   alpha=0.7, label=rf'$y_{{{col}}}$; med $K={median_k:+.3f}$')
        any_data = True

    if not any_data:
        plt.close(fig)
        return

    ax.axhline(0, color='gray', linestyle='--', alpha=0.5, linewidth=1)
    ax.set_xlabel(r'$c$')
    ax.set_ylabel(r'$K$')
    ax.set_title(rf'$K(c)$ regresja,\quad $s={int(case)/1000:.3f}$')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best')
    fig.tight_layout()
    _save(fig, case, "K_reg.png")


def main():
    # for case in CASES:
    #     base = f"data/{case}/K_vs_Nc"
    #     if not os.path.exists(base):
    #         print(f"Brak katalogu: {base}")
    #         continue

    #     nc_values = sorted({
    #         int(f.split("_Nc")[1].replace(".dat", ""))
    #         for f in os.listdir(base)
    #         if f.startswith("K_vs_c_col_") and "_Nc" in f
    #     })
    #     for nc in nc_values:
    #         plot_k_vs_c(case, nc)
    #     plot_k_vs_nc(case)

    plot_k_reg(4750)


if __name__ == '__main__':
    main()
