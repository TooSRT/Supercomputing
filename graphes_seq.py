import subprocess
import time

# Liste des valeurs de N à tester (nombre d'itérations)
n_values = [1000, 10000, 50000, 100000, 150000, 200000]

# Nom du fichier exécutable
executable = "./mandel.out"

# Fichier de sortie temporaire
output_file = "/tmp/test_mandel.ppm"

# Résultats
results = []

print("Démarrage des tests...\n")

for n in n_values:
    print(f"Exécution pour N = {n}...")

    start_time = time.time()  # Début du chronométrage

    # Commande pour exécuter le fichier avec les arguments nécessaires
    try:
        result = subprocess.run(
            [executable, "-n", str(n), "-f", output_file],
            check=True,
            capture_output=True,
            text=True
        )
        output = result.stdout  # Sortie standard (facultatif)
    except subprocess.CalledProcessError as e:
        print(f"Erreur lors de l'exécution pour N = {n}: {e}")
        results.append((n, None))  # Ajouter un résultat d'erreur
        continue

    end_time = time.time()  # Fin du chronométrage
    elapsed_time = end_time - start_time

    print(f"Temps d'exécution pour N = {n}: {elapsed_time:.2f} secondes")
    results.append((n, elapsed_time))

print("\nTests terminés.\n")

# Résumé des résultats
print("Résumé des résultats :")
for n, elapsed_time in results:
    if elapsed_time is not None:
        print(f"N = {n}: {elapsed_time:.2f} secondes")
    else:
        print(f"N = {n}: Erreur")

# Enregistrer les résultats dans un fichier
with open("results.txt", "w") as f:
    for n, elapsed_time in results:
        if elapsed_time is not None:
            f.write(f"N = {n}: {elapsed_time:.2f} secondes\n")
        else:
            f.write(f"N = {n}: Erreur\n")

print("\nLes résultats ont été sauvegardés dans 'results.txt'.")
