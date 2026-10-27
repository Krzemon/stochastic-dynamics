import os
import numpy as np
import matplotlib.pyplot as plt

from config import COLORS, save_me


# przypadek: folder data_out/<case>/output/ oraz nazwa pliku wejściowego bez rozszerzenia
CASES = [
    ("7889", "y_266_11_od_0_7.88900"),
]


def data_path(case, name, prefix):
    return f"data_out/{case}/output/{prefix}_{name}.dat"


def load(path):
    if not os.path.exists(path):
        print(f"Brak pliku danych: {path}. Uruchom najpierw obliczenia w C++.")
        return None
    return np.loadtxt(path)  # linia nagłówka zaczyna się od '#', więc jest pomijana


def plot_K_file(path, xlabel, title, out_name, logx=False):
    """Rysuje jeden plik: kolumna 0 = oś X, kolumny 1..5 = szeregi y0..y4."""
    data = load(path)
    if data is None:
        return

    x = data[:, 0]

    plt.figure(figsize=(9, 5))
    for k in range(1, data.shape[1]):
        plt.plot(x, data[:, k], marker='o', markersize=3, linestyle='-',
                 color=COLORS[(k - 1) % len(COLORS)], label=f"$y_{k - 1};$ mediana $= {np.median(data[:, k]):+.3f}$")

    plt.title(title, fontsize=12)
    plt.xlabel(xlabel, fontsize=11)
    plt.ylabel("Współczynnik $K$", fontsize=11)
    plt.grid(True, linestyle=':', alpha=0.6)
    if logx:
        plt.xscale('log')
    plt.legend(frameon=True, title="szereg", loc="best")

    save_me(out_name)


def plot_K_vs_c_corr(case, name):
    plot_K_file(
        data_path(case, name, "K_vs_c_corr"),
        "Parametr $c$",
        f"$K(c)$, metoda korelacji (s = {case})",
        f"K_vs_c_corr_{case}.png",
    )


def plot_K_vs_c_reg(case, name):
    plot_K_file(
        data_path(case, name, "K_vs_c_reg"),
        "Parametr $c$",
        f"$K(c)$, metoda regresji (s = {case})",
        f"K_vs_c_reg_{case}.png",
    )


def plot_K_vs_Nc_corr(case, name):
    plot_K_file(
        data_path(case, name, "K_vs_Nc_corr"),
        "Liczba punktów $N_c$",
        f"Mediana $K(N_c)$, metoda korelacji (s = {case})",
        f"K_vs_Nc_corr_{case}.png",
    )


def plot_K_vs_Nc_reg(case, name):
    plot_K_file(
        data_path(case, name, "K_vs_Nc_reg"),
        "Liczba punktów $N_c$",
        f"Mediana $K(N_c)$, metoda regresji (s = {case})",
        f"K_vs_Nc_reg_{case}.png",
    )


def main():
    print("\n  Wykonywanie wykresow w toku...")
    for case, name in CASES:
        plot_K_vs_c_corr(case, name)
        plot_K_vs_c_reg(case, name)
        plot_K_vs_Nc_corr(case, name)
        plot_K_vs_Nc_reg(case, name)


if __name__ == '__main__':
    main()