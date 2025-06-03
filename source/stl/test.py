#!/usr/bin/env python3
import subprocess

# Step 1: Compile the C++ program
compile_command = ["g++", "-o", "test", "ink_memory.h", "memtest.cc"]
print("Compiling...")
compilation = subprocess.run(compile_command, capture_output=True, text=True)

# Check if compilation succeeded
if compilation.returncode != 0:
    print("Compilation failed:")
    print(compilation.stderr)
    exit(1)
else:
    print("Compilation successful.")

# Step 2: Run the compiled binary 100 times and append output to results.txt
with open("results.txt", "a") as results_file:
    for i in range(100):
        run_command = ["./test"]
        print(f"Running iteration {i+1}...")
        result = subprocess.run(run_command, capture_output=True, text=True)
        results_file.write(result.stdout)
        results_file.write("\n")  # Optional: separate runs with a newline

print("All runs complete. Results saved to results.txt.")
