dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"checkip"

	INCLUDES        "sigscanning"
	INCLUDES        "lua51"
	INCLUDES        "source_sdk"
	INCLUDES        "hooking"
	INCLUDES        "steamworks"
	--INCLUDES        "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
		INCLUDES        "lua51"
		INCLUDES        "sigscanning"
		INCLUDES        "hooking"
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }