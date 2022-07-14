#!/bin/bash

# verifying the arguments
if [[ $# -ne 5 ]]; then
    echo "usage: $(basename "$0") [coll-path] [indexfile] [stopwordfile] {0|1|2|3|4|5} [xml-tags-info]" <&2
    exit -1
fi

./bin/invidx_cons $1 $2 $3 $4 $5

rm -fr ./temp/*
