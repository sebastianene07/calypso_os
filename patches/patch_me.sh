#!/bin/sh

cd ./lib/fatfs && git apply --reject --whitespace=nowarn --ignore-space-change --ignore-whitespace  ../patches/patch_fix_compilation_warnings 2> /dev/null
