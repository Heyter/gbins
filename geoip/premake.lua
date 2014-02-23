dofile("../common.lua")

SOLUTION"geoip"
	
	targetdir	"Release"
	INCLUDES	(false,true,false)
	includedirs	{"geoip/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"NNOOOOO"}
		libdirs		{"geoip/libGeoIP"}
		includedirs	{"geoip/libGeoIP"}
		links		{"Ws2_32","GeoIP"}
	
	LINUX()
		libdirs		{"libGeoIP","geoip/lib"}
		linkoptions	{"-Wl,-Bstatic,-lGeoIP,-Bdynamic"}

	PROJECT()
		configuration 		"windows"
			files{"geoip/libGeoIP/GeoIP_deprecated.c"}
		configuration 		"not windows"
