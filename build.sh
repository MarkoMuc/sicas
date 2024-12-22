#!/bin/bash
INCLUDES="-I ./includes/"
DBG_CFLAGS=" -Wall -ggdb -fanalyzer"
REL_CFLAGS="-O2"

in="$1"
mode=${in:-"dbg"}

if [ "$mode" == "dbg" ]; then
	mkdir -p ./target/dbg
	gcc $DBG_CFLAGS $INCLUDES -o ./target/dbg/sicas src/* -DDEBUG_MODE
elif [ "$mode" == "rel" ]; then 
	mkdir -p ./target/bin
	gcc $REL_CFLAGS $INCLUDES -o ./target/bin/sicas src/*
elif [ "$mode" == "clean" ]; then 
	rm -rf ./target
elif [ "$mode" == "test" ]; then
	mkdir -p ./test/out
	for i in $(ls ./test/test-cases| grep ".s"); do
		./target/dbg/sicas "./test/$i" > "./test/out/$i.out"
		diff "./test/out/$i.out" "./test/cmpr/$i.res"
	done 
fi

