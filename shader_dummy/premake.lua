dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"game_shader_generic_garrysmod_srv"
	
	INCLUDES	"source_sdk"
	defines		{"PRELOADPLUGIN","GMMODULE","GAME_DLL"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		
	        targetprefix                    ""
		
		configuration 		"windows"
        	        targetsuffix            ""
		configuration 		"linux"
	                targetsuffix            ""
	                targetextension         ".so"
					