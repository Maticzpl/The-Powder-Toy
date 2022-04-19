powder_files += files(
	'Request.cpp',
)

if not uopt_http
	powder_files += files(
		'NullRequestManager.cpp',
	)
else
	powder_files += files(
		'CurlRequestManager.cpp',
	)
endif
