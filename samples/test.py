import sys
import subprocess


def test_compiler_valid(compiler: str, files: list[str]) -> None:
    """
    Test the compiler with the given files.

    Args:
        compiler (str): The compiler to test
        files (list[str]): A list of file names
    """

    for file in files:
        res = subprocess.run(
            [compiler, file],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        # Assert that the compiler returned successfully
        if res.returncode != 0:
            print(f"\nError: {file} did not return successfully")
            print(res.stderr)
            sys.exit(1)

        # Assert stderr is empty
        if res.stderr:
            print(f"\nError: {file} has non-empty stderr\n")
            print(res.stderr)
            sys.exit(1)

        # Assert that the compiler emits the correct output
        print(f"- Test passed: {file}")


def test_compiler_error(compiler: str, files: list[tuple[str, str, str]]) -> None:
    """
    Test the compiler's error handling by checking if the compiler emits the correct error
    for the given files.

    Args:
        compiler (str): The compiler to test
        files (list[tuple[str, str, str]]): A list of tuples containing the file name, the expected error id and the location
    """
    import json

    for file, error_id, location in files:
        res = subprocess.run(
            [compiler, "--error-format=json", file],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        # Assert that the compiler returned an error
        if res.returncode == 0:
            print(f"\nError: {file} did not return an error")
            sys.exit(1)

        # Assert stderr is not empty
        if not res.stderr:
            print(f"\nError: {file} has empty stderr")
            sys.exit(1)

        # Assert stderr is a valid JSON
        error_json = None
        try:
            error_json = json.loads(res.stderr)
        except json.JSONDecodeError:
            print(f"\nError: {file} has invalid JSON in stderr")
            print(res.stderr)
            sys.exit(1)

        # Check the length of the errors is 1
        if len(error_json) != 1:
            print(f"\nError: {file} has more than one error")
            print(res.stderr)
            sys.exit(1)

        # Check the error id
        if error_json[0]["id"] != error_id:
            print(
                f"\nError: {file} has incorrect error id: got {error_json[0]['id']}, expected {error_id}"
            )
            sys.exit(1)

        # Check the error location
        if error_json[0]["loc"] != location:
            print(
                f"\nError: {file} has incorrect error location: got {error_json[0]['loc']}, expected {location}"
            )
            sys.exit(1)

        # Assert that the correct error is emitted
        print(f"- Test passed: {file}")


def main():
    import argparse

    FILE_EXT = "lang"
    VALID_PATH = "samples/valid"
    ERROR_PATH = "samples/error"

    # Parse the command line arguments

    parser = argparse.ArgumentParser(
        description="A testing script for compiler's error handling"
    )

    parser.add_argument("compiler", help="The compiler to test")

    # TODO: Actually implement this
    parser.add_argument(
        "-p",
        "--pipeline",
        help="The pipeline to test",
        choices=["lex", "syn", "output", "all"],
        default="all",
    )

    # Parse command line arguments

    args = parser.parse_args()

    # Test the compiler with the valid files

    valid_test_set = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]

    valid_files = map(
        lambda file: f"{VALID_PATH}/{file}.{FILE_EXT}",
        valid_test_set,
    )

    print("Testing the compiler with the valid files:")
    test_compiler_valid(args.compiler, valid_files)

    # Test the compiler with the error files

    error_test_set = [
        ("01", "lex-invalid-char", "1:1"),
        ("02", "parse-unexpected-eof", "1:1"),
        ("03", "parse-unexpected-token", "2:9"),
        # ("1", "cfa-early-return-stmt", "2:5"),
        # ("2", "cfa-invalid-break-stmt", "2:5"),
    ]

    error_files = map(
        lambda file: (
            f"{ERROR_PATH}/{file[0]}.{FILE_EXT}",
            file[1],
            f"{ERROR_PATH}/{file[0]}.{FILE_EXT}:{file[2]}",
        ),
        error_test_set,
    )

    print("\nTesting the compiler with the error files:")
    test_compiler_error(args.compiler, error_files)


if __name__ == "__main__":
    main()
