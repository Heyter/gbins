
dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"geoip"
	
	targetdir	"Release"
	INCLUDES	"gmod_sdk"
	defines		{"NDEBUG"}
	includedirs	"geoip/"
	includedirs	"geoip/libGeoIP"

	
	WINDOWS()
		links		{"Ws2_32"}
	
	LINUX()

	PROJECT()
		links "geoiplib"
		configuration 		"windows"
		configuration 		"linux"


	project"geoiplib"
		kind "StaticLib"
		language "C"
		
		defines	{"GEOIP_EXPORTS","_CRT_SECURE_NO_WARNINGS"}
		
		files{
			"geoip/libGeoIP/GeoIP.c",
			"geoip/libGeoIP/GeoIPCity.c",
			"geoip/libGeoIP/GeoIP_deprecated.c",
			"geoip/libGeoIP/regionName.c",
			"geoip/libGeoIP/timeZone.c",

		}
		
		WINDOWS()
			defines	[[PACKAGE_VERSION="1.6.0"]]
			files{
				"geoip/libGeoIP/pread.c",

			}
		LINUX()
			defines	[[PACKAGE_VERSION=\"1.6.0\"]]
			defines[[GEOIPDATADIR=\".\"]]
		