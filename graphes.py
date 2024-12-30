import subprocess

# Path to your compiled MPI program
program_path = './mandel_MPI3.out'

# Range of processes to test (from 1 to 32)
process_range = range(1, 33)

# File to store results
output_file = 'execution_times_mpi3.txt'

# Open the file to store execution times
with open(output_file, 'w') as result_file:
    result_file.write("Number of Processes, Elapsed Time (seconds)\n")

    # Run the program for each number of processes
    for num_procs in process_range:
        print(f"Running with {num_procs} process(es)...")

        # Use mpiexec or mpirun to execute the MPI program
        result = subprocess.run(
            ['mpiexec', '-n', str(num_procs), program_path],
            capture_output=True,
            text=True
        )

        # Print the standard output and error
        print(result.stdout)
        if result.stderr:
            print(result.stderr)

        # Extract the elapsed time from the output
        elapsed_time = None
        for line in result.stdout.splitlines():
            if "Elapsed time" in line:
                elapsed_time = line.split(":")[1].strip()
                break

        # Write the number of processes and elapsed time to the file
        if elapsed_time:
            result_file.write(f"{num_procs}, {elapsed_time}\n")
        else:
            result_file.write(f"{num_procs}, Error in output\n")

print(f"Execution times written to '{output_file}'.")
