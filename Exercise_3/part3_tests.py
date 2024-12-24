import subprocess
import json
import os
import shutil
import signal

class TimeoutException(Exception):
    pass

def timeout_handler(signum, frame):
    raise TimeoutException

def compile_c_files():
    subprocess.run(["sudo", "gcc", "main.c", "copytree.c", "-o", "copytree"], check=True)

def setup_test_environment(test_case):
    os.makedirs(test_case['source_directory'], exist_ok=True)
    for item in test_case['items']:
        item_path = os.path.join(test_case['source_directory'], item['name'])
        if item['type'] == 'file':
            with open(item_path, 'w') as f:
                f.write(item['content'])
            os.chmod(item_path, item['permissions'])
        elif item['type'] == 'directory':
            os.makedirs(item_path, exist_ok=True)
            os.chmod(item_path, item['permissions'])
        elif item['type'] == 'symlink':
            os.symlink(item['target'], item_path)

def run_test_case(test_case):
    setup_test_environment(test_case)
    command = ["sudo", "./copytree"]
    if test_case['copy_symlinks']:
        command.append("-l")
    if test_case['copy_permissions']:
        command.append("-p")
    command.extend([test_case['source_directory'], test_case['destination_directory']])
    subprocess.run(command, check=True)

def verify_test_result(test_case):
    failure_details = []
    for item in test_case['items']:
        dest_item_path = os.path.join(test_case['destination_directory'], item['name'])
        if item['type'] == 'file':
            if not os.path.isfile(dest_item_path):
                failure_details.append(f"File {item['name']} was not copied correctly.")
            elif (os.stat(dest_item_path).st_mode & 0o777) != (item['permissions'] & 0o777) and test_case['copy_permissions']:
                failure_details.append(f"Permissions for file {item['name']} do not match.")
        elif item['type'] == 'directory':
            if not os.path.isdir(dest_item_path):
                failure_details.append(f"Directory {item['name']} was not copied correctly.")
            elif (os.stat(dest_item_path).st_mode & 0o777) != (item['permissions'] & 0o777) and test_case['copy_permissions']:
                failure_details.append(f"Permissions for directory {item['name']} do not match.")
        elif item['type'] == 'symlink':
            if test_case['copy_symlinks']:
                if not os.path.islink(dest_item_path):
                    failure_details.append(f"Symlink {item['name']} was not copied as a link.")
                elif os.readlink(dest_item_path) != item['target']:
                    failure_details.append(f"Symlink target for {item['name']} does not match. Expected: {item['target']}, Found: {os.readlink(dest_item_path)}.")
    return failure_details

def run_tests():
    signal.signal(signal.SIGALRM, timeout_handler)
    signal.alarm(30)  # Set timeout to 30 seconds

    try:
        with open('test_cases.json', 'r') as f:
            test_cases = json.load(f)

        compile_c_files()

        results = []
        for i, test_case in enumerate(test_cases, 1):
            if os.path.exists(test_case['source_directory']):
                shutil.rmtree(test_case['source_directory'])
            if os.path.exists(test_case['destination_directory']):
                shutil.rmtree(test_case['destination_directory'])

            try:
                run_test_case(test_case)
                failure_details = verify_test_result(test_case)
                if failure_details:
                    results.append({
                        "test_name": test_case['description'],
                        "test_number": i,
                        "status": "FAILED",
                        "details": failure_details
                    })
                else:
                    results.append({
                        "test_name": test_case['description'],
                        "test_number": i,
                        "status": "PASSED"
                    })
            except Exception as e:
                results.append({
                    "test_name": test_case['description'],
                    "test_number": i,
                    "status": "FAILED",
                    "details": [str(e)]
                })

        signal.alarm(0)  # Disable the alarm

        # Write results to test_output.txt
        with open('test_output.txt', 'w') as f:
            for result in results:
                f.write(f"TEST_{result['test_number']}, {result['status']}\n")
                if "details" in result:
                    for detail in result["details"]:
                        f.write(f"  - {detail}\n")

        return json.dumps(results, indent=4)

    except TimeoutException:
        # Return failure for all tests if the timeout is reached
        results = []
        for i, test_case in enumerate(test_cases, 1):
            results.append({
                "test_name": test_case['description'],
                "test_number": i,
                "status": "FAILED",
                "details": ["Timeout reached."]
            })

        # Write results to test_output.txt
        with open('test_output.txt', 'w') as f:
            for result in results:
                f.write(f"TEST_{result['test_number']}, {result['status']}\n")
                if "details" in result:
                    for detail in result["details"]:
                        f.write(f"  - {detail}\n")

        return json.dumps(results, indent=4)

def main():
    output = run_tests()
    print(output)

if __name__ == "__main__":
    main()
