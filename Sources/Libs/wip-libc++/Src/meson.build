project('Embedded Artistry libcpp',
	['cpp'],
	default_options : [
		'warning_level=3',
		'werror=false',
		# `build.*` options affect `native: true targets`
		# plain options affect `native: false` targets.
		'cpp_std=c++17', 'build.cpp_std=c++17',
		# This project defaults to a release build
		'debug=false',
		'optimization=2',
	],
	license: 'MIT',
	meson_version: '>=0.52.0')

#######################
# Read Build Settings #
#######################

disable_rtti = get_option('disable-rtti')
disable_exceptions = get_option('disable-exceptions')
use_compiler_rt = get_option('libcxx-use-compiler-rt')
enable_pedantic_error = get_option('enable-pedantic-error')
use_llvm_libunwind = get_option('libcxx-use-llvm-libunwind')
enable_threads = get_option('enable-threading')
has_external_threading = get_option('libcxx-has-external-thread-api')
build_external_threading = get_option('libcxx-build-external-thread-api')
enable_filesystem = get_option('libcxx-enable-filesystem')
enable_stdinout = get_option('libcxx-enable-stdinout')
default_newdelete = get_option('libcxx-default-newdelete')
enable_monotonic_clock = get_option('libcxx-monotonic-clock')
force_32_bit = get_option('force-32-bit')
enable_chrono = get_option('libcxx-enable-chrono')
thread_library = get_option('libcxx-thread-library')
use_external_libc = get_option('use-libc-subproject')

# The default terminate handler attempts to demangle uncaught exceptions, which
# causes extra I/O and demangling code to be pulled in.
silent_terminate = get_option('libcxx-silent-terminate')

#####################################
# If Darwin, determine the SDK path #
#####################################

if build_machine.system() == 'darwin'
	darwin_sdk_path = run_command('xcrun', '--show-sdk-path').stdout().strip()
	darwin_sdk_include_arg = '-idirafter' + darwin_sdk_path + '/usr/include/'
	message('Using Darwin SDK path: ' + darwin_sdk_path)
endif

##################
# Compiler Flags #
##################

host_os = host_machine.system()
build_os = build_machine.system()

# Pick up our common compiler variables + desired_*_flags variables
subdir('build/compiler')
subdir('build/compiler/cpp')

if meson.is_cross_build()
	host_cpp_compiler = meson.get_compiler('cpp', native: false)
	if host_cpp_compiler.get_id() == 'gcc' and host_cpp_compiler.version().version_compare('<9.0') and target_architecture == 'arm'
		error('This project requires gcc-arm-none-eabi v9.0 or later. See README.md for more information.')
	endif
endif

if get_option('enable-pedantic')
	desired_common_compile_flags += '-pedantic'
else
	desired_common_compile_flags += '-Wno-pedantic'
endif

if get_option('enable-pedantic-error')
	desired_common_compile_flags += '-pedantic-error'
endif

desired_common_compile_flags += [
	'-W',
	'-Wwrite-strings',
	'-Wno-unused-member-function',
	'-Wno-useless-cast',
	'-Wno-weak-vtables',
	'-Wno-missing-prototypes',
	'-Wno-class-varargs',
	'-Wno-unused-template',
	'-Wno-zero-as-null-pointer-constant',
	'-Wno-reserved-id-macro',
	'-Wno-deprecated-dynamic-exception-spec',
	'-Wno-comma',
	'-Wno-old-style-cast',
	# These are disabled because I don't want libc++ warnings when we turn
	# on extra warnings for the framework
	'-Werror=return-type',
	'-Wno-unused-parameter',
	'-Wno-long-long',
	'-Wno-switch-default',
	'-Wno-switch-enum',
	'-Wno-inline',
	'-Wno-effc++',
	'-Wno-cast-align',
]

