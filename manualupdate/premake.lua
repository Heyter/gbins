dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"cllup"

	INCLUDES        "sigscanning"
	INCLUDES        "lua51"
	INCLUDES        "source_sdk"
	--INCLUDES        "hooking"
	--INCLUDES        "steamworks"
	--INCLUDES        "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
		includedirs"/home/srcds/gbins/bootil/include"
		SOURCE_SDK_LINKS()
		INCLUDES        "lua51"
		--INCLUDES        "hooking"
		INCLUDES        "sigscanning"
		configuration 		"windows"
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }