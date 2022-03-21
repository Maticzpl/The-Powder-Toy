language_codes = [
	'en_US',
]
langauge_files = []
foreach language_code : language_codes
	langauge_files += to_array.process(language_code + '.txt', extra_args: 'language_' + language_code)
endforeach

powder_files += langauge_files
font_files += langauge_files
