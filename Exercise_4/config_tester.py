import os
import subprocess
import random
import re

# Path to the executable and temporary config files
EXECUTABLE = "./ex4.out"
CONFIG_FILE = "test_config.txt"
OUTPUT_FILE = "test_output.txt"

def generate_config(num_producers, max_products, max_queue_size, co_editor_queue_size):
    """
    Generates a random configuration and writes it to a file.
    """
    expected_messages = {}
    with open(CONFIG_FILE, "w") as f:
        for producer_id in range(1, num_producers + 1):
            num_products = random.randint(1, max_products)
            queue_size = random.randint(1, max_queue_size)
            expected_messages[producer_id] = num_products
            f.write(f"PRODUCER {producer_id}\n{num_products}\nqueue size = {queue_size}\n")
        f.write(f"Co-Editor queue size = {co_editor_queue_size}\n")
    print(f"Generated config file: {CONFIG_FILE}")
    return expected_messages

def run_program():
    """
    Runs the program with the generated configuration file.
    """
    with open(OUTPUT_FILE, "w") as output:
        try:
            result = subprocess.run([EXECUTABLE, CONFIG_FILE], stdout=output, stderr=subprocess.PIPE, text=True)
            if result.returncode != 0:
                print("Error running the program:", result.stderr)
                return False
        except FileNotFoundError:
            print(f"Executable {EXECUTABLE} not found!")
            return False
    return True

def validate_output(expected_messages, num_producers):
    """
    Validates the output to ensure all producers finished and messages are logical.
    """
    with open(OUTPUT_FILE, "r") as f:
        output_lines = f.readlines()

    producer_message_count = {i: 0 for i in range(1, num_producers + 1)}
    producers_done = set()
    all_messages = []
    errors = []

    for line in output_lines:
        line = line.strip()
        all_messages.append(line)

        # Check for producer messages
        match = re.match(r"producer (\d+) (\w+) (\d+)", line, re.IGNORECASE)
        if match:
            producer_id = int(match.group(1))
            if producer_id in producer_message_count:
                producer_message_count[producer_id] += 1
            else:
                errors.append(f"Unexpected producer ID in output: {line}")

        # Check for DONE messages
        if line == "DONE":
            producers_done.add(line)

    # Validate all producers completed their expected messages
    for producer_id, count in producer_message_count.items():
        if count != expected_messages[producer_id]:
            errors.append(
                f"Producer {producer_id} produced {count} messages, "
                f"expected {expected_messages[producer_id]}."
            )

    # Ensure "DONE" is written correctly
    if len(producers_done) == 0:
        errors.append("No DONE message found in the output.")
    elif len(producers_done) > 1:
        errors.append("Multiple DONE messages found, which is unexpected.")

    if errors:
        print("Validation failed due to the following issues:")
        for error in errors:
            print(f"- {error}")
        return False

    print("All producers completed their messages as expected.")
    return True

def main():
    # Configuration parameters
    num_producers = 20
    max_products = 2
    max_queue_size = 2
    co_editor_queue_size = 2

    # Generate random config
    expected_messages = generate_config(num_producers, max_products, max_queue_size, co_editor_queue_size)

    # Run the program
    if not run_program():
        print("Failed to run the program.")
        return

    # Validate the output
    if validate_output(expected_messages, num_producers):
        print("Output is valid!")
    else:
        print("Output validation failed. Check details above.")

if __name__ == "__main__":
    main()
