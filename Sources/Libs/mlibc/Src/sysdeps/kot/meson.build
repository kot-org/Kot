rtdl_sources += files(
	'generic/generic.cpp',
)

libc_sources += files(
	'generic/sys.cpp',
	'generic/shell.cpp',
	'generic/atomic.S',
	'generic/entry.cpp',
	'generic/thread.cpp',
	'generic/memory.cpp',
	'generic/cstring.cpp',
	'generic/generic.cpp',
	'generic/uisd/uisd.cpp',
	'generic/filesystem.cpp',
	'generic/utils/utils.cpp',
	'generic/utils/map/map.cpp',
	'generic/uisd/srvs/pci/pci.cpp',
	'generic/uisd/srvs/hid/hid.cpp',
	'generic/uisd/srvs/time/time.cpp',
	'generic/utils/vector/vector.cpp',
	'generic/uisd/srvs/storage/fs.cpp',
	'generic/uisd/srvs/audio/audio.cpp',
	'generic/uisd/srvs/system/system.cpp',
	'generic/uisd/srvs/storage/device.cpp',
	'generic/uisd/srvs/storage/storage.cpp',
	'generic/uisd/srvs/graphics/graphics.cpp',
	'generic/uisd/srvs/authorization/authorization.cpp',
)

if not no_headers
	install_headers(
		'include/abi-bits/auxv.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/in.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/socket.h',
		'include/abi-bits/termios.h',
		'include/abi-bits/time.h',
		'include/abi-bits/blkcnt_t.h',
		'include/abi-bits/blksize_t.h',
		'include/abi-bits/dev_t.h',
		'include/abi-bits/gid_t.h',
		'include/abi-bits/ino_t.h',
		'include/abi-bits/mode_t.h',
		'include/abi-bits/nlink_t.h',
		'include/abi-bits/pid_t.h',
		'include/abi-bits/uid_t.h',
		'include/abi-bits/access.h',
		'include/abi-bits/wait.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/utsname.h',
		'include/abi-bits/ptrace.h',
		'include/abi-bits/poll.h',
		'include/abi-bits/epoll.h',
		'include/abi-bits/packet.h',
		'include/abi-bits/inotify.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/shm.h',
		'include/abi-bits/mqueue.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/fsfilcnt_t.h',
		'include/abi-bits/fsblkcnt_t.h',
		'include/abi-bits/socklen_t.h',
		'include/abi-bits/statfs.h',
		'include/abi-bits/statvfs.h',
		'include/abi-bits/ioctls.h',
		'include/abi-bits/xattr.h',
		subdir: 'abi-bits'
	)
	install_headers(
		'include/kot/authorization.h',
		'include/kot/keyhole.h',
		'include/kot/syscall.h',
		'include/kot/cstring.h',
		'include/kot/memory.h',
		'include/kot/atomic.h',
		'include/kot/thread.h',
		'include/kot/x86_64.h',
		'include/kot/shell.h',
		'include/kot/types.h',
		'include/kot/utils.h',
		'include/kot/math.h',
		'include/kot/uisd.h',
		'include/kot/bits.h',
		'include/kot/arch.h',
		'include/kot/sys.h',
		subdir: 'kot'
	)
	install_headers(
		'include/kot/syscall/list.h',
		subdir: 'kot/syscall'
	)
	install_headers(
		'include/kot/utils/map.h',
		'include/kot/utils/vector.h',
		subdir: 'kot/utils'
	)
	install_headers(
		'include/kot/uisd/srvs/audio.h',
		'include/kot/uisd/srvs/graphics.h',
		'include/kot/uisd/srvs/hid.h',
		'include/kot/uisd/srvs/pci.h',
		'include/kot/uisd/srvs/storage.h',
		'include/kot/uisd/srvs/system.h',
		'include/kot/uisd/srvs/time.h',
		subdir: 'kot/uisd/srvs'
	)
	install_headers(
		'include/kot/uisd/srvs/storage/device.h',
		subdir: 'kot/uisd/srvs/storage'
	)
endif

if not headers_only
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-x86_64/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
