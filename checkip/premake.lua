dofile("../common.lua")

RequireDefaultlibs()

SOLUTION "checkip"

	INCLUDES        "source_sdk"
	INCLUDES        "hooking"
	INCLUDES        "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
		INCLUDES        "hooking"
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }