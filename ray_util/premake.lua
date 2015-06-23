dofile("../common.lua")

RequireDefaultlibs()

SOLUTION "ray_util"

	INCLUDES "source_sdk"
	INCLUDES "gmod_sdk"
	INCLUDES "sigscanning"
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		INCLUDES "sigscanning"
		configuration "windows"
		configuration "linux"
			buildoptions {"-fpermissive"}