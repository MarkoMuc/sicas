#!/bin/bash

set -xe

INCLUDES="-I ./includes/"
DBG_CFLAGS=" -Wall -ggdb -fanalyzer -pedantic -std=c99"
REL_CFLAGS="-O2"
DEBUG_FLAG="-D""DEBUG_MODE"

mode=${1:-"dbg"}
execute=${2:-""}

if [ "$mode" == "run" ]; then
	mode="dbg"
	execute="run"
fi

if [ "$mode" == "dbg" ]; then
	mkdir -p ./target/dbg
	target_path="./target/dbg/sicas"
	gcc $DBG_CFLAGS $INCLUDES -o "$target_path" src/* "$DEBUG_FLAG"
elif [ "$mode" == "rel" ]; then 
	mkdir -p ./target/rel
	target_path="./target/rel/sicas"
	gcc $REL_CFLAGS $INCLUDES -o "$target_path" src/*
elif [ "$mode" == "clean" ]; then 
	rm -rf ./target
	exit 0
elif [ "$mode" == "test" ]; then
	mkdir -p ./test/out
	for i in $(ls ./test/test-cases| grep ".s"); do
		./target/dbg/sicas "./test/$i" > "./test/out/$i.out"
		diff "./test/out/$i.out" "./test/cmpr/$i.res"
	done
	exit 0
fi

if [ "$execute" == "run" ]; then
	"$target_path"
fi

exit 0
