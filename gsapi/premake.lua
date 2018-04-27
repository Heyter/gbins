dofile("../common.lua")

--RequireDefaultlibs()
RequireRuntime()

SOLUTION"gameserver"

	includedirs{STEAMWORKS_SDK..'/public'}
	includedirs{STEAMWORKS_SDK..'/public/steam'}

	INCLUDES	"gmod_sdk"

	includedirs(SOURCE_SDK_INCLUDES)



	defines {"GMOD_ALLOW_DEPRECATED", "GMOD_USE_SOURCESDK"}

	WINDOWS()
	LINUX()

	PROJECT()
		-- we have to manually define these because source-sdk now comes with a steam_api.lib
		-- that doesn't contain some symbols we need, so we use steamworks' first instead
		libdirs	{
			STEAMWORKS_SDK..'/redistributable_bin',
			STEAMWORKS_SDK..'/redistributable_bin/linux32'
		}

		libdirs{
			SOURCE_SDK.."/lib/public/linux32",
			SOURCE_SDK.."/lib/linux32",
			SOURCE_SDK.."/lib/public"
		}

		SOURCE_SDK_LINKS()

		configuration 		"windows"
			links "steam_api"
			links "legacy_stdio_definitions" -- fixes some printf link issues
			links		{"Ws2_32"}
		configuration 		"linux"
