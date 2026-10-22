# Exercise 4 - draw "Time vs Number of Processors" and "Speedup" graphs from ex4/results.csv
# Install once:  sudo apt install -y python3-matplotlib
# Run:           python3 ex4/plot_graphs.py     -> ex4/time_vs_processors.png, ex4/speedup.png
import csv
import matplotlib
matplotlib.use("Agg")                      # no screen needed (works over SSH)
import matplotlib.pyplot as plt

data = {}                                  # {"sum": {1: t1, 2: t2, ...}, "pi": {...}}
with open("ex4/results.csv") as f:
    for row in csv.DictReader(f):
        data.setdefault(row["program"], {})[int(row["processes"])] = float(row["time"])

titles = {"sum": "Exercise 2: Sum 1..10,000,000", "pi": "Exercise 3: Monte Carlo Pi (10,000,000)"}

# Graph 1: Time vs number of processors
fig, axes = plt.subplots(1, 2, figsize=(11, 4))
for ax, prog in zip(axes, ["sum", "pi"]):
    p = sorted(data[prog]); t = [data[prog][n] for n in p]
    ax.plot(p, t, marker="o")
    ax.set_title(titles[prog]); ax.set_xlabel("Number of processors"); ax.set_ylabel("Time (s)")
    ax.set_xticks(p); ax.grid(alpha=0.3)
fig.tight_layout(); fig.savefig("ex4/time_vs_processors.png", dpi=150)

# Graph 2: Speedup = T(1) / T(p), compared with the ideal speedup = p
fig, axes = plt.subplots(1, 2, figsize=(11, 4))
for ax, prog in zip(axes, ["sum", "pi"]):
    p = sorted(data[prog]); t1 = data[prog][1]
    s = [t1 / data[prog][n] for n in p]
    ax.plot(p, s, marker="o", label="Measured speedup")
    ax.plot(p, p, linestyle="--", label="Ideal (speedup = p)")
    ax.set_title(titles[prog]); ax.set_xlabel("Number of processors"); ax.set_ylabel("Speedup")
    ax.set_xticks(p); ax.grid(alpha=0.3); ax.legend()
fig.tight_layout(); fig.savefig("ex4/speedup.png", dpi=150)

for prog in ["sum", "pi"]:
    t1 = data[prog][1]
    print(prog, "  ".join(f"p={n}: {data[prog][n]:.4f}s speedup={t1/data[prog][n]:.2f}" for n in sorted(data[prog])))
print("Saved ex4/time_vs_processors.png and ex4/speedup.png")
