#!/bin/sh

set -xe

CC="${CXX:-cc}"

gcc kv.c -o kv -Wall -ggdb -O3 -std=c11 -pedantic -lraylib
