Garry's Mod Binaries (Mostly for servers)
=========================================

Common Requirements:
		command line git
		Garry's Mod SRCDS (duh)
		Steamworks SDK
		source-sdk-2013
		TIME
		
Windows requirements:
	Visual Studio 2010
	GnuWin32 ( http://sourceforge.net/projects/getgnuwin32/files/getgnuwin32/ )
		- It is recommended that you get gnuwin32 tools in your PATH or some things may not work
	Game servers also need VS2010 redistributable ( http://www.microsoft.com/en-us/download/details.aspx?id=5555 )

Linux requirements:
	32 bit versions of gcc, etc...
	( apt-get install build-essential gcc g++ gcc-multilib git subversion wget tar bzip2 gzip unzip )

Settings up:
	settings.lua: path to various libraries

Things you may want to execute:
	get_source_sdk.cmd
	dump_lua_shared.cmd
	lua51/get_lua51.cmd



Credits/Copyright: The people appearing in the SVN history and...
	Middle finger to Garry for making us do this in the first hand
	Big thanks to danielga for lua_shared dumping help
	Blackawps for backwars_headers
	thanks to premake for bowing at lua
	Lua developers for awesome language and implementation
	Microsoft for making it hell to compile things
	Valve for awesomely hackable engine
