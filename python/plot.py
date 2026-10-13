import numpy as np
import matplotlib.pyplot as plt

from config import COLORS, get_spaced_colors


def save_me(filename, fig=None):
    pathname = "plots/" + filename
    if fig is None:
        fig = plt.gcf()
    fig.tight_layout()
    fig.savefig(pathname)
    plt.close(fig)
    print(f"Zapisano: {pathname}")


def plot():
    pass


def main():
    print("\n  Wykonywanie wykresow w toku...")
    plot()


if __name__ == '__main__':
    main()
