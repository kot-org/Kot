/*
 * Written by Tim Kientzle, March 2009.
 *
 * Released into the public domain.
 */

#include <stdio.h>

static int parseoct(const char *p, size_t n){
	int i = 0;

	while (*p < '0' || *p > '7') {
		++p;
		--n;
	}
	while (*p >= '0' && *p <= '7' && n > 0) {
		i *= 8;
		i += *p - '0';
		++p;
		--n;
	}
	return (i);
}

/* Returns true if this is 512 zero bytes. */
static int is_end_of_archive(const char *p){
	int n;
	for (n = 511; n >= 0; --n)
		if (p[n] != '\0')
			return (0);
	return (1);
}

/* Create a directory, including parent directories as necessary. */
static void create_dir(char *pathname, int mode){
	char *p;
	int r;

	/* Strip trailing '/' */
	if (pathname[strlen(pathname) - 1] == '/')
		pathname[strlen(pathname) - 1] = '\0';

	/* Try creating the directory. */
	r = mkdir(pathname, mode);

	if (r != 0) {
		/* On failure, try creating parent directory. */
		p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			create_dir(pathname, 0755);
			*p = '/';
			r = mkdir(pathname, mode);
		}
	}
	if (r != 0)
		fprintf(stderr, "Could not create directory %s\n", pathname);
}

/* Create a file, including parent directory as necessary. */
static FILE* create_file(char *pathname, int mode){
	FILE *f;
	f = fopen(pathname, "w+");
	if (f == NULL) {
		/* Try creating parent dir and then creating file. */
		char *p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			create_dir(pathname, 0755);
			*p = '/';
			f = fopen(pathname, "w+");
		}
	}
	return (f);
}

/* Verify the tar checksum. */
static int verify_checksum(const char *p){
	int n, u = 0;
	for (n = 0; n < 512; ++n) {
		if (n < 148 || n > 155)
			/* Standard tar checksum adds unsigned bytes. */
			u += ((unsigned char *)p)[n];
		else
			u += 0x20;

	}
	return (u == parseoct(p + 148, 8));
}

/* Extract a tar archive. */
int untar(const char* tar_path, const char* dst_path){
	char buffer[512];
	FILE* file_tmp = NULL;
	FILE* tar_file = fopen(tar_path, "r");;
	size_t bytes_read;
	int file_size;
    int dst_path_len = strlen(dst_path);

	printf("Extracting from %s\n", tar_path);
	for (;;) {
		bytes_read = fread(buffer, 1, 512, tar_file);
		if (bytes_read < 512) {
			fprintf(stderr, "Short read on %s: expected 512, got %d\n", tar_path, bytes_read);
			return -1;
		}
		if (is_end_of_archive(buffer)) {
			printf("End of %s\n", tar_path);
			return -1;
		}
		if (!verify_checksum(buffer)) {
			fprintf(stderr, "Checksum failure\n");
			return -1;
		}
		file_size = parseoct(buffer + 124, 12);
		switch (buffer[156]) {
		case '1':
			printf(" Ignoring hardlink %s\n", buffer);
			break;
		case '2':
			printf(" Ignoring symlink %s\n", buffer);
			break;
		case '3':
			printf(" Ignoring character device %s\n", buffer);
				break;
		case '4':
			printf(" Ignoring block device %s\n", buffer);
			break;
		case '5':
            char* path_dir = malloc(dst_path_len + strlen(buffer) + 1);
            strcpy(path_dir, dst_path);
            strcat(path_dir, buffer);
			printf(" Extracting dir %s to %s\n", buffer, path_dir);
			create_dir(path_dir, parseoct(buffer + 100, 8));
			file_size = 0;
			break;
		case '6':
			printf(" Ignoring FIFO %s\n", buffer);
			break;
		default:
            char* path_file = malloc(dst_path_len + strlen(buffer) + 1);
            strcpy(path_file, dst_path);
            strcat(path_file, buffer);
			printf(" Extracting file %s to %s\n", buffer, path_file);
			file_tmp = create_file(path_file, parseoct(buffer + 100, 8));
			break;
		}
		while (file_size > 0) {
			bytes_read = fread(buffer, 1, 512, tar_file);
			if (bytes_read < 512) {
				fprintf(stderr, "Short read on %s: Expected 512, got %d\n", dst_path, bytes_read);
				return -1;
			}
			if (file_size < 512)
				bytes_read = file_size;
			if(file_tmp != NULL){
				if (fwrite(buffer, 1, bytes_read, file_tmp) != bytes_read){
					fprintf(stderr, "Failed write\n");
					fclose(file_tmp);
					file_tmp = NULL;
				}
			}
			file_size -= bytes_read;
		}
		if(file_tmp != NULL){
			fclose(file_tmp);
			file_tmp = NULL;
		}
	}

    fclose(tar_file);

    return 0;
}