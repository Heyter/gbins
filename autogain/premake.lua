DEBUG=true
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
		includedirs"src/speex"
		includedirs"src/silk"
		includedirs"src"
		libdirs"src/silk"
		libdirs"src/speex"
		
		INCLUDES        "lua51"
		INCLUDES        "hooking"
		INCLUDES        "sigscanning"
		
		configuration 		"windows"
		configuration 		"linux"
			linkoptions			"../../src/speex/libspeex.a"
			linkoptions			"../../src/silk/libSKP_SILK_SDK.a"
			buildoptions 		{ "-fpermissive" }
			linkoptions		{"-Wl,-rpath='$$ORIGIN'"}
			linkoptions		{"garrysmod/bin/server_srv.so"}
			prelinkcommands	{"mkdir -p garrysmod/bin && ln -fs "..SRCDS_DIR.."/garrysmod/bin/server_srv.so garrysmod/bin/server_srv.so "}
			