libcxxabi_compile_flags = [
	'-Wmismatched-tags',
	'-Wnewline-eof',
	'-Wshorten-64-to-32',
	'-Wconversion',
	'-Wchar-subscripts',
	'-Wmissing-braces',
	'-Wshadow',
	'-Wsign-compare',
	'-Wsign-conversion',
	'-Wstrict-aliasing=2',
	'-Wstrict-overflow=4',
	'-Wunused-variable',
	'-Wundef',
	# These are disabled because I don't want libc++ warnings when we turn
	# on extra warnings for the framework
	'-Wno-switch',
	'-Wno-missing-declarations',
]

libcxx_compile_flags = [
	'-Wno-literal-suffix',
	'-Wno-c++14-compat',
	'-Wno-noexcept-type',
	'-Wno-user-defined-literals',
	'-Wno-covered-switch-default',
	# These are disabled because I don't want libc++ warnings when we turn
	# on extra warnings for the framework
	'-Wno-sign-conversion',
	'-Wno-conversion',
	'-Wno-double-promotion',
]

compile_settings_list = [
	{'lang': 'cpp', 'compiler': host_cpp_compiler, 'flags': desired_cpp_compile_flags, 'isnative': false},
	{'lang': 'cpp', 'compiler': native_cpp_compiler, 'flags': desired_native_cpp_compile_flags, 'isnative': true},
]

# Process the compilation flags
subdir('build/compiler/check-and-apply-flags')
libcxxabi_host_compile_flags = host_cpp_compiler.get_supported_arguments(libcxxabi_compile_flags)
libcxxabi_native_compile_flags = native_cpp_compiler.get_supported_arguments(libcxxabi_compile_flags)
libcxx_host_compile_flags = host_cpp_compiler.get_supported_arguments(libcxx_compile_flags)
libcxx_native_compile_flags = native_cpp_compiler.get_supported_arguments(libcxx_compile_flags)

## Add Additional Flags after processing

# Reset the variables for new common flags
libcxxabi_compile_flags = [
	# Let C++ library know we are building libcxxabi
	'-D_LIBCXXABI_BUILDING_LIBRARY',
	# Bring back `std::unexpected`, which is removed in C++17, to support
	# pre-C++17.
	'-D_LIBCPP_ENABLE_CXX17_REMOVED_UNEXPECTED_FUNCTIONS',
	# ABI needs GNU functions to compile/link
	'-D_GNU_SOURCE',
]

libcxx_compile_flags = [
	'-fvisibility-inlines-hidden',
	'-fvisibility=hidden',
	'-D_LIBCPP_BUILD_STATIC',
	'-D_LIBCPP_BUILDING_LIBRARY',
	'-D_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER',
	'-DLIBCXX_BUILDING_LIBCXXABI',
	# Define we don't touch libcpp debug settings,
	'-D_LIBCPP_DEBUG=0',
	# STL needs GNU functions to compile/link
	'-D_GNU_SOURCE',
]

if get_option('debug') == true
	libcxx_compile_flags += '-D_DEBUG'
else
	libcxx_compile_flags += '-D_LIBCPP_DISABLE_ADDITIONAL_DIAGNOSTICS'
endif

if host_os == 'darwin'
	libcxx_host_compile_flags += ['-U__STRICT_ANSI__']
endif

if build_os == 'darwin'
	libcxx_native_compile_flags += ['-U__STRICT_ANSI__']
endif

common_link_flags = [
	'-nodefaultlibs',
]

libcxxabi_link_flags = [
	common_link_flags,
	'-lunwind',
]
libcxx_link_flags = [
	common_link_flags,
]

libcxx_host_dep_compiler_flags = []
libcxx_host_dep_link_flags = common_link_flags
libcxx_native_dep_compiler_flags = []
libcxx_native_dep_link_flags = common_link_flags

libcxx_dep_compile_flags = [
	'-nostdinc++',
	'-D_GNU_SOURCE',
]

######################
# Handle Linux Build #
######################
# Linux requires special headers... and doesn't work with -nostdinc++

if build_machine.system() == 'linux'
	libcxx_native_compile_flags += ['-idirafter/usr/include']
	libcxx_native_dep_compiler_flags += ['-idirafter/usr/include']
	libcxxabi_native_compile_flags += ['-idirafter/usr/include']
endif

if host_machine.system() == 'linux'
	libcxx_host_compile_flags += ['-idirafter/usr/include']
	libcxx_host_dep_compiler_flags += ['-idirafter/usr/include']
	libcxxabi_host_compile_flags += ['-idirafter/usr/include']
endif

##########################
# Set Default File Lists #
##########################

libcxxabi_files = [
	files(
		'libcxxabi/src/cxa_aux_runtime.cpp',
		'libcxxabi/src/cxa_default_handlers.cpp',
		'src/c++abi/cxa_handlers.cpp',
		'libcxxabi/src/cxa_unexpected.cpp',
		'libcxxabi/src/cxa_vector.cpp',
		'libcxxabi/src/cxa_virtual.cpp',
		'libcxxabi/src/fallback_malloc.cpp',
		'libcxxabi/src/private_typeinfo.cpp',
		'libcxxabi/src/stdlib_exception.cpp',
		'libcxxabi/src/stdlib_typeinfo.cpp',
	)
]
libcxxabi_native_files = []

libcpp_core_files = [
	files(
		'libcxx/src/algorithm.cpp',
		'libcxx/src/any.cpp',
		'libcxx/src/bind.cpp',
		'libcxx/src/charconv.cpp',
		'libcxx/src/condition_variable.cpp',
		'libcxx/src/condition_variable_destructor.cpp',
		'libcxx/src/exception.cpp',
		'libcxx/src/functional.cpp',
		'libcxx/src/future.cpp',
		'libcxx/src/hash.cpp',
		'libcxx/src/ios.cpp',
		'libcxx/src/memory.cpp',
		'libcxx/src/mutex.cpp',
		'libcxx/src/mutex_destructor.cpp',
		'src/c++/new_terminate_badalloc.cpp',
		'libcxx/src/optional.cpp',
		'libcxx/src/regex.cpp',
		'libcxx/src/shared_mutex.cpp',
		'libcxx/src/strstream.cpp',
		'libcxx/src/system_error.cpp',
		'libcxx/src/typeinfo.cpp',
		'libcxx/src/utility.cpp',
		'libcxx/src/valarray.cpp',
		'libcxx/src/variant.cpp',
		'libcxx/src/vector.cpp',
	)
]

if enable_chrono == true
	if use_external_libc
		libcpp_core_files += files('src/c++/chrono.cpp')
	else
		libcpp_core_files += files('libcxx/src/chrono.cpp')
	endif
endif

libcpp_experimental_files = files('libcxx/src/experimental/memory_resource.cpp')

libcpp_filesystem_files = [
	files(
		'libcxx/src/filesystem/directory_iterator.cpp',
		'libcxx/src/filesystem/operations.cpp'
	)
]

libcxxabi_include_directories = [
	include_directories('include/c++abi/', is_system: true),
	include_directories('include/c++/', is_system: true)
]
libcxx_extensions_include_dir = include_directories('extensions', is_system: true)
libcxx_include_directories = [
	include_directories('include/c++abi', is_system: true),
	include_directories('include/c++', is_system: true),
	libcxx_extensions_include_dir
]

# Variables to hold target dependencies that are triggered by build options
libcxxabi_host_dependencies = []
libcxxabi_native_dependencies = []
libcxx_host_dependencies = []
libcxx_native_dependencies = []

using_pthread = false

#################################
# Initialize Configuration Data #
#################################

libcxx_conf_data = configuration_data()

# ABI version of libc++. Can be either 1 or 2, where 2 is currently not stable. Defaults to 1.
libcxx_conf_data.set('_LIBCPP_ABI_VERSION', '1')

# The inline ABI namespace used by libc++. It defaults to __n where `n` is the current ABI version.
libcxx_conf_data.set('_LIBCPP_ABI_NAMESPACE', '__1')

########################
# External libc Option #
########################

# NOTE: External libc dependencies are only used for building the library.
# They are not forwarded through dependencies. You are expected to handle
# That with the rest of your program.

