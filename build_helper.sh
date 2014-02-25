#!/bin/bash
export VERBOSE="verbose=1"
../premake5 --os=linux --file=premake.lua gmake
