libcxx_include_files = files(
	'refstring.h',
	'atomic_support.h',
)

libcxx_include_files += files('../../../libcxx/src/include/apple_availability.h')

configure_file(
	input: libcxx_include_files,
	output: 'refstring.h',
	command: [
		cp, '@INPUT@', meson.current_build_dir()
	],
)
