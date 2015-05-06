dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"autogain"

	INCLUDES        "sigscanning"
	INCLUDES        "lua51"
	INCLUDES        "source_sdk"
	INCLUDES        "hooking"
	--INCLUDES        "steamworks"
	--INCLUDES        "gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()

		SOURCE_SDK_LINKS()
		INCLUDES        "lua51"
		INCLUDES        "hooking"
		INCLUDES        "sigscanning"
		configuration 		"windows"
		configuration 		"linux"
			buildoptions 		{ "-fpermissive" }
			linkoptions		{"-Wl,-rpath='$$ORIGIN'"}
			linkoptions		{"garrysmod/bin/server_srv.so"}
			prelinkcommands	{"mkdir -p garrysmod/bin && ln -fs "..SRCDS_DIR.."/garrysmod/bin/server_srv.so garrysmod/bin/server_srv.so "}
			
