dofile("../common.lua")

SOLUTION"nixinfo"
	
	INCLUDES "source_sdk"
	INCLUDES "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
		configuration "windows"
		configuration "linux"

