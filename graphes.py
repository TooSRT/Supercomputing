import subprocess

# Configuration
executable = "./mandel.out"  # Chemin vers l'exécutable mandel.out
image_width = 1024
image_height = 768
output_file = "/tmp/mandel.ppm"
iterations_list = [100, 1000, 10000, 50000, 100000, 1000000]  # Différentes valeurs d'itérations

# Exécuter le programme avec différentes valeurs d'itérations
for iterations in iterations_list:
    print(f"Testing with {iterations} iterations:")
    try:
        # Commande pour exécuter le fichier mandel avec les paramètres appropriés
        command = [
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
                print(f"  Time for {iterations} iterations: {elapsed_time:.9f} seconds")
                break
    except subprocess.CalledProcessError as e:
        print(f"  Error running with {iterations} iterations: {e}")
        continue
