#!/bin/sh
. common.sh $*
perl tests.pl "$TESTSPATH" memory-read-rom
perl tests.pl "$TESTSPATH" memory-read-write-ram
