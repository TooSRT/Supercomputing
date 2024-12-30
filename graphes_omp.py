import subprocess
import time
import os

# Liste des valeurs de N à tester
n_value = 200000  # Fixer N à une valeur constante
max_threads = 32  # Nombre maximum de threads à tester

# Nom du fichier exécutable
executable = "./mandel_OMP_static.out"

# Fichier de sortie temporaire
output_file = "/tmp/test_mandel_omp.ppm"

# Résultats
results = []

print(f"Démarrage des tests OpenMP pour N = {n_value} avec différents nombres de threads...\n")

for num_threads in range(1, max_threads + 1):
    print(f"Exécution avec {num_threads} thread(s)...")

    # Définir le nombre de threads via la variable d'environnement
    os.environ["OMP_NUM_THREADS"] = str(num_threads)

    start_time = time.time()  # Début du chronométrage

    # Commande pour exécuter le fichier avec les arguments nécessaires
    try:
        result = subprocess.run(
            [executable, "-n", str(n_value), "-f", output_file],
            check=True,
            capture_output=True,
            text=True
        )
        output = result.stdout  # Sortie standard (facultatif)
    except subprocess.CalledProcessError as e:
        print(f"Erreur lors de l'exécution avec {num_threads} thread(s): {e}")
        results.append((num_threads, None))  # Ajouter un résultat d'erreur
        continue

    end_time = time.time()  # Fin du chronométrage
    elapsed_time = end_time - start_time

    print(f"Temps d'exécution avec {num_threads} thread(s): {elapsed_time:.2f} secondes")
    results.append((num_threads, elapsed_time))

print("\nTests terminés.\n")

# Résumé des résultats
print("Résumé des résultats :")
for threads, elapsed_time in results:
    if elapsed_time is not None:
        print(f"{threads} thread(s): {elapsed_time:.2f} secondes")
    else:
        print(f"{threads} thread(s): Erreur")

# Enregistrer les résultats dans un fichier
with open("results_openmp_static.txt", "w") as f:
    for threads, elapsed_time in results:
        if elapsed_time is not None:
            f.write(f"{threads} thread(s): {elapsed_time:.2f} secondes\n")
        else:
            f.write(f"{threads} thread(s): Erreur\n")

print("\nLes résultats ont été sauvegardés dans 'results_openmp_threads.txt'.")
