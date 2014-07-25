dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"ghostinj"

	INCLUDES        "sigscanning"
--	INCLUDES        "lua51"
--	INCLUDES        "source_sdk"
	INCLUDES        "hooking"
	--INCLUDES        "steamworks"
	--INCLUDES        "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
	
        targetprefix                    ""

        configuration                   "windows"
                targetsuffix            ""

        configuration                   "linux"
                targetsuffix            ""
                targetextension         ".dll"

--		SOURCE_SDK_LINKS()
--		INCLUDES        "lua51"
		INCLUDES        "hooking"
		INCLUDES        "sigscanning"
		configuration 		"windows"
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }
