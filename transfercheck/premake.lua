dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"transfercheck"

	INCLUDES        "sigscanning"
	INCLUDES        "lua51"
	--INCLUDES        "source_sdk"
	INCLUDES        "hooking"
	--INCLUDES        "steamworks"
	--INCLUDES        "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }