dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"vfs"

	INCLUDES		"source_sdk"
	INCLUDES		"gmod_sdk"

	WINDOWS()
	LINUX()

	PROJECT()
		targetprefix	"gmcl_"
		SOURCE_SDK_LINKS()