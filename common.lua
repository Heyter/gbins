dofile("settings.lua")
print("SRCDS_DIR",SRCDS_DIR)
print("SOURCE_SDK",SOURCE_SDK)

function HDB(name)
	local f=_G[name]
	_G[name] = function(a,...)
		if type(a)=="string" then
			--print(name,"",a,...)
		else
			--print(name,"",unpack(a))
		end
		return f(a,...)
	end
end

HDB("links"        )
HDB("targetdir"    )
HDB("location"     )
HDB("libdirs"      )
HDB("configuration")
HDB("flags"        )
HDB("solution"     )
HDB("defines"      )
HDB("includedirs"  )
HDB("linkoptions"  )
HDB("project"      )
HDB("files"        )
HDB("targetname"   )

GARRYSMOD_INCLUDES_PATH = "../gmod-module-base/include"
BACKWARDS_HEADERS = "../backwards_headers"
HOOKING = "../hooking"
SIGSCANNING = "../sigscanning"

PROJECT_FOLDER = os.get() .. "/" .. _ACTION
local _INCLUDE_SDK=false
local _INCLUDE_GMOD=false
local _INCLUDE_BACKWARDS=false
local _INCLUDE_STEAMWORKS=false

SOURCE_SDK_INCLUDES={
	SOURCE_SDK.."/public",
	SOURCE_SDK.."/public/engine",
	SOURCE_SDK.."/common",
	SOURCE_SDK.."/public/steam",
	SOURCE_SDK.."/public/tier0",
	SOURCE_SDK.."/public/tier1",
	SOURCE_SDK.."/tier1",
	SOURCE_SDK.."/public/game/server",
	SOURCE_SDK.."/game/shared",
	SOURCE_SDK.."/game/server",
	SOURCE_SDK.."/game/client",
}
function SOURCE_SDK_LINKS()
	local cfg = configuration()
	configuration 		"windows"
		links{"tier0","tier1","tier2","tier3","mathlib","steam_api","vstdlib"}

	configuration 		"not windows"
		linkoptions {  -- :(
			SOURCE_SDK.."/lib/public/linux32/tier1.a",
			SOURCE_SDK.."/lib/public/linux32/tier2.a",
			SOURCE_SDK.."/lib/public/linux32/tier3.a",
			SOURCE_SDK.."/lib/public/linux32/mathlib.a",
			}
		links {"tier0_srv","vstdlib_srv","steam_api"}
	configuration(cfg.terms)
end

local include_helpers = {
	source_sdk={
		func = function()		
			libdirs{
				SOURCE_SDK.."/lib/public/linux32",
				SOURCE_SDK.."/lib/linux32",
				SOURCE_SDK.."/lib/public"
				}
			includedirs(SOURCE_SDK_INCLUDES)
		end,
	},
	gmod_sdk={
		func = function()
			includedirs{GARRYSMOD_INCLUDES_PATH}	
		end,
	},	
	backwards_headers={
		func = function()
			includedirs{BACKWARDS_HEADERS}
			libdirs{BACKWARDS_HEADERS}
		end,
	},
	steamworks={
		func = function()
		end,
	},
}
function INCLUDES(what)
	local t = include_helpers[what]
	if not t then error("Not found: "..what) end
	local included = t.included
	print("Including "..what,included and "REINCLUDE!?!?" or "")
	t:func(included)
	t.included=true
end
function IsIncluded(what)
	local t = include_helpers[what]
	if not t then error("Not found: "..what) end
	local included = t.included
	return included or false
end

function SOLUTION(name)
	_SOLUTION_NAME=name
	solution(name)
	language("C++")
	location(PROJECT_FOLDER)
	flags{"ExtraWarnings","NoPCH", "StaticRuntime", "EnableSSE2","EnableSSE"}
	configurations
	{ 
		"Release"
	}
	
end

function WINDOWS()
	configuration	("windows")
		defines		{"WIN32","_WIN32","_WINDOWS"}
		linkoptions  { "/nodefaultlib:\"libcmt\"", "/nodefaultlib:\"libcmtd\"" }
end

function LINUX()
	configuration	("not windows")
		defines		{"POSIX","_POSIX","LINUX","_LINUX","GNUC","NO_MALLOC_OVERRIDE"}
		linkoptions	{"-Wl,-z,defs"}
		links		{"rt"}
		libdirs		{SRCDS_DIR..'/bin'}
end


function PROJECT()
	project(_SOLUTION_NAME)
	targetname(_SOLUTION_NAME)
	
	kind	("SharedLib")		
	defines	{
				"GMMODULE",
				--"NO_HOOK_MALLOC",
				--LINUX? "NO_MALLOC_OVERRIDE",
				--"SOURCE_SDK=1",
				"CLIENT_DLL",
				"VERSION_SAFE_STEAM_API_INTERFACES",
				"VECTOR",
				"NO_STRING_T",
				"NO_SDK",
				"COMPILER_MSVC32",
				"_CRT_NONSTDC_NO_DEPRECATE",
				"_CRT_SECURE_NO_DEPRECATE",
				"RAD_TELEMETRY_DISABLED",
				"PROTECTED_THINGS_ENABLE",
				"strncpy=use_Q_strncpy_instead",
				"_snprintf=use_Q_snprintf_instead",
				"fopen=dont_use_fopen",
			}
			
	files	{"src/*.cpp"}

	targetprefix		"gmsv_"
	

	if IsIncluded"backwards_headers" then
		files{BACKWARDS_HEADERS.."/*.cpp"}
	end
	
	configuration 		"windows"
		targetsuffix 	"_win32"
		
	configuration 		"not windows"
		targetsuffix 	"_linux"
		targetextension ".dll"
	
	project(_SOLUTION_NAME)
	
end

function links_static(what)
	linkoptions{"-Wl,-Bstatic,-l"..what..",-Bdynamic"}
end