libc_dep = []
libc_native_dep = []

if use_external_libc
	message('Using external libc specified in libc-subproject option.')
	message('If you are using Embedded Artistry libc, note that iostreams and locale are not supported.')
	libc_array = get_option('libc-subproject')
	libc_subproject = subproject(libc_array[0])

	libc_dep = libc_subproject.get_variable(libc_array[1])
	# Only pull in include directories + compile arguments
	libc_dep = libc_dep.partial_dependency(includes: true, compile_args: true)
	if(host_machine.system() == 'darwin')
			# -nostdinc will mess with availabilty.h and a few other headers, so we
			# reference local includes
			message('Using local headers for Apple dependencies.')
			libc_dep = [
				libc_dep,
				declare_dependency(
					include_directories: include_directories('include/apple', is_system: true),
				)
			]
		endif

	if libc_array.length() > 2
		libc_native_dep = libc_subproject.get_variable(libc_array[2])
		libc_native_dep = libc_native_dep.partial_dependency(includes: true, compile_args: true)
		if(build_machine.system() == 'darwin')
			# -nostdinc will mess with availabilty.h and a few other headers, so we
			# reference local includes
			message('Using local headers for Apple dependencies.')
			libc_native_dep = [
				libc_native_dep,
				declare_dependency(
					include_directories: include_directories('include/apple', is_system: true),
				)
			]
		endif
	endif

	# Libcxxabi Settings
	libcxxabi_compile_flags += ['-D_POSIX_MEMALIGN_VISIBLE', '-DLIBCXXABI_USE_LLVM_UNWINDER']
	libcxxabi_include_directories += include_directories('embedded-unwind/include', is_system: true)
	libcxxabi_files += files(
		'src/c++abi/abort_message.cpp',
		'src/c++abi/cxa_guard.cpp',
		'src/c++abi/stdlib_stdexcept.cpp',
	)

	if disable_rtti == false
		libcxxabi_files += files('src/c++abi/cxa_demangle.cpp')
	endif

	# TODO: build Unwind-sjlj_pthread.c
	if host_machine.cpu_family() == 'arm'
		libcxxabi_host_compile_flags += ['-D__USING_SJLJ_EXCEPTIONS__']
	endif

	## Libcxx Settings
	libcxx_compile_flags += ['-D_POSIX_MEMALIGN_VISIBLE']
	libcpp_core_files += files(
		'src/c++/debug.cpp',
		'src/c++/locale.cpp',
		'src/c++/string.cpp',
		'src/c++/random.cpp',
		'src/c++/stdexcept.cpp',
	)
	# Enable "baremetal" fallbacks when working with an alternate libc
	# TODO: better name for this argument - could be split out even?
	libcxx_conf_data.set('_BAREMETAL', true)
else
	message('Compiling with the built-in libc and system headers')

	libcxxabi_compile_flags += '-nostdinc++'
	libcxxabi_files += files(
		'libcxxabi/src/abort_message.cpp',
		'libcxxabi/src/stdlib_stdexcept.cpp',
		'libcxxabi/src/cxa_guard.cpp',
	)

	if disable_rtti == false
		libcxxabi_files += files(
			'libcxxabi/src/cxa_demangle.cpp',
		)
	endif

	libcxx_compile_flags += '-nostdinc++'
	libcpp_core_files += files(
		'libcxx/src/debug.cpp',
		'libcxx/src/iostream.cpp',
		'libcxx/src/locale.cpp',
		'libcxx/src/random.cpp',
		'libcxx/src/string.cpp',
		'libcxx/src/stdexcept.cpp',
	)
endif

################################
# Apply Project Option Changes #
################################

### Exceptions
if disable_exceptions == true
	message('Exceptions are disabled')

	libcxxabi_files += files(
		'libcxxabi/src/cxa_noexception.cpp',
	)
	libcxxabi_compile_flags += ['-D_LIBCXXABI_NO_EXCEPTIONS', '-fno-exceptions']

	libcxx_compile_flags += ['-fno-exceptions']
	libcxx_conf_data.set('_LIBCPP_NO_EXCEPTIONS', true)
