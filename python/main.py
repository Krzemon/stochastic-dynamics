import numpy as np
import matplotlib.pyplot as plt

# from config.style import COLORS, plt, save_me
from pq.phase_space_plots import make_2x2_plot

# import os
# from pq.sequences import (
#     DATA_DIR,
#     N_TAIL,
#     UNIT,
#     mode_dir_name,
#     load_theta,
#     plot_found_block,
# )


CASES = ["4750", "6000", "7500", "8390"]
# CASES = ["7500"]
N_COLS = 5
DATA_DIR = "data"

def main():
    for case in CASES:
        for col in range(1, N_COLS + 1):
            make_2x2_plot(case, col)





    # case = "7500"
    # col = 1
    # filepath = os.path.join(DATA_DIR, case, mode_dir_name, f"angle_distance_col_{col}.dat")

    # theta_deg = load_theta(filepath, unit=UNIT, col_idx=0, n_tail=N_TAIL)
    # fig, seq, starts, runs = plot_found_block(theta_deg, rank=0, occurrence_idx=0)
    # print(f"Liczba przebiegów (runów): {len(runs)}")
    # print(f"Najlepszy wzorzec: {list(seq)}, wystąpienia: {len(starts)}")

    # os.makedirs(f"plots/{case}", exist_ok=True)
    # out_path = f"plots/{case}/pattern_block_col{col}.png"
    # fig.savefig(out_path, dpi=300, bbox_inches="tight")
    # plt.close(fig)

if __name__ == "__main__":
    main()
