#!/bin/bash
in="$1"
mode=${in:-"zip"}

if [ "$mode" == "zip" ]; then
	zip -r test-cases ./test-cases/*.s
elif [ "$mode" == "unzip" ]; then 
	unzip *.s ./test-cases
elif [ "$mode" == "clean" ]; then 
	echo "TODO"
fi
