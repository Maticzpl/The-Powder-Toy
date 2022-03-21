luaconsole_files = files(
	'LegacyLuaAPI.cpp',
	'LuaBit.cpp',
	'LuaCompat.c',
	'LuaScriptInterface.cpp',
	'LuaSmartRef.cpp',
)
if uopt_http
	luaconsole_files += files(
		'LuaTCPSocket.cpp',
	)
endif

subdir('luascripts')

powder_files += luaconsole_files
