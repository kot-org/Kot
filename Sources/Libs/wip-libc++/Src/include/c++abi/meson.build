# C++ ABI header install meson.build

libcxxabi_header = configure_file(
	input: '../../libcxxabi/include/cxxabi.h',
	output: 'cxxabi.h',
	copy: true
)

libcxxabi_config_header = configure_file(
	input: '../../libcxxabi/include/__cxxabi_config.h',
	output: '__cxxabi_config.h',
	copy: true
)

libcxxabi_header_deps = declare_dependency(
	sources: [libcxxabi_header, libcxxabi_config_header]
)
