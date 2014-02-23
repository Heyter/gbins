dofile("../common.lua")

SOLUTION"geoip"
	
	targetdir	"Release"
	INCLUDES	"gmod_sdk"
	includedirs	{"geoip/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		libdirs		{"geoip/libGeoIP"}
		includedirs	{"geoip/libGeoIP"}
		links		{"Ws2_32","GeoIP"}
	
	LINUX()
		libdirs		{"libGeoIP","geoip/lib"}
		links_static "GeoIP"

	PROJECT()
		configuration 		"windows"
			files{"geoip/libGeoIP/GeoIP_deprecated.c"}
		configuration 		"not windows"
