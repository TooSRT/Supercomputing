import subprocess
import time
import os

# Function to run CUDA Mandelbrot and measure the time taken
def run_mandelbrot(nb_iter, width, height, output_file="mandel_cuda.ppm"):
    # Command to run the CUDA Mandelbrot program
    command = [
        "./mandel_cuda.out", "-n", str(nb_iter), "-d", f"{width} {height}", "-f", output_file
    ]

    start_time = time.time()  # Start timer
    subprocess.run(command)  # Execute the program
    end_time = time.time()  # End timer
    
    return end_time - start_time  # Return elapsed time in seconds

# Configuration values for testing
nb_iter_values = [100,1000, 10000, 25000, 50000,75000,100000,150000,200000]  # Different iteration values (precision)
image_sizes = [(512, 512), (1024, 768), (2048, 1536)]  # Different image sizes

# Create a directory for results if it doesn't exist
if not os.path.exists("results"):
    os.makedirs("results")

# Run tests for each combination of precision and image size
for nb_iter in nb_iter_values:
    for width, height in image_sizes:
        # Define output file based on configuration
        output_file = f"results/mandel_{nb_iter}_{width}x{height}.ppm"
        
        # Run the Mandelbrot program and measure performance
        elapsed_time = run_mandelbrot(nb_iter, width, height, output_file)
        
        # Print and log the results
        print(f"Test with {nb_iter} iterations, {width}x{height} image size took {elapsed_time:.6f} seconds.")
        
        # Log results to a file
        with open("results/performance_log.txt", "a") as log_file:
            log_file.write(f"Iterations: {nb_iter}, Size: {width}x{height}, Time: {elapsed_time:.6f} seconds\n")
