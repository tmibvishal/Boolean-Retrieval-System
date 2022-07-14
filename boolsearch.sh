#!/bin/bash

# verifying the arguments
if [[ $# -ne 4 ]]; then
    echo "usage: $(basename "$0") [queryfile] [resultfile] [indexfile] [dictfile]" <&2
    exit -1
fi

./bin/boolsearch $1 $2 $3 $4

rm -fr ./temp/*