else
	message('Exceptions are enabled')

	libcxxabi_compile_flags += ['-funwind-tables']
	libcxxabi_files += files(
		'libcxxabi/src/cxa_exception.cpp',
		# We always use our CXA Personality because the standard one doesn't support GCC
		'src/c++abi/cxa_personality.cpp',
	)

	libcxxabi_files += files('libcxxabi/src/cxa_exception_storage.cpp',)
endif

### RTTI
if disable_rtti == true
	message('RTTI is Disabled')
	libcxx_compile_flags += ['-fno-rtti']
	libcxx_conf_data.set('_LIBCPP_NO_RTTI', true)
endif

### STDIO
if enable_stdinout == false
	libcxx_conf_data.set('_LIBCPP_HAS_NO_STDIN', true)
endif

### Filesystem
if enable_filesystem == false
	libcxx_conf_data.set('_LIBCPP_HAS_NO_GLOBAL_FILESYSTEM_NAMESPACE', true)
endif

### Compiler-rt
if use_compiler_rt == true
	libcxxabi_link_flags += '-rtlib=compiler-rt'
	libcxx_link_flags += '-rtlib=compiler-rt'
else
	# Filesystem uses __int128_t, which requires a definition of __muloi4 when
	# compiled with UBSAN. This definition is not provided by libgcc_s, but is
	# provided by compiler-rt. So we need to disable it to avoid having multiple
	# definitions. See filesystem/int128_builtins.cpp.
	libcpp_filesystem_files += files('libcxx/src/filesystem/int128_builtins.cpp')
endif

### Unwind
if use_llvm_libunwind == true
	libcxxabi_compile_flags += '-DLIBCXXABI_USE_LLVM_UNWINDER'
endif

### Terminate
# The default terminate handler attempts to demangle uncaught exceptions, which
# causes extra I/O and demangling code to be pulled in.
if silent_terminate == true
	libcxxabi_compile_flags += '-DLIBCXXABI_SILENT_TERMINATE'
else
	assert(disable_rtti == false, 'Loud terminate requires RTTI')
endif

### New/Delete
if default_newdelete == true
	libcxxabi_files += files('libcxxabi/src/stdlib_new_delete.cpp')
else
	libcxx_compile_flags += '-D_LIBCPP_DISABLE_NEW_DELETE_DEFINITIONS'
endif

### Threading Support
if enable_threads == true
	assert(not ((thread_library == '') or (thread_library == 'none')),
		'Supply thread library if enable_threads is used')

	if thread_library == 'pthread'
		message('Using pthread dependency for threading support')
		using_pthread = true
	elif thread_library == 'ea-framework'
		# If we aren't using pthreads, use an external header
		message('Building with framework thread library support')

		libcxx_conf_data.set('_LIBCPP_HAS_THREAD_API_EXTERNAL', true)
		libcpp_core_files += files('src/c++/thread.cpp')
		using_pthread = true
	else
		# If we aren't using pthreads or framework, use an external header
		assert(not (has_external_threading == true and build_external_threading == true),
		'Cannot select both has_external_threading and build_external_threading. Pick one.')

		message('Enabling custom external threading support.')
		if build_external_threading == true
			libcxx_conf_data.set('_LIBCPP_HAS_THREAD_LIBRARY_EXTERNAL', true)
		else
			libcxx_conf_data.set('_LIBCPP_HAS_THREAD_API_EXTERNAL', true)
			libcpp_core_files += files('src/c++/thread.cpp')
		endif

		# TODO: test - does the new logic break in this case, and should we reactivate this?
		#libcxxabi_files += files('src/c++abi/cxa_guard.cpp')
	endif

	if host_os != 'darwin' and host_os != 'windows'
		message('enabling threads')
		libcxxabi_files += files(
			'libcxxabi/src/cxa_thread_atexit.cpp',
		)
	endif

	if build_os != 'darwin' and build_os != 'windows'
		libcxxabi_native_files += files(
			'libcxxabi/src/cxa_thread_atexit.cpp',
		)
	endif
