#!/bin/sh

TOPDIR=$(pwd)
cd lib/fatfs && git apply --reject --whitespace=nowarn --ignore-space-change --ignore-whitespace  ../patches/patch_fix_compilation_warnings 2> /dev/null
git am $TOPDIR/patches/0001-Bugfix-Return-correct-status-codes-from-low-level-ac.patch 
git am $TOPDIR/patches/0001-Use-MMC-as-disk-number-0-in-this-system.patch
