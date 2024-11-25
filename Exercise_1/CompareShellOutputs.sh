#!/bin/bash

# Get the input commands from the user and save them to a file
echo "Please enter the test commands (end input with Ctrl+D):"
cat > shell_test_commands.txt

# Run the two different shell programs with the same test input
echo "output_myshell"
./myshell /tmp/custom_folder < shell_test_commands.txt > output_myshell.txt 2>&1
echo "output_reference"
./part3.out /tmp/custom_folder < shell_test_commands.txt > output_reference.txt 2>&1

# Compare the outputs
diff -u output_myshell.txt output_reference.txt > shell_output_diff.txt

# Check if there were differences
if [ -s shell_output_diff.txt ]; then
    echo "Differences found between the outputs of myshell and reference_shell:"
    cat shell_output_diff.txt
else
    echo "No differences found between myshell and reference_shell outputs."
fi
