# C++ header install meson.build

mkdir = find_program('mkdir')
cp = find_program('cp')
cat = find_program('cat')

### Generate and Install Config Header ###

subdir('include')
subdir('support')

config_site_header = configure_file(
	input: '__libcxx_config_site.in',
	output: '__libcpp_config_site',
	configuration: libcxx_conf_data,
)

config_header = configure_file(
	input: '__config.in',
	output: '__config_base',
	copy: true
)

libcpp_config_headers = [config_site_header, config_header]

# This step composes the two headers above
generate_libpp_config_header = custom_target(
  'generate_libpp_config_header',
  command: [ cat, '@INPUT@' ],
  capture: true,
  input: libcpp_config_headers,
  output: '__config',
)

### Handle Threading Headers ###

external_threading_header_dep = []

if thread_library == 'ea-framework'
	framework_external_threading_header = configure_file(
		input: 'external_threading/__external_threading_framework',
		output: '__external_threading',
		copy: true
	)
	external_threading_header_dep += framework_external_threading_header
elif thread_library == 'freertos'

	freertos_external_threading_header = configure_file(
		input: 'external_threading/__external_threading_freertos',
		output: '__external_threading',
		copy: true
	)

elif thread_library == 'threadx'

	threadx_external_threading_header = configure_file(
		input: 'external_threading/__external_threading_threadx',
		output: '__external_threading',
		copy: true
	)
endif

### Install C++ Headers ###

libcpp_install_headers = files(
	'../../libcxx/include/__bit_reference',
	'__baremetal_locale_fallbacks.hpp',
	'../../libcxx/include/__bsd_locale_defaults.h',
	'../../libcxx/include/__bsd_locale_fallbacks.h',
	'../../libcxx/include/__errc',
	'../../libcxx/include/__debug',
	'../../libcxx/include/__functional_03',
	'../../libcxx/include/__functional_base',
	'../../libcxx/include/__functional_base_03',
	'../../libcxx/include/__hash_table',
	'../../libcxx/include/__libcpp_version',
	'__locale',
	'../../libcxx/include/__mutex_base',
	'../../libcxx/include/__node_handle',
	'../../libcxx/include/__nullptr',
	'../../libcxx/include/__split_buffer',
	'../../libcxx/include/__sso_allocator',
	'../../libcxx/include/__std_stream',
	'../../libcxx/include/__string',
	'../../libcxx/include/__threading_support',
	'../../libcxx/include/__tree',
	'../../libcxx/include/__tuple',
	'../../libcxx/include/__undef_macros',
	'../../libcxx/include/algorithm',
	'../../libcxx/include/any',
	'../../libcxx/include/array',
	'../../libcxx/include/atomic',
	'../../libcxx/include/bit',
	'../../libcxx/include/bitset',
	'../../libcxx/include/cassert',
	'../../libcxx/include/ccomplex',
	'../../libcxx/include/cctype',
	'../../libcxx/include/cerrno',
	'../../libcxx/include/cfenv',
	'../../libcxx/include/cfloat',
	'../../libcxx/include/charconv',
	'../../libcxx/include/chrono',
	'../../libcxx/include/cinttypes',
	'../../libcxx/include/ciso646',
	'../../libcxx/include/climits',
	'../../libcxx/include/clocale',
	'../../libcxx/include/cmath',
	'../../libcxx/include/codecvt',
	'../../libcxx/include/compare',
	'../../libcxx/include/complex',
	'../../libcxx/include/complex.h',
	'../../libcxx/include/condition_variable',
	'../../libcxx/include/csetjmp',
	'../../libcxx/include/csignal',
	'../../libcxx/include/cstdarg',
	'../../libcxx/include/cstdbool',
	'../../libcxx/include/cstddef',
	'../../libcxx/include/cstdint',
	'../../libcxx/include/cstdio',
	'../../libcxx/include/cstdlib',
	'../../libcxx/include/cstring',
	'../../libcxx/include/ctgmath',
	'../../libcxx/include/ctime',
	'../../libcxx/include/ctype.h',
	'../../libcxx/include/cwchar',
	'../../libcxx/include/cwctype',
	'../../libcxx/include/deque',
	'../../libcxx/include/errno.h',
	'../../libcxx/include/exception',
	'../../libcxx/include/fenv.h',
	'../../libcxx/include/filesystem',
	'../../libcxx/include/float.h',
	'../../libcxx/include/forward_list',
	'../../libcxx/include/fstream',
	'../../libcxx/include/functional',
	'../../libcxx/include/future',
	'../../libcxx/include/initializer_list',
	'../../libcxx/include/inttypes.h',
	'../../libcxx/include/iomanip',
	'../../libcxx/include/ios',
	'../../libcxx/include/iosfwd',
	'../../libcxx/include/iostream',
	'../../libcxx/include/istream',
	'../../libcxx/include/iterator',
	'../../libcxx/include/limits',
	'../../libcxx/include/limits.h',
	'../../libcxx/include/list',
	'locale',
	'../../libcxx/include/locale.h',
	'../../libcxx/include/map',
	'../../libcxx/include/math.h',
	'../../libcxx/include/memory',
	'../../libcxx/include/mutex',
	'../../libcxx/include/new',
	'../../libcxx/include/numeric',
	'../../libcxx/include/optional',
	'../../libcxx/include/ostream',
	'../../libcxx/include/queue',
	'../../libcxx/include/random',
	'../../libcxx/include/ratio',
	'../../libcxx/include/regex',
	'../../libcxx/include/scoped_allocator',
	'../../libcxx/include/set',
	'../../libcxx/include/setjmp.h',
	'../../libcxx/include/shared_mutex',
	'../../libcxx/include/span',
	'../../libcxx/include/sstream',
	'../../libcxx/include/stack',
	'../../libcxx/include/stdbool.h',
	'../../libcxx/include/stddef.h',
	'../../libcxx/include/stdexcept',
	'../../libcxx/include/stdint.h',
	'../../libcxx/include/stdio.h',
	'../../libcxx/include/stdlib.h',
	'../../libcxx/include/streambuf',
	'../../libcxx/include/string',
	'../../libcxx/include/string.h',
	'../../libcxx/include/string_view',
	'../../libcxx/include/strstream',
	'../../libcxx/include/system_error',
	'../../libcxx/include/tgmath.h',
	'../../libcxx/include/thread',
	'../../libcxx/include/tuple',
	'../../libcxx/include/type_traits',
	'../../libcxx/include/typeindex',
	'../../libcxx/include/typeinfo',
	'../../libcxx/include/unordered_map',
	'../../libcxx/include/unordered_set',
	'../../libcxx/include/utility',
	'../../libcxx/include/valarray',
	'../../libcxx/include/variant',
	'../../libcxx/include/vector',
	'../../libcxx/include/version',
	'../../libcxx/include/wchar.h',
	'../../libcxx/include/wctype.h',
)

