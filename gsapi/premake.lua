--DEBUG=true
dofile("../common.lua")

--RequireDefaultlibs()
RequireRuntime()

SOLUTION"gameserver"
	
	INCLUDES	"steamworks"
	
	INCLUDES	"gmod_sdk"
	INCLUDES	"source_sdk"
	
	--defines		{"DEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			links	"steam_api"
			links		{"Ws2_32"}
		configuration 		"linux"
