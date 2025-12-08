#!/usr/bin/env python3
"""
Integration test runner for clox compiler.

Runs .lox test files and validates output against expectations
embedded in comments.
"""

import subprocess
import sys
import os
from pathlib import Path
import re


class TestResult:
    def __init__(self, name, passed, message=""):
        self.name = name
        self.passed = passed
        self.message = message


def parse_expectations(content):
    """Extract expected outputs and errors from test file comments."""
    expect_output = []
    expect_error = None
    expect_runtime_error = None

    for line in content.split('\n'):
        # Look for: // expect: <output>
        output_match = re.search(r'//\s*expect:\s*(.+)', line)
        if output_match:
            expect_output.append(output_match.group(1))

        # Look for: // expect compile error
        if re.search(r'//\s*expect compile error', line):
            expect_error = "compile"

        # Look for: // expect runtime error: <message>
        runtime_match = re.search(r'//\s*expect runtime error:\s*(.+)', line)
        if runtime_match:
            expect_runtime_error = runtime_match.group(1)

    return expect_output, expect_error, expect_runtime_error


def run_test(test_file, interpreter='./clox'):
    """Run a single test file and validate output."""
    test_name = str(test_file.relative_to('test/integration'))

    # Read test file
    try:
        with open(test_file) as f:
            content = f.read()
    except Exception as e:
        return TestResult(test_name, False, f"Failed to read test file: {e}")

    # Parse expectations
    expect_output, expect_error, expect_runtime_error = parse_expectations(content)

    # Run interpreter
    try:
        result = subprocess.run(
            [interpreter, str(test_file)],
            capture_output=True,
            text=True,
            timeout=5
        )
    except subprocess.TimeoutExpired:
        return TestResult(test_name, False, "Test timed out after 5 seconds")
    except Exception as e:
        return TestResult(test_name, False, f"Failed to run interpreter: {e}")

    # Validate compile error expectation
    if expect_error == "compile":
        if result.returncode == 65:
            return TestResult(test_name, True)
        else:
            return TestResult(test_name, False,
                            f"Expected compile error (exit 65), got exit {result.returncode}")

    # Validate runtime error expectation
    if expect_runtime_error:
        if result.returncode == 70:
            # Check if error message matches
            if expect_runtime_error in result.stderr:
                return TestResult(test_name, True)
            else:
                return TestResult(test_name, False,
                                f"Expected runtime error: {expect_runtime_error}\n"
                                f"Got: {result.stderr}")
        else:
            return TestResult(test_name, False,
                            f"Expected runtime error (exit 70), got exit {result.returncode}")

    # Validate success case
    if result.returncode != 0:
        return TestResult(test_name, False,
                        f"Expected success (exit 0), got exit {result.returncode}\n"
                        f"stderr: {result.stderr}")

    # Validate output
    actual_output = result.stdout.strip().split('\n') if result.stdout.strip() else []

    if len(expect_output) != len(actual_output):
        return TestResult(test_name, False,
                        f"Expected {len(expect_output)} lines of output, got {len(actual_output)}\n"
                        f"Expected: {expect_output}\n"
                        f"Got: {actual_output}")

    for i, (expected, actual) in enumerate(zip(expect_output, actual_output)):
        if expected != actual:
            return TestResult(test_name, False,
                            f"Line {i+1} mismatch:\n"
                            f"  Expected: {expected}\n"
                            f"  Got:      {actual}")

    return TestResult(test_name, True)


def find_tests(test_dir='test/integration'):
    """Find all .lox test files in the test directory."""
    test_path = Path(test_dir)
    if not test_path.exists():
        return []
    return sorted(test_path.glob('**/*.lox'))


def print_results(results):
    """Print test results with summary."""
    passed = sum(1 for r in results if r.passed)
    failed = sum(1 for r in results if not r.passed)

    # Print individual results
    for result in results:
        if result.passed:
            print(f"✓ PASS: {result.name}")
        else:
            print(f"✗ FAIL: {result.name}")
            if result.message:
                for line in result.message.split('\n'):
                    print(f"    {line}")

    # Print summary
    print()
    print("=" * 60)
    print(f"Tests run: {len(results)}")
    print(f"Passed:    {passed}")
    print(f"Failed:    {failed}")
    print("=" * 60)

    return failed == 0


def main():
    # Check if clox executable exists
    if not os.path.exists('./clox'):
        print("Error: clox executable not found. Run 'make' first.")
        return 1

    # Find and run all tests
    test_files = find_tests()

    if not test_files:
        print("No test files found in test/integration/")
        print("Create .lox files with '// expect:' comments to add tests.")
        return 0

    print(f"Running {len(test_files)} integration tests...\n")

    results = []
    for test_file in test_files:
        result = run_test(test_file)
        results.append(result)

    # Print results
    success = print_results(results)

    return 0 if success else 1


if __name__ == '__main__':
    sys.exit(main())
