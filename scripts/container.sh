#!/bin/bash
BASE_DIR=$(dirname "$0")/..
REAL_DIR=$(realpath $BASE_DIR)
docker build -t dads $REAL_DIR
docker run -it --rm -v $REAL_DIR:/usr/src dads