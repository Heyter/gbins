dofile("../common.lua")
SOLUTION "gluac"

	language "C"
	
	flags { "Symbols", "NoEditAndContinue", "NoPCH", "StaticRuntime", "EnableSSE" }
		
	project "gluac"
		flags{ "Optimize", "FloatFast" }
		defines {}
		files { "src/**.*" }
		kind "ConsoleApp"
		