else
	message('Building without threading support.')
	libcxx_conf_data.set('_LIBCPP_HAS_NO_THREADS', true)
	libcxxabi_compile_flags += '-D_LIBCXXABI_HAS_NO_THREADS'
endif

# Handle pthread-specific logic
if using_pthread == true
	libthread = dependency('threads', native: false, required: false)
	libthread_native = dependency('threads', native: true, required: true)

	# First, we'll a sanity check to make sure -pthread is actually supported!
	if libthread.found()
		if(host_cpp_compiler.has_argument('-pthread') == false)
			message('Meson says the compiler supports pthread, but it does not. Disabling pthread dependency')
			libthread = disabler()
		endif
	endif

	if libthread.found()
		if host_machine.system() == 'darwin'
			libcxxabi_host_compile_flags += darwin_sdk_include_arg
			libcxx_host_compile_flags += darwin_sdk_include_arg
			libcxx_host_dep_compiler_flags += darwin_sdk_include_arg
		endif

		if thread_library == 'pthread'
			libcxx_conf_data.set('_LIBCPP_HAS_THREAD_API_PTHREAD', true)
			libcxxabi_host_dependencies += libthread
			libcxx_host_dependencies += libthread
			libcpp_core_files += files('src/c++/thread.cpp')
		elif thread_library == 'ea-framework'
			message('Enabling pthread support for the host machine C++ library')
			libcxxabi_host_compile_flags += '-D_LIBCPP_FRAMEWORK_FORCE_PTHREAD'
			libcxx_host_compile_flags += '-D_LIBCPP_FRAMEWORK_FORCE_PTHREAD'
			libcxx_host_dep_compiler_flags += '-D_LIBCPP_FRAMEWORK_FORCE_PTHREAD'
		else
			error('Unexpected thread configuration')
		endif
	elif thread_library == 'pthread'
		error('pthread selected as threading library, but it is not available for the host machine.')
	endif

	if libthread_native.found()
		if build_machine.system() == 'darwin'
			libcxxabi_native_compile_flags += darwin_sdk_include_arg
			libcxx_native_compile_flags += darwin_sdk_include_arg
			libcxx_native_dep_compiler_flags += darwin_sdk_include_arg
		endif

		libcxxabi_native_dependencies += libthread_native
		libcxx_native_dependencies += libthread_native

		if thread_library == 'ea-framework'
			message('Enabling pthread support for the native C++ library')
			libcxxabi_native_compile_flags += '-D_LIBCPP_FRAMEWORK_FORCE_PTHREAD'
			libcxx_native_compile_flags += '-D_LIBCPP_FRAMEWORK_FORCE_PTHREAD'
			libcxx_native_dep_compiler_flags += '-D_LIBCPP_FRAMEWORK_FORCE_PTHREAD'
		endif
	endif
endif

#### Monotonic Clock
# monotonic clock can only be disabled if there is no threading
if enable_monotonic_clock == false and enable_threads == false
	libcxx_conf_data.set('_LIBCPP_HAS_NO_MONOTONIC_CLOCK', true)
endif

### 32-bit
if force_32_bit == true
	libcxx_compile_flags += '-m32'
	libcxxabi_compile_flags += '-m32'
endif

### Atexit Handling
if host_cpp_compiler.has_function('__cxa_thread_atexit_impl')
	libcxx_host_compile_flags += ['-DHAVE___CXA_THREAD_ATEXIT_IMPL']
elif host_machine.system() == 'darwin'
	# On OS X, clang doesn't handle weak linking the way we expect
	# So we'll work around it by undefining the weak symbol
	libcxx_host_dep_link_flags += '-Wl,-U,___cxa_thread_atexit_impl'
endif

if native_cpp_compiler.has_function('__cxa_thread_atexit_impl')
	libcxx_native_compile_flags += ['-DHAVE___CXA_THREAD_ATEXIT_IMPL']
