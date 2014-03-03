dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"geoip"
	
	targetdir	"Release"
	INCLUDES	"gmod_sdk"
	includedirs	{"geoip/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		libdirs		{"geoip/libGeoIP"}
		includedirs	{"geoip/libGeoIP"}
		links		{"Ws2_32","GeoIP.lib"}
	
	LINUX()
		libdirs		{"geoip/libGeoIP/.libs","geoip/libGeoIP","libGeoIP","geoip/lib"}
		links_static "GeoIP"

	PROJECT()
		configuration 		"windows"
			files{"geoip/libGeoIP/GeoIP_deprecated.c"}
		configuration 		"linux"
