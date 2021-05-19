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
    ADDITIONAL_ARGS=$4

    echo "Testing $NAME mode of operation";

    BASE_ARGS=(
        -m${MODE} -n 
        -p nonbreaking_prefixes/nonbreaking_prefix.en 
        ${ADDITIONAL_ARGS[@]} 
    )

    INPUT_FILE="tests/sample.en${FSUFFIX}"
    EXPECTED_OUTPUT="tests/sample.en${FSUFFIX}.m${MODE}.n.expected"

    diff -qa <(${BINARY_DIR}/ssplit ${BASE_ARGS[@]} ${INPUT_FILE}) ${EXPECTED_OUTPUT} || (echo " - [FAIL] mapped ${NAME} mode " && return 1); 
    echo " - [SUCCESS] mapped ${NAME} mode";

    diff -qa <(${BINARY_DIR}/ssplit ${BASE_ARGS[@]} < ${INPUT_FILE}) ${EXPECTED_OUTPUT} || (echo " - [FAIL] streamed ${NAME} mode " && return 1); 
    echo " - [SUCCESS] streamed ${NAME} mode";

}

echo "File based loads"
ADDITIONAL_ARGS=""
evaluate-diff "paragraph" "p" "" ${ADDITIONAL_ARGS}
evaluate-diff "sentence" "s" "" ${ADDITIONAL_ARGS}
evaluate-diff "wrapped" "w" ".wrapped" ${ADDITIONAL_ARGS}

echo "ByteArray based loads"
ADDITIONAL_ARGS="--byte-array=1"
evaluate-diff "paragraph" "p" "" ${ADDITIONAL_ARGS}
evaluate-diff "sentence" "s" "" ${ADDITIONAL_ARGS}
evaluate-diff "wrapped" "w" ".wrapped" ${ADDITIONAL_ARGS}
