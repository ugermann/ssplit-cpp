#!/bin/bash

# Utility to run ssplit locally.

if [ -z ${TEST_BINARY+UNSET} ]; then
	echo "Usage: TEST_BINARY=<> $0"
	echo "TEST_BINARY is not set."
	exit 1
fi

SCRIPT_DIR=`dirname $0`
TESTS="${SCRIPT_DIR}/../tests"

${TEST_BINARY} \
	-p nonbreaking_prefixes/nonbreaking_prefix.en \
	-i ${TESTS}/sample.en \
	-m p -e ${TESTS}/sample.en.one_paragraph_per_line.expected  &> /dev/null \
	|| echo "Paragraph per line test failed"

${TEST_BINARY} \
	-p nonbreaking_prefixes/nonbreaking_prefix.en \
	-i ${TESTS}/sample.en \
	-m s -e ${TESTS}/sample.en.one_sentence_per_line.expected  &> /dev/null \
	|| echo "One sentence per line test failed"

${TEST_BINARY} \
	-p nonbreaking_prefixes/nonbreaking_prefix.en \
	-i ${TESTS}/sample.en.wrapped \
	-m w -e ${TESTS}/sample.en.wrapped.expected &> /dev/null \
	|| echo "Wrapped text test failed"

echo "Tests completed successfully"
