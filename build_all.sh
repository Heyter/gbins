#!/bin/bash

for f in */build; do
	path=$(dirname $f)
	(cd path;./build)
done
