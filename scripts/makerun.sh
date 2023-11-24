#!/bin/bash
BASE_DIR=$(dirname "$0")/..
REAL_DIR=$(realpath $BASE_DIR)
(cd $REAL_DIR && make clean && make && ./bin/main "${@:1}")