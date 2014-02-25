dofile("../common.lua")

--RequireDefaultlibs()
--RequireRuntime()


SOLUTION"cares"
	
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	includedirs	{"cares/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		libdirs		{"cares/msvc100/cares/lib-release/","cares/msvc100/cares/lib-release/lib/"}
		links		{"Ws2_32","libcares","tier0"}
		
	LINUX()
		libdirs		{"cares/.libs"}
		links_static "cares"
		
	PROJECT()
		targetprefix		"gmsv_lib" -- hack