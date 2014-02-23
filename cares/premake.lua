dofile("../common.lua")

SOLUTION"cares"
	
	targetdir("Release")
	INCLUDES(true,false,true)
	includedirs	({"cares/"})
	defines({"NDEBUG","USE_WINSOCK"})
	
	WINDOWS() -- cares does not autobuild!
		libdirs		{"cares/msvc100/cares/lib-release/","cares/msvc100/cares/lib-release/lib/"}
		links		{"Ws2_32","libcares"}
	
	LINUX() -- cares does not autobuild!
		links		{"cares"}
		libdirs		{"cares/linux/cares/lib-release/"} -- wrong path!
		linkoptions({"-Wl,-Bstatic,-lcares,-Bdynamic"})

	PROJECT()
		configuration 		"windows"
			SOURCE_SDK_LINKS(true)

		configuration 		"not windows"
			SOURCE_SDK_LINKS(false)