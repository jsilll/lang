# Lang Compiler (Name TBD)

This project is an LLVM-based compiler for a custom programming language called "Lang".

## Prerequisites

- CMake (version 3.15 or higher)
- LLVM (version 14.0.0 or higher)
- A C++17 compatible compiler

## Building the Project

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/lang.git
   cd lang
   ```

2. Create a build directory:
   ```
   mkdir build && cd build
   ```

3. Configure the project with CMake:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   cmake --build .
   ```

## Running the Compiler

After building, you can run the compiler using:

```
./src/compiler <input_file> [options]
```

Available options:
- `--emit=lex`: Dump the lexed tokens of the input file
- `--emit=ast`: Dump the abstract syntax tree of the input file

## Design Decisions

1. **Memory Management**: The project uses a custom Arena allocator for efficient memory management of AST nodes.

2. **Error Reporting**: The compiler implements a robust error reporting system that provides detailed information about lexing and parsing errors.

3. **LLVM Integration**: The project leverages LLVM libraries for code generation and optimization.

4. **Modular Architecture**: The compiler is divided into distinct phases (lexing, parsing, etc.) for better maintainability and extensibility.

5. **Type System**: A basic type system is implemented, with room for future expansion.

## Project Structure

- `src/`: Contains the main source files
- `include/`: Header files
- `CMakeLists.txt`: CMake configuration files

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
