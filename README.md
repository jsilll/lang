# Lang Compiler (Name TBD)

This project is an LLVM-based compiler for a custom programming language called "Lang".

## Prerequisites

- CMake (version 3.15 or higher)
- LLVM (version 14.0.0 or higher)
- A C++17 compatible compiler

## Building the Project

You can use the provided `build.sh` script to configure, clean, and build the project:

1. Clone the repository:
   ```
   git clone https://github.com/jsilll/lang.git
   cd lang
   ```

2. Make the build script executable:
   ```
   chmod +x build.sh
   ```

3. Configure the project:
   ```
   ./build.sh configure
   ```

4. Build the project:
   ```
   ./build.sh build
   ```

Alternatively, you can run all steps at once:
```
./build.sh all
```

The `build.sh` script supports several options:
- `--llvm-dir <path>`: Set LLVM_DIR (default: /usr/lib/llvm-14)
- `--build-type <type>`: Set CMAKE_BUILD_TYPE (default: Debug)
- `--c-compiler <path>`: Set CMAKE_C_COMPILER (default: clang)
- `--cxx-compiler <path>`: Set CMAKE_CXX_COMPILER (default: clang++)
- `--ccache <path>`: Set CMAKE_CXX_COMPILER_LAUNCHER (default: ccache)

## Running the Compiler

After building, you can run the compiler using:

```
./src/compiler <input_file> [options]
```

Available options:
- `--until=<stage>`: Run the compiler until a specific stage
  - `lex`: Run until the lexing stage
  - `ast`: Run until the parsing stage
- `--emit=<output>`: Select the kind of output desired
  - `lex`: Dump the lexed tokens of the input file
  - `src`: Dump the original source code of the input file (not implemented yet)
  - `ast`: Dump the abstract syntax tree of the input file

## Design Decisions

1. **Memory Management**: The project uses an Arena allocator for efficient memory management of AST nodes.

2. **Error Reporting**: The compiler implements a robust error reporting system that provides detailed information about lexing and parsing errors.

3. **LLVM Integration**: The project leverages LLVM libraries for code generation and optimization.

4. **Modular Architecture**: The compiler is divided into distinct phases (lexing, parsing, etc.) for better maintainability and extensibility.

5. **Type System**: A basic type system is implemented, with a TypeContext for managing types.

6. **Resolver**: A resolver stage is implemented to perform semantic analysis on the AST.

## Project Structure

- `src/`: Contains the main source files
  - `Alloc/`: Custom allocators
  - `AST/`: Abstract Syntax Tree related code
  - `Lex/`: Lexer implementation
  - `Parse/`: Parser implementations
  - `Sema/`: Semantic analysis code
  - `Support/`: Utility classes and functions
  - `main.cpp`: Entry point of the compiler
- `include/`: Header files
  - `ADT/`: Abstract Data Types
- `CMakeLists.txt`: CMake configuration files

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
