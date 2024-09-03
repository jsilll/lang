set -e
cmake -S . -B build
cmake --build build
for file in ./samples/*.lang; do
    echo -e "\033[1;4mCompiling $file\033[0m"
    ./build/src/compiler $file $@ --emit=ast
done
