BOLD=$(tput bold)
UNDERLINE=$(tput smul)
RESET=$(tput sgr0)
set -e
./build.sh build
for file in ./samples/*.lang; do
    echo -e "\n${BOLD}${UNDERLINE}Compiling $file${RESET}"
    ./build/src/compiler $file --emit=ast $@
done
