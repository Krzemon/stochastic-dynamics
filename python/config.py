import numpy as np
import matplotlib.pyplot as plt
# import matplotlib.gridspec as gridspec

COLORS = ['#3b82f6', '#10b981', '#f59e0b', '#ef4444', '#8b5cf6', '#06b6d4', '#84cc16']

def get_spaced_colors(n, cmap_name='viridis'):
    cmap = plt.get_cmap(cmap_name)
    return [plt.cm.colors.to_hex(cmap(i)) for i in np.linspace(0, 1, n)]

# default / dark_background / seaborn-v0_8 / ggplot / bmh 
plt.style.use('dark_background') # >print(plt.style.available)

plt.rcParams["text.usetex"] = True
plt.rcParams["text.latex.preamble"] = r"""
    \usepackage[T1]{fontenc}
    \usepackage[utf8]{inputenc}
    \usepackage[polish]{babel}
    \usepackage{lmodern}
    \usepackage{amsmath}
    \usepackage{amssymb}
    \usepackage{physics}
"""

plt.rcParams.update({
    # ===== Kolory tła =====
    'figure.facecolor': '#0a0e17',
    'axes.facecolor': '#111827',

    # ===== Font =====
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],

    # ===== Sizes =====
    "font.size": 17,
    "axes.labelsize": 18,
    "figure.titlesize": 24,
    "axes.titlesize": 18,
    "xtick.labelsize": 14,
    "ytick.labelsize": 14,
    "legend.fontsize": 12,

    # ===== Lines =====
    "axes.linewidth": 1.2,
    "lines.linewidth": 2.5,
    # "grid.alpha": 0.3,
    # "grid.linestyle": "--",

    # ===== Ticks =====
    "xtick.major.width": 0.8,
    "ytick.major.width": 0.8,
    "xtick.minor.width": 0.8,
    "ytick.minor.width": 0.8,

    # ===== Figure =====
    "figure.figsize": (12, 7),
    "figure.dpi": 250,

    # ===== Save =====
    "savefig.dpi": 250,
    "savefig.bbox": "tight",

    # ===== Legend =====
    "legend.frameon": False,
    "legend.fancybox": False,
})


def test_style():
    x = np.linspace(0, 2 * np.pi, 200)
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 6))

    for i in range(4):
        ax1.plot(x, np.sin(x + i*0.5), color=COLORS[i], label=f'Seria {i+1}')

    ax1.set_title(r'\textbf{Test konfiguracji:} Paleta z C')
    ax1.set_ylabel(r'$y(x)$')
    ax1.legend(loc='upper right')
    ax1.grid(True)

    colors = get_spaced_colors(6, 'plasma')
    for i, color in enumerate(colors):
        ax2.plot(x, np.cos(x + i*0.3), color=color, label=f'Krok {i+1}')

    ax2.set_title(r'\textbf{Test konfiguracji:} Generowane kolory (\texttt{plasma})')
    ax2.set_xlabel(r'Kąt $x$ [rad]')
    ax2.set_ylabel(r'$y(x)$')
    ax2.legend(loc='upper right')
    ax2.grid(True)

    plt.tight_layout()
    plt.savefig('wykres.png', dpi=300)
    plt.close()
    

def test_binary_import():

    # out.meta - metadane pliku
    with open("out.meta") as f:
        rows = int(f.readline().split("=")[1])
        cols = int(f.readline().split("=")[1])
        names = f.readline().split()
        
    # out.bin  - dane zapisane binarnie
    data = np.fromfile("out.bin", dtype=np.float64)
    data = data.reshape(rows, cols)


def main():
    pass

if __name__ == '__main__':
    main()