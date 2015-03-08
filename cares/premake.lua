dofile("../common.lua")

--RequireDefaultlibs()
--RequireRuntime()


SOLUTION"cares"
	language"C"
	
	includedirs	"cares/"
	INCLUDES	"lua51"
	
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		libdirs		{
			"cares/msvc120/cares/lib-release/",
			"cares/msvc120/cares/lib-release/lib/",
			"cares/msvc110/cares/lib-release/",
			"cares/msvc110/cares/lib-release/lib/",
			"cares/msvc100/cares/lib-release/",
			"cares/msvc100/cares/lib-release/lib/",
			
			}
		links		{"Ws2_32","libcares"}
		
	LINUX()
		libdirs		{"cares/.libs"}
		links_static "cares"
		
	PROJECT()
		targetprefix		"gmsv_lib" -- hack
		files	{"src/*.c"}
		INCLUDES	"lua51"
		
		
