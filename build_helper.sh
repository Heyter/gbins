#!/bin/bash
export VERBOSE="verbosee=1"
../premake4 --os=linux --platform=x32 --file=premake.lua gmake
