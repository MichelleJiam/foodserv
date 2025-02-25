#!/usr/bin/env bash

# Colours
BOLD="\033[0;1m"
RED="\033[0;31m"
CYAN="\033[0;36m"
PURP="\033[0;35m"
GREEN="\033[0;32m"
BLUE="\033[0;34m"
BLUEBG="\033[44m"
WHITE="\033[1;97m"
RESET="\033[0m"

choose_test () {
    printf "\n${CYAN}"
	read -r -n 1 -p $'\nPress [1] for basic test output. Press [2] for visual test output.\n\n' choice
	printf "${RESET}\n\n"
	case $choice in
		1)
			cp -r config_files build/ && cd build && ctest
			;;
		2)
			cp -r config_files build/ && cd build && ./$bin_name
			;;
		*) # any other input
			exit 0
			;;
	esac
}

cmake .. -DBUILD_GMOCK=OFF -S . -B build
cmake --build build | tee -a build_out # output cmake build to stdout & also file
# grab last line starting with [100%] and greps only the binary name
bin_name=$(grep -E '^\[100%\]' build_out | tail -n 1 | grep -oE '[^ ]+$')
rm build_out
choose_test "$1"