elif build_machine.system() == 'darwin'
	# On OS X, clang doesn't handle weak linking the way we expect
	# So we'll work around it by undefining the weak symbol
	libcxx_native_dep_link_flags += '-Wl,-U,___cxa_thread_atexit_impl'
endif

### Cross-compilation Specific

if meson.is_cross_build()
	libcxxabi_host_compile_flags += ['-DLIBCXXABI_BAREMETAL', '-ffreestanding']
	libcxx_host_compile_flags += ['-ffreestanding']
endif

###################
# Install Headers #
###################

# This must be after all the config_data is set
# This is the source of install_cpp_headers_dep
subdir('include')

#############################
# libcxxabi Library targets #
#############################

# Note that due to the behavior of #include_next and -isystem, you need to ensure that
# the C++ includes are _last_ to be added to a target so that they are first in the search list.
# This is a helper variable that is used to control include directory order.
libcxxabi_include_dep = declare_dependency(
	include_directories: libcxxabi_include_directories,
)

libcxxabi = static_library('c++abi',
    libcxxabi_files,
    cpp_args: libcxxabi_compile_flags + libcxxabi_host_compile_flags,
    link_args: libcxxabi_link_flags,
    dependencies: [
    	install_cpp_headers_dep,
    	libcxxabi_host_dependencies,
    	libcxxabi_header_deps,
    	# libc dep must come bfore libcxxabi_include_dep
    	# for proper include directory ordering
    	libc_dep,
    	libcxxabi_include_dep,
    ],
    build_by_default: meson.is_subproject() == false,
)

libcxxabi_native = static_library('c++abi_native',
    libcxxabi_files + libcxxabi_native_files,
    cpp_args: libcxxabi_compile_flags + libcxxabi_native_compile_flags,
    link_args: libcxxabi_link_flags,
    dependencies: [
    	install_cpp_headers_dep,
    	libcxxabi_native_dependencies,
    	libcxxabi_header_deps,
    	# libc dep must come bfore libcxxabi_include_dep
    	# for proper include directory ordering
    	libc_native_dep,
    	libcxxabi_include_dep,
    ],
    native: true,
    build_by_default: meson.is_subproject() == false,
)

##########################
# libcxxabi Dependencies #
##########################

libcxxabi_dep = declare_dependency(
	include_directories: [
		include_directories('include/c++abi', is_system: true),
	],
	link_with: libcxxabi,
	link_args: libcxx_host_dep_link_flags,
	dependencies: libcxxabi_host_dependencies
)

libcxxabi_native_dep = declare_dependency(
	include_directories: [
		include_directories('include/c++abi', is_system: true),
	],
	link_with: libcxxabi_native,
	link_args: libcxx_native_dep_link_flags,
	dependencies: libcxxabi_native_dependencies
)

##########################
# libcxx Library Targets #
##########################

# Note that due to the behavior of #include_next and -isystem, you need to ensure that
# the C++ includes are _last_ to be added to a target so that they are first in the search list.
# This is a helper variable that is used to control include directory order.
libcxx_include_dep = declare_dependency(
	include_directories: libcxx_include_directories,
)

libcpp = static_library('c++',
    libcpp_core_files,
    cpp_args: libcxx_compile_flags + libcxx_host_compile_flags,
    link_args: libcxx_link_flags,
    dependencies: [
	    install_cpp_headers_dep,
	    libcxx_host_dependencies,
	    libc_dep,
	    libcxx_include_dep
    ],
    build_by_default: meson.is_subproject() == false,
)

libcpp_native = static_library('c++_native',
    libcpp_core_files,
    cpp_args: libcxx_compile_flags + libcxx_native_compile_flags,
    link_args: libcxx_link_flags,
    dependencies: [
    	install_cpp_headers_dep,
    	libcxx_native_dependencies,
    	libc_native_dep,
    	libcxx_include_dep
    ],
    native: true,
    build_by_default: meson.is_subproject() == false,
)

