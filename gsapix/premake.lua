--DEBUG=true
--DEBUGGING=true

dofile("../common.lua")

SOLUTION"gsapix"
	
	INCLUDES	"steamworks"
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	INCLUDES	"hooking"
	INCLUDES	"sigscanning"
	INCLUDES	"lua51"
	
	WINDOWS()
	
	LINUX()
		
	PROJECT()
		
		SOURCE_SDK_LINKS	()
		INCLUDES			"sigscanning"
		INCLUDES			"hooking"
		INCLUDES	"lua51"
		files 				"src/csteamid.cpp"
		
		configuration 		"windows"
		
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }
