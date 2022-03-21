simulation_tool_ids = [
	[ 'HEAT' ],
	[ 'COOL' ],
	[ 'AIR'  ],
	[ 'VAC'  ],
	[ 'PGRV' ],
	[ 'NGRV' ],
	[ 'MIX'  ],
	[ 'CYCL' ],
	[ 'AMBM' ],
	[ 'AMBP' ],
	[ 'WIND' ],
	[ 'ADD'  ],
	[ 'SUB'  ],
	[ 'MUL'  ],
	[ 'DIV'  ],
	[ 'SMDG' ],
	[ 'CLR'  ],
	[ 'SET'  ],
	[ 'PKDC' ],
]

simulation_tool_src = []
foreach tool_name_id : simulation_tool_ids
	simulation_tool_src += tool_name_id[0] + '.cpp'
endforeach
simulation_files += files(simulation_tool_src)
