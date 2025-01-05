import matplotlib.pyplot as plt

# Data for OpenMP (number of cores and elapsed time)
cores = list(range(1, 33))  # Total cores from 1 to 32
elapsed_time = [
    315.65, 160.42, 108.31, 86.92, 66.50, 55.32, 48.12, 43.68, 43.09, 43.14,
    43.19, 43.08, 43.26, 43.06, 43.18, 43.12, 43.09, 43.12, 43.07, 43.16,
    43.17, 43.10, 43.09, 43.07, 43.10, 43.10, 43.08, 43.07, 43.11, 43.15,
    43.10, 43.14
]

# Sequential execution time (1 thread)
sequential_time = 315.65

# Compute the speed-up
speed_up = [sequential_time / t for t in elapsed_time]

# Ideal speed-up (linear scaling)
ideal_speed_up = cores

# Plot the graph
plt.figure(figsize=(12, 7))
plt.plot(cores, speed_up, marker='o', linestyle='-', color='b', label='Measured Speed-up')
plt.plot(cores, ideal_speed_up, marker='o', linestyle='--', color='r', label='Ideal Speed-up')

# Add labels, title, and grid
plt.xlabel('Number of Cores', fontsize=12)
plt.ylabel('Speed-up', fontsize=12)
plt.title('Speed-up vs Number of Cores (OpenMP Dynamic, Chunk Size 50)', fontsize=14)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=12)
plt.tight_layout()

# Display the plot
plt.show()
