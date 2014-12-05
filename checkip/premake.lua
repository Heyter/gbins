dofile("../common.lua")

RequireDefaultlibs()

SOLUTION "checkip"

	INCLUDES "source_sdk"
	INCLUDES "hooking"
	INCLUDES "sigscanning"
	INCLUDES "gmod_sdk"

	WINDOWS()
	LINUX()

	PROJECT()
		INCLUDES "hooking"
		INCLUDES "sigscanning"

		SOURCE_SDK_LINKS()

		buildoptions { "-std=c++11" }

		configuration "linux"
			buildoptions { "-fpermissive" }