if __name__ == "__main__":
    import pytest

    pytest.main([__file__])


import json


def test_valid_01() -> None:
    res = compile_program("samples/valid/01.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_02() -> None:
    res = compile_program("samples/valid/02.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_03() -> None:
    res = compile_program("samples/valid/03.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_04() -> None:
    res = compile_program("samples/valid/04.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_05() -> None:
    res = compile_program("samples/valid/05.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_06() -> None:
    res = compile_program("samples/valid/06.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_07() -> None:
    res = compile_program("samples/valid/07.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_08() -> None:
    res = compile_program("samples/valid/08.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_09() -> None:
    res = compile_program("samples/valid/09.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_valid_10() -> None:
    res = compile_program("samples/valid/10.lang")

    assert res.returncode == 0
    assert not res.stderr


def test_error_01() -> None:
    res = compile_program("samples/error/01.lang", "--error-format=json")

    assert res.returncode != 0
    assert res.stderr

    error = parse_json(res.stderr)

    assert len(error) == 1
    assert error[0]["id"] == "lex-invalid-char"
    assert error[0]["loc"] == "samples/error/01.lang:1:1"


def test_error_02() -> None:
    res = compile_program("samples/error/02.lang", "--error-format=json")

    assert res.returncode != 0
    assert res.stderr

    error = parse_json(res.stderr)

    assert len(error) == 1
    assert error[0]["id"] == "parse-unexpected-eof"
    assert error[0]["loc"] == "samples/error/02.lang:1:1"


def test_error_03() -> None:
    res = compile_program("samples/error/03.lang", "--error-format=json")

    assert res.returncode != 0
    assert res.stderr

    error = parse_json(res.stderr)

    assert len(error) == 1
    assert error[0]["id"] == "parse-unexpected-token"
    assert error[0]["loc"] == "samples/error/03.lang:2:9"


def test_error_04() -> None:
    res = compile_program("samples/error/04.lang", "--error-format=json")

    assert res.returncode != 0
    assert res.stderr

    error = parse_json(res.stderr)

    assert len(error) == 1
    assert error[0]["id"] == "cfa-early-return-stmt"
    assert error[0]["loc"] == "samples/error/04.lang:2:5"


def test_error_05() -> None:
    res = compile_program("samples/error/05.lang", "--error-format=json")

    assert res.returncode != 0
    assert res.stderr

    error = parse_json(res.stderr)

    assert len(error) == 1
    assert error[0]["id"] == "cfa-invalid-break-stmt"
    assert error[0]["loc"] == "samples/error/05.lang:2:5"


from subprocess import CompletedProcess


def parse_json(s: str) -> dict:
    import json

    try:
        return json.loads(s)
    except json.JSONDecodeError as e:
        print(e)
        assert False


def compile_program(file: str, opts: str = "") -> CompletedProcess[str]:
    import subprocess

    """
    Compile the given program through the compiler executable and return the output.

    Args:
        program (str): The program to compile.

    Returns:
        CompletedProcess: The result of the compilation process.
    """
    # Open process with pipes for stdout and stderr
    return subprocess.run(
        ["./build/compiler", file, opts],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
