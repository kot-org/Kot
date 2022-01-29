#include <iostream>
#include <filesystem>
#include <fstream>
#include "RamFs.h"

int main(int argc, char** argv)
{
	//if (argc != 4) return 1;
	char* fileName = argv[arg_outfile];
	fileName = (char*)"D:\\Data\\users\\Konect\\1Documents\\programmation\\Kot\\Bin\\RamFS.bin";

	//get all files
	Header* header = (Header*)malloc(sizeof(Header));
	header->signature[0] = 'r';
	header->signature[1] = 'a';
	header->signature[2] = 'm';
	header->signature[3] = 'f';
	header->signature[4] = 's';

	header->version = VERSION;

	header->filenumber = 0;
	header->initfile = 0;

	int fileInitPos = 0; 

	uint64_t size = sizeof(Header);

	for (int i = arg_infiles; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == 'i') {
			//file init
			memcpy(argv[i], (void*)(argv[i][2]), strlen(argv[i]) - 2);
			fileInitPos = i;
		}

		header->filenumber++;

		FILE* f = fopen(argv[i], "r");
		fseek(f, 0, SEEK_END);
		size += ftell(f) + sizeof(File);
		fclose(f);
	}

	void* buffer = malloc(size);

	//open RamFS file
	std::fstream file(fileName, std::ios::binary | std::ios::out | std::ios::in);

	uint64_t cursorlocation = sizeof(Header);
	
	for (int i = 0; i < header->filenumber; i++) {
		printf("Copy %s to ram FS \n", argv[i + arg_infiles]);
		FILE* f = fopen(argv[i + arg_infiles], "r");
		fseek(f, 0, SEEK_END);
		uint64_t filesize = ftell(f);
		File* fileHeader = (File*)((uint64_t)buffer + (uint64_t)cursorlocation);
		fileHeader->size = filesize;

		std::string path(argv[i + arg_infiles]);
		std::string fileName = path.substr(path.find_last_of("/\\") + 1);
		uint64_t sizeofname = fileName.length();
		if (sizeofname > MaxName) {
			sizeofname = MaxName;
		}

		memset(fileHeader->name, 0, MaxName);
		memcpy(fileHeader->name, &fileName[0], sizeofname);
		fileHeader->name[MaxName] = 0;

		rewind(f);
		fread((void*)((uint64_t)buffer + (uint64_t)cursorlocation + sizeof(File)), 1, filesize, f);
		fclose(f);

		if (i == fileInitPos) {
			header->initfile = cursorlocation;
		}

		cursorlocation += filesize + sizeof(File);
	}

	memcpy(buffer, header, sizeof(Header));
	file.seekp(0);
	file.write((char*)buffer, size);

	file.close();
}
