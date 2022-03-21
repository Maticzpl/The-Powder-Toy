config_template = files('Config.template.h')
subdir('config')

powder_files += files(
	'PowderMain.cpp',
	'Update.cpp',
	'lua/CommandInterface.cpp',
	'lua/TPTScriptInterface.cpp',
	'lua/TPTSTypes.cpp',
	'lua/LuaEvents.cpp',
)

#render_files += files(
#	'RendererMain.cpp',
#)
#
#font_files += files(
#	'FontMain.cpp',
#)

common_files = files(
	'Format.cpp',
	'Misc.cpp',
	'Probability.cpp',
)

subdir('activities')
subdir('backend')
subdir('bson')
subdir('bzip2')
subdir('common')
subdir('graphics')
subdir('gui')
subdir('json')
subdir('language')
if uopt_lua != 'none'
	subdir('lua')
endif
subdir('network')
subdir('prefs')
subdir('simulation')

powder_files += common_files
render_files += common_files
font_files += common_files

powder_files += pref_files
font_files += pref_files

simulation_elem_defs = []
foreach elem_name_id : simulation_elem_ids
	simulation_elem_defs += 'ELEMENT_DEFINE(' + elem_name_id[0] + ', ' + elem_name_id[1].to_string() + ')'
endforeach
elements_conf_data = configuration_data()
elements_conf_data.set('element_defs', '\n'.join(simulation_elem_defs))
configure_file(
	input: 'simulation/ElementNumbers.template.h',
	output: 'ElementNumbers.h',
	configuration: elements_conf_data
)

simulation_tool_defs = []
foreach tool_name_id : simulation_tool_ids
	simulation_tool_defs += 'TOOL_DEFINE(' + tool_name_id[0] + ')'
endforeach
tools_conf_data = configuration_data()
tools_conf_data.set('tool_defs', '\n'.join(simulation_tool_defs))
configure_file(
	input: 'simulation/ToolNumbers.template.h',
	output: 'ToolNumbers.h',
	configuration: tools_conf_data
)

language_defs = []
foreach language_code : language_codes
	language_defs += 'LANGUAGE_DEFINE(' + language_code + ')'
endforeach
langs_conf_data = configuration_data()
langs_conf_data.set('language_defs', '\n'.join(language_defs))
configure_file(
	input: 'language/Languages.template.h',
	output: 'Languages.h',
	configuration: langs_conf_data
)
