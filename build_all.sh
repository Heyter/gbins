#!/bin/bash

for f in */build; do
	path=$(dirname $f)
	echo "Building $path"
	(cd "$path";./build)
	echo press enter to continue...
	read temppage
done
