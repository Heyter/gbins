dofile("../common.lua")

SOLUTION"geoip"
	
	targetdir	"Release"
	INCLUDES	(false,true,false)
	includedirs	{"geoip/"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"NNOOOOO"}
		libdirs		{"geoip/libGeoIP"}
		links		{"GeoIP"}
	
	LINUX()
		libdirs		{"libGeoIP","geoip/lib"}
		linkoptions	{"-Wl,-Bstatic,-lGeoIP,-Bdynamic"}

	PROJECT()
		configuration 		"windows"
			SOURCE_SDK_LINKS(true)

		configuration 		"not windows"
			SOURCE_SDK_LINKS(false)
