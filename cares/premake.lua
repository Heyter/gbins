dofile("../common.lua")

SOLUTION"cares"
	
	targetdir	"Release"
	INCLUDES	(true,false,true)
	includedirs	{"cares/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		libdirs		{"cares/msvc100/cares/lib-release/","cares/msvc100/cares/lib-release/lib/"}
		links		{"Ws2_32","libcares"}
	
	LINUX()
		libdirs		{"cares/.libs"}
		linkoptions({"-Wl,-Bstatic,-lcares,-Bdynamic"})

	PROJECT()
		configuration 		"windows"
			SOURCE_SDK_LINKS(true)

		configuration 		"not windows"
			SOURCE_SDK_LINKS(false)
