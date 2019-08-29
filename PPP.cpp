#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "Dirent.h"
#include <string>
#include <vector>

#define PATH_LENGTH 1024

int Proc(const char* inFileName, const char* outFileName);

extern std::string line, error;
extern int lineCount, columb;

std::string getWorkingPath() {
	TCHAR path[PATH_LENGTH];
	DWORD length = MAX_PATH;
	GetCurrentDirectory(length, path);
	char char_path[PATH_LENGTH];
	wcstombs(char_path, path, PATH_LENGTH);
	return std::string(char_path);
}

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

void main(int argc, char* argv[])
{
	bool invokePython = false;

	std::string filePath;

	if (argc == 3) {
		if (!strcmp(argv[1], "-R")) {
			invokePython = true;
			filePath = argv[2];
		}
		else {
			printf("Syntex: \"PPP [-R] <file>\"");
			return;
		}
	}
	else if (argc == 2) {
		filePath = argv[1];
	}
	else {
		printf("Syntex: \"PPP [-R] <file>\"");
		return;
	}

	std::string workingPath = getWorkingPath();


	if (filePath.size() > 4 && filePath[1] == ':' && (filePath[1] == '\\' || filePath[1] == '/')) { //absolute path
		workingPath = "";
	}

	int ext_index = filePath.find_last_of('.');;
	std::string ext = filePath.substr(ext_index + 1);
	std::string path_without_ext = filePath.substr(0, ext_index);

	std::string inPath = workingPath + "/" + filePath;
	std::string outPath = workingPath + "/" + path_without_ext + ".py";

	printf("Proccessing file: %s\n", filePath.c_str());
	int e = Proc(inPath.c_str(), outPath.c_str());
	if (e == -1) {
		printf("Failed to read file\n");
		return;
	}
	if (e) {
		printf("Error: %s  on line %d\n", error.c_str(), lineCount);
		printf("%s\n", line.c_str());
		for (int i = 0; i < columb; i++) {
			printf(" ");
		}
		printf("^");
		return;
	}
	if (invokePython) {
		printf("Running file: %s\n", outPath.c_str());
		system(("python " + outPath).c_str());
	}
	return;
}