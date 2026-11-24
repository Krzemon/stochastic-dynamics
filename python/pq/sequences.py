from collections import Counter

import numpy as np

from config.style import COLORS, plt

CASES = ["4750", "6000", "7500", "8390"]
N_COLS = 5
DATA_DIR = "data"
UNIT = "rad"
mode_dir_name = "PQ_DTheta_center"
N_TAIL = 10000

MIN_LEN = 3
MAX_LEN = 16
MIN_COUNT = 2
TOP_PER_LEN = 3
SKIP_CONSTANT = True


def load_theta(filepath, unit=UNIT, col_idx=0, n_tail=N_TAIL):
    """Wczytuje ostatnie n_tail kątów z pliku i zwraca kąty w stopniach w [0, 360)."""
    data = np.loadtxt(filepath, comments="#")
    raw = data[:, col_idx] if data.ndim > 1 else data
    if n_tail is not None and len(raw) > n_tail:
        raw = raw[-n_tail:]

    if unit == "rad":
        return np.degrees(raw) % 360.0
    if unit == "deg":
        return raw % 360.0
    raise ValueError("UNIT musi być równe 'rad' lub 'deg'")


def identify_runs(theta_deg):
    """Zlicza długości kolejnych rosnących przebiegów kątów w [0, 360)."""
    theta_deg = np.asarray(theta_deg, dtype=float)
    if len(theta_deg) == 0:
        return np.array([], dtype=int)

    runs = []
    current_run_len = 1

    for i in range(1, len(theta_deg)):
        if theta_deg[i] > theta_deg[i - 1]:
            current_run_len += 1
        else:
            runs.append(current_run_len)
            current_run_len = 1

    runs.append(current_run_len)
    return np.array(runs, dtype=int)


def run_offsets(runs):
    """Zwraca indeks początkowy w tablicy kątów dla każdego runu."""
    return np.concatenate(([0], np.cumsum(runs))).astype(int)


def find_occurrences(runs, seq):
    """Zwraca indeksy początkowe nienakładających się wystąpień sekwencji seq w runs."""
    seq = tuple(int(x) for x in seq)
    L = len(seq)
    starts = []
    i = 0
    while i <= len(runs) - L:
        if tuple(int(x) for x in runs[i:i + L]) == seq:
            starts.append(i)
            i += L
        else:
            i += 1
    return starts


def find_repeated_sequences(runs, min_len=MIN_LEN, max_len=MAX_LEN,
                            min_count=MIN_COUNT, top_per_len=TOP_PER_LEN,
                            skip_constant=SKIP_CONSTANT):
    """Wyszukuje powtarzające się wzorce długości runów w gotowej tablicy runs."""
    results = []
    n_runs = len(runs)
    if n_runs < min_len:
        return results

    max_l_actual = min(max_len, n_runs)
    for L in range(min_len, max_l_actual + 1):
        counter = Counter(
            tuple(int(x) for x in runs[i:i + L]) for i in range(n_runs - L + 1)
        )
        added = 0
        for seq, cnt in counter.most_common():
            if cnt < min_count or added >= top_per_len:
                break
            if skip_constant and len(set(seq)) == 1:
                continue
            starts = find_occurrences(runs, seq)
            if len(starts) >= min_count:
                results.append((seq, starts))
                added += 1

    results.sort(key=lambda r: len(r[0]) * len(r[1]), reverse=True)
    return results


def plot_found_block(theta_deg, rank=0, occurrence_idx=0, context_angles=10,
                     min_len=MIN_LEN, max_len=MAX_LEN, min_count=MIN_COUNT,
                     top_per_len=TOP_PER_LEN, skip_constant=SKIP_CONSTANT,
                     title=None):
    """Buduje tablicę runów, szuka powtarzających się wzorców i rysuje jeden blok."""
    runs = identify_runs(theta_deg)
    offsets = run_offsets(runs)
    patterns = find_repeated_sequences(
        runs,
        min_len=min_len,
        max_len=max_len,
        min_count=min_count,
        top_per_len=top_per_len,
        skip_constant=skip_constant,
    )

    if not patterns:
        raise ValueError("Nie znaleziono powtarzających się wzorców dla podanych progów.")

    seq, starts = patterns[min(max(0, rank), len(patterns) - 1)]
    run_start = starts[min(max(0, occurrence_idx), len(starts) - 1)]
    run_end = run_start + len(seq)

    angle_start = offsets[run_start]
    angle_end = offsets[run_end]

    plot_start = max(0, angle_start - context_angles)
    plot_end = min(len(theta_deg), angle_end + context_angles)

    indices = np.arange(plot_start, plot_end)
    angles = theta_deg[plot_start:plot_end]
    colors = [COLORS[0] if angle_start <= i < angle_end else "gray" for i in indices]

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(indices, angles, "-", color="white", linewidth=0.8, alpha=0.3, zorder=1)
    ax.scatter(indices, angles, c=colors, s=50, alpha=0.9,
               edgecolors="white", linewidths=0.8, zorder=2)

    ax.axvline(angle_start, color="yellow", linewidth=1.5, linestyle="--", alpha=0.7)
    ax.axvline(angle_end, color="red", linewidth=1.5, linestyle="--", alpha=0.7)

    ax.set_xlim(plot_start - 1, plot_end + 1)
    ax.set_ylim(-10, 370)
    ax.set_xlabel(r"Indeks kąta")
    ax.set_ylabel(r"Kąt $\theta$ $[^\circ]$")
    ax.set_yticks(np.arange(0, 361, 45))
    ax.grid(True, alpha=0.3)
    ax.set_title(
        title if title is not None
        else f"Wzorzec #{rank + 1} ({len(starts)}x): {list(seq)}"
    )

    fig.tight_layout()
    return fig, seq, starts, runs