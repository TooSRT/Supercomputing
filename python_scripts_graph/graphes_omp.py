import matplotlib.pyplot as plt

# Données pour OpenMP (nombre de threads et temps écoulé)
threads = list(range(1, 33))  # Total threads de 1 à 32
elapsed_time = [
    315.65, 160.42, 108.31, 86.92, 66.50, 55.32, 48.12, 43.68, 43.09, 43.14,
    43.19, 43.08, 43.26, 43.06, 43.18, 43.12, 43.09, 43.12, 43.07, 43.16,
    43.17, 43.10, 43.09, 43.07, 43.10, 43.10, 43.08, 43.07, 43.11, 43.15,
    43.10, 43.14
]

# Temps d'exécution séquentiel (1 thread)
sequential_time = elapsed_time[0]

# Calcul du speed-up
speed_up = [sequential_time / t for t in elapsed_time]

# Speed-up idéal (scaling linéaire)
ideal_speed_up = threads

# Graphique Speed-up
plt.figure(figsize=(12, 7))
plt.plot(threads, speed_up, marker='o', linestyle='-', color='b', label='Mesured Speed-up')
plt.plot(threads, ideal_speed_up, marker='o', linestyle='--', color='r', label='Ideal Speed-up')

# Ajout des labels, titre, et grille
plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Speed-up', fontsize=12)
plt.title('Speed-up vs Number of Threads (OpenMP Static, Chunk Size 50)', fontsize=14)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=12)
plt.tight_layout()
plt.show()