libcpp_install_experimental_headers = files(
	'../../libcxx/include/experimental/__config',
	'../../libcxx/include/experimental/__memory',
	'../../libcxx/include/experimental/algorithm',
	'../../libcxx/include/experimental/coroutine',
	'../../libcxx/include/experimental/deque',
	'../../libcxx/include/experimental/filesystem',
	'../../libcxx/include/experimental/forward_list',
	'../../libcxx/include/experimental/functional',
	'../../libcxx/include/experimental/iterator',
	'../../libcxx/include/experimental/list',
	'../../libcxx/include/experimental/map',
	'../../libcxx/include/experimental/memory_resource',
	'../../libcxx/include/experimental/propagate_const',
	'../../libcxx/include/experimental/regex',
	'../../libcxx/include/experimental/set',
	'../../libcxx/include/experimental/simd',
	'../../libcxx/include/experimental/string',
	'../../libcxx/include/experimental/type_traits',
	'../../libcxx/include/experimental/unordered_map',
	'../../libcxx/include/experimental/unordered_set',
	'../../libcxx/include/experimental/utility',
	'../../libcxx/include/experimental/vector',
)

libcpp_install_ext_headers = files(
	'../../libcxx/include/ext/__hash',
	'../../libcxx/include/ext/hash_map',
	'../../libcxx/include/ext/hash_set',
)

install_cpp_headers = custom_target('install-cpp-headers',
	input: libcpp_install_headers,
	output: '__baremetal_locale_fallbacks.hpp',
	command: [
		cp, '@INPUT@', meson.current_build_dir()
	],
)

install_cpp_experimental_headers = custom_target('install-cpp-experimental-headers',
	input: libcpp_install_experimental_headers,
	output: 'experimental',
	command: [
		mkdir, '-p', meson.current_build_dir() + '/' + 'experimental', '&&',
		cp, '@INPUT@', meson.current_build_dir() + '/' + 'experimental'
	],
)

install_cpp_ext_headers = custom_target('install-cpp-ext-headers',
	input: libcpp_install_ext_headers,
	output: 'ext',
	command: [
		mkdir, '-p', meson.current_build_dir() + '/' + 'ext', '&&',
		cp, '@INPUT@', meson.current_build_dir() + '/' + 'ext'
	],
)

install_cpp_headers_dep = declare_dependency(
	sources: [
		generate_libpp_config_header[0],
		install_cpp_ext_headers[0],
		install_cpp_experimental_headers[0],
		install_cpp_headers[0]
	]
)
