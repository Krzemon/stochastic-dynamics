import os
import re
from collections import Counter

import numpy as np

from config import COLORS, plt


# ==================== Parametry wyszukiwania ====================

MIN_LEN = 3          # minimalna długość sekwencji (liczba runów)
MAX_LEN = 16         # maksymalna długość sekwencji (liczba runów)
MIN_COUNT = 5        # minimalna liczba (nienakładających się) wystąpień
TOP_PER_LEN = 3      # ile najczęstszych sekwencji zapisać dla każdej długości
SKIP_CONSTANT = True # pomijaj sekwencje złożone z jednej wartości, np. [6, 6, 6, 6]
N_PLOTS = 3          # ile najlepszych sekwencji narysować


def load_theta(filepath):
    theta = np.loadtxt(filepath, usecols=2)
    theta_deg = np.degrees(theta) % 360
    return theta_deg


def case_and_s_from_path(filepath):
    # data/7889/p_q_theta_d_y0_last_7000_y_266_11_od_0_7.88900.dat -> ("7889", 7.889)
    case = os.path.basename(os.path.dirname(filepath))
    match = re.search(r'od_0_(\d+\.\d+)', os.path.basename(filepath))
    s_value = float(match.group(1)) if match else float(case) / 1000
    return case, s_value


def identify_runs(theta_deg):
    runs = []
    current_run_length = 1

    for i in range(1, len(theta_deg)):
        diff = theta_deg[i] - theta_deg[i-1]

        if 0 < diff < 180:
            current_run_length += 1
        else:
            if current_run_length > 0:
                runs.append(current_run_length)
            current_run_length = 1

    if current_run_length > 0:
        runs.append(current_run_length)

    return runs


def run_offsets(runs):
    # offsets[i] = indeks kąta, od którego zaczyna się run o numerze i
    return np.concatenate(([0], np.cumsum(runs))).astype(int)


# ==================== Szukanie sekwencji ====================

def count_occurrences(runs, seq):
    L = len(seq)
    starts = []
    i = 0
    while i <= len(runs) - L:
        if tuple(runs[i:i+L]) == seq:
            starts.append(i)
            i += L
        else:
            i += 1
    return starts


def find_repeated_sequences(runs, min_len, max_len, min_count, top_per_len):
    results = []

    for L in range(min_len, max_len + 1):
        counter = Counter(tuple(runs[i:i+L]) for i in range(len(runs) - L + 1))
        added = 0

        for seq, cnt in counter.most_common():
            if cnt < min_count or added >= top_per_len:
                break
            if SKIP_CONSTANT and len(set(seq)) == 1:
                continue

            starts = count_occurrences(runs, seq)
            if len(starts) < min_count:
                continue

            results.append((seq, starts))
            added += 1

    return results


# ==================== Wykres ====================

def plot_single_block(theta_deg, offsets, name, run_start, sequence, context_angles=10):
    angle_start = offsets[run_start]
    angle_end = offsets[run_start + len(sequence)]

    plot_start = max(0, angle_start - context_angles)
    plot_end = min(len(theta_deg), angle_end + context_angles)

    indices = np.arange(plot_start, plot_end)
    angles = theta_deg[plot_start:plot_end]

    colors = []
    for i in indices:
        if angle_start <= i < angle_end:
            colors.append(COLORS[0])
        else:
            colors.append('gray')

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.plot(indices, angles, '-', color='white', linewidth=0.8, alpha=0.3, zorder=1)
    ax.scatter(indices, angles, c=colors, s=50, alpha=0.9, edgecolors='white', linewidths=0.8, zorder=2)

    ax.axvline(angle_start, color='yellow', linewidth=1.5, linestyle='--', alpha=0.7)
    ax.axvline(angle_end, color='red', linewidth=1.5, linestyle='--', alpha=0.7)

    ax.set_xlim(plot_start - 1, plot_end + 1)
    ax.set_ylim(-10, 370)
    ax.set_xlabel(r'Indeks kąta')
    ax.set_ylabel(r'Kąt $\theta$ $[^\circ]$')
    ax.grid(True, alpha=0.3)
    ax.set_yticks(np.arange(0, 361, 45))
    ax.axhline(0, color='white', linewidth=0.8, linestyle='--', alpha=0.3)
    ax.axhline(360, color='white', linewidth=0.8, linestyle='--', alpha=0.3)

    ax.set_title(f'{name}:  {list(sequence)}')

    fig.tight_layout()
    return fig


# ==================== Zapis ====================

def write_found_sequences(results, offsets, case, s_value, plotted):
    os.makedirs(f"data/{case}", exist_ok=True)
    filepath = f"data/{case}/found_sequences_s_{s_value:.5f}.txt"

    with open(filepath, 'w') as f:
        f.write(f"Analiza sekwencji kątów dla s = {s_value:.3f}\n")
        f.write(f"Długość {MIN_LEN}-{MAX_LEN} runów, min. {MIN_COUNT} wystąpień\n")
        f.write("=" * 50 + "\n\n")

        for seq, starts in results:
            f.write("=" * 50 + "\n")
            f.write(f"{list(seq)}: {len(starts)} wystąpień\n")
            f.write("=" * 50 + "\n")
            for run_start in starts:
                run_end = run_start + len(seq)
                f.write(f"  run {run_start:5d}-{run_end:5d}  |  "
                        f"indeks kąta {offsets[run_start]:6d}-{offsets[run_end]:6d}\n")
            f.write("\n")

        f.write("=" * 50 + "\n")
        f.write("Wykres sekwencji wykonany dla:\n")
        for name, (seq, run_start) in plotted.items():
            run_end = run_start + len(seq)
            f.write(f"  {name}: {list(seq)}  run {run_start}-{run_end}  |  "
                    f"indeks kąta {offsets[run_start]}-{offsets[run_end]}\n")

    print(f"Zapisano: {filepath}")


# ==================== Funkcja główna ====================

def search_angle_sequences(filepath):
    case, s_value = case_and_s_from_path(filepath)

    theta_deg = load_theta(filepath)
    runs = identify_runs(theta_deg)
    offsets = run_offsets(runs)

    results = find_repeated_sequences(runs, MIN_LEN, MAX_LEN, MIN_COUNT, TOP_PER_LEN)

    for seq, starts in results:
        print(f"{list(seq)}: {len(starts)} wystąpień")

    # najlepsze = najwięcej pokrytych kątów (długość * liczba wystąpień)
    best = sorted(results, key=lambda r: len(r[0]) * len(r[1]), reverse=True)[:N_PLOTS]

    os.makedirs(f"plots/{case}", exist_ok=True)

    plotted = {}
    for rank, (seq, starts) in enumerate(best, start=1):
        name = f"seq_{rank}"
        run_start = starts[len(starts) // 2]

        fig = plot_single_block(theta_deg, offsets, name, run_start, seq)
        filename = f"plots/{case}/block_{name}_{s_value:.5f}.png"
        fig.savefig(filename, dpi=300, bbox_inches='tight')
        plt.close(fig)
        print(f"Zapisano: {filename}")

        plotted[name] = (seq, run_start)

    write_found_sequences(results, offsets, case, s_value, plotted)


def main():
    search_angle_sequences("data/7889/p_q_theta_d_y0_last_7000_y_266_11_od_0_7.88900.dat")


if __name__ == '__main__':
    main()