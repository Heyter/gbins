#!/bin/bash
export VERBOSE="verbosee=1"
../premake5 --os=linux --file=premake.lua gmake
