import subprocess
import matplotlib.pyplot as plt

# Configuration
executable = "./mandel_MPI2"  # Nom du programme compilé
image_width = 1024
image_height = 768
output_file = "/tmp/mandel.ppm"
iterations = 1000 # Nombre d'itérations de Mandelbrot
processes_to_test = [1, 2, 4, 6]  # Test avec 1, 2, 4 et 6 processeurs

# Stockage des résultats
processors = []
times = []

# Exécuter le programme avec différents nombres de processeurs
for num_procs in processes_to_test:
    try:
        # Commande MPI
        command = [
            "mpirun",
            "-np", str(num_procs),
            executable,
            "-d", str(image_width), str(image_height),
            "-n", str(iterations),
            "-f", output_file
        ]
        print(f"Running: {' '.join(command)}")

        # Exécuter la commande et capturer la sortie
        result = subprocess.run(command, capture_output=True, text=True, check=True)
        output = result.stdout

        # Rechercher la ligne contenant "Elapsed time"
        for line in output.splitlines():
            if "Elapsed time" in line:
                elapsed_time = float(line.split(":")[1].strip())
                processors.append(num_procs)
                times.append(elapsed_time)
                print(f"Processors: {num_procs}, Time: {elapsed_time:.9f} seconds")
                break
    except subprocess.CalledProcessError as e:
        print(f"Error running with {num_procs} processors: {e}")
        continue

# Vérification des résultats
if not processors or not times:
    print("No valid results were captured. Check your executable or input parameters.")
    exit(1)

# Calculer le speed-up
sequential_time = times[0]  # Temps pour 1 processeur
speedup = [sequential_time / t for t in times]

# Calculer l'efficacité
efficiency = [s / p for s, p in zip(speedup, processors)]

# Tracé des graphes
plt.figure(figsize=(15, 8))

# Speed-up
plt.subplot(1, 3, 2)
plt.plot(processors, speedup, marker='o', label='Speed-up', color='green')
plt.plot(processors, processors, linestyle='--', label='Speed-up idéal', color='red')
plt.xlabel('Nombre de processeurs')
plt.ylabel('Speed-up')
plt.title('Speed-up en fonction du nombre de processeurs')
plt.grid(True)
plt.legend()

# Affichage des graphes
plt.tight_layout()
plt.show()