libcpp_experimental = static_library('c++experimental',
    libcpp_experimental_files,
    cpp_args: libcxx_compile_flags + libcxx_host_compile_flags,
    link_args: libcxx_link_flags,
    dependencies: [
    	install_cpp_headers_dep,
    	libcxx_host_dependencies,
    	libc_dep,
    	libcxx_include_dep
    ],
    build_by_default: meson.is_subproject() == false,
)

libcpp_experimental_native = static_library('c++experimental_native',
    libcpp_experimental_files,
    cpp_args: libcxx_compile_flags + libcxx_native_compile_flags,
    link_args: libcxx_link_flags,
    dependencies: [
    	install_cpp_headers_dep,
    	libcxx_native_dependencies,
    	libc_native_dep,
    	libcxx_include_dep
    ],
    native: true,
    build_by_default: meson.is_subproject() == false,
)

if enable_filesystem == true
	libcpp_fs = static_library('c++fs',
		libcpp_filesystem_files,
		cpp_args: libcxx_compile_flags + libcxx_host_compile_flags,
		link_args: libcxx_link_flags,
		dependencies: [
			install_cpp_headers_dep,
			libcxx_host_dependencies,
			libc_dep,
			libcxx_include_dep
		],
		build_by_default: meson.is_subproject() == false,
	)

	libcpp_fs_native = static_library('c++fs_native',
		libcpp_filesystem_files,
		cpp_args: libcxx_compile_flags + libcxx_native_compile_flags,
		link_args: libcxx_link_flags,
		dependencies: [
			install_cpp_headers_dep,
			libcxx_native_dependencies,
			libc_native_dep,
			libcxx_include_dep
		],
		native: true,
		build_by_default: meson.is_subproject() == false,
	)
else
	libcpp_fs = disabler()
	libcpp_fs_native = disabler()
endif

#######################
# libcxx Dependencies #
#######################

libcxx_full_dep = declare_dependency(
	include_directories: libcxx_include_directories,
	link_with: [libcpp, libcpp_experimental],
	dependencies: [
		install_cpp_headers_dep,
		libcxxabi_dep,
		libc_dep,
		libcxx_host_dependencies
	],
	compile_args: libcxx_dep_compile_flags + libcxx_host_dep_compiler_flags,
	link_args: libcxx_host_dep_link_flags
)

libcxx_full_native_dep = declare_dependency(
	include_directories: libcxx_include_directories,
	link_with: [libcpp_native, libcpp_experimental_native],
	dependencies: [
		install_cpp_headers_dep,
		libcxxabi_native_dep,
		libc_native_dep,
		libcxx_native_dependencies
	],
	compile_args: libcxx_dep_compile_flags + libcxx_native_dep_compiler_flags,
	link_args: libcxx_native_dep_link_flags
)

libcxx_filesystem_dep = declare_dependency(
	include_directories: libcxx_include_directories,
	link_with: libcpp_fs,
	dependencies: [
		install_cpp_headers_dep,
		libc_dep,
	],
	compile_args: libcxx_dep_compile_flags,
	link_args: libcxx_host_dep_link_flags
)

libcxx_filesystem_native_dep = declare_dependency(
	include_directories: libcxx_include_directories,
	link_with: libcpp_fs_native,
	dependencies: [
		install_cpp_headers_dep,
		libc_native_dep
	],
	compile_args: libcxx_dep_compile_flags,
	link_args: libcxx_native_dep_link_flags
)

libcxx_header_include_dep = declare_dependency(
	include_directories: libcxx_include_directories,
	dependencies: [
		install_cpp_headers_dep,
		libc_dep,
		libcxx_host_dependencies,
	],
	compile_args: libcxx_dep_compile_flags + libcxx_host_dep_compiler_flags
)

libcxx_native_header_include_dep = declare_dependency(
	include_directories: libcxx_include_directories,
	dependencies: [
		install_cpp_headers_dep,
		libc_native_dep,
		libcxx_native_dependencies,
	],
	compile_args: libcxx_dep_compile_flags + libcxx_native_dep_compiler_flags
)
