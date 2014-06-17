dofile("../common.lua")

--RequireDefaultlibs()
RequireRuntime()

SOLUTION"sourcenet"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"hooking"
	INCLUDES	"sigscanning"
	INCLUDES	"backwards_headers"
	includedirs	{	
					"src",
					"src/sourcenet",
					"src/engine",
					"src/simplescan",
					"src/common"
				}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		links		{"Ws2_32"}
	
	LINUX()
		links		{"dl"}
		
	PROJECT()
		files 
		{
			"src/sourcenet/*.cpp",
			"src/engine/*.cpp",
			"src/simplescan/*.cpp",
		}
		
		SOURCE_SDK_LINKS()
		INCLUDES			"sigscanning"
		
		configuration 		"windows"
			defines 		"SOURCENET_HOOKING"
		configuration 		"linux"
			defines 		"_LINUX"
			buildoptions 	{ "-fpermissive" }