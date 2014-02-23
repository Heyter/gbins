dofile("../common.lua")

SOLUTION"cares"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	includedirs	{"cares/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		libdirs		{"cares/msvc100/cares/lib-release/","cares/msvc100/cares/lib-release/lib/"}
		links		{"Ws2_32","libcares"}
	
	LINUX()
		libdirs		{"cares/.libs"}
		links_static "cares"
		
	PROJECT()
		targetprefix		"gmsv_lib" -- hack
		
		configuration 		"windows"
			SOURCE_SDK_LINKS(true)

		configuration 		"not windows"
			SOURCE_SDK_LINKS(false)
