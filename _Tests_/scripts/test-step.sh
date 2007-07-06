#!/bin/sh
. common.sh $*
perl tests.pl "$TESTSPATH" step 1
perl tests.pl "$TESTSPATH" step 2
perl tests.pl "$TESTSPATH" step 3
