#!/bin/bash

set -eo pipefail;

if [ -z ${BINARY_DIR+UNSET} ]; then
	echo "Usage: BINARY_DIR=<> $0"
	echo "BINARY_DIR is not set."
	exit 1
fi

function evaluate-diff {
    NAME=$1
    MODE=$2
    FSUFFIX=$3

    echo "Testing $NAME mode of operation";

    diff -qa <(${BINARY_DIR}/ssplit -m${MODE} -n \
    -p nonbreaking_prefixes/nonbreaking_prefix.en \
    tests/sample.en${FSUFFIX} ) tests/sample.en${FSUFFIX}.m${MODE}.n.expected || (echo " - [FAIL] mapped ${NAME} mode " && return 1); 

    echo " - [SUCCESS] mapped ${NAME} mode";

    diff -qa <(${BINARY_DIR}/ssplit -m${MODE} -n \
    -p nonbreaking_prefixes/nonbreaking_prefix.en \
    < tests/sample.en${FSUFFIX} ) tests/sample.en${FSUFFIX}.m${MODE}.n.expected || (echo " - [FAIL] streamed ${NAME} mode" && return 1);

    echo " - [SUCCESS] streamed ${NAME} mode";
}

evaluate-diff "paragraph" "p" ""
evaluate-diff "sentence" "s" ""
evaluate-diff "wrapped" "w" ".wrapped"
