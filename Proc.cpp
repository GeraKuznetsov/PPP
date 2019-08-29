#define _CRT_SECURE_NO_DEPRECATE
#include <fstream>
#include <string>

bool stringChar(char c) { return c == '\'' || c == '\"'; }

bool isWhiteSpace(char c) { return c == ' ' || c == '\t'; }

#define OUT_BUFFER_SIZE 2048

FILE * outFile;

std::string line;
std::string error;

char outBuffer[OUT_BUFFER_SIZE];
int outBufferIndex = 0;

int LEVEL = 0;

#define M_IGNORE 2
#define M_MULTI_IGNORE 3
char mode = 0;

char Char(int i) { return i < line.size() ? line[i] : ' '; }

bool CompString(int i, std::string s) {
	for (int n = 0; n < s.size(); n++) {
		if (Char(i + n) != s[n]) return false;
	}
	return true;
}

bool isNumberOrLetter(char c) {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

void Buff(char c) {
	outBuffer[outBufferIndex] = c;
	outBufferIndex++;
}

void SetUpOutBuffer() {
	outBufferIndex = 0;
	for (int i = 0; i < LEVEL; i++) {
		Buff(' '); Buff(' '); 	Buff(' '); 	Buff(' ');
	}
}

void FlushOutBuffer() {
	Buff(0);
	fprintf(outFile, "%s\n", outBuffer);
}

bool isKeyword(int i) {
	if (CompString(i, "if ")) return true;
	if (CompString(i, "def ")) return true;
	if (CompString(i, "class ")) return true;
	if (CompString(i, "for ")) return true;
	if (CompString(i, "while ")) return true;
	if (CompString(i, "else ")) return true;
	if (CompString(i, "elif ")) return true;
	return false;
}

int lineCount = 0, columb;

int Proc(const char* inFileName, const char* outFileName) {
	std::ifstream inFile(inFileName);

	if (!inFile.is_open()) {
		return -1;
	}

	outFile = fopen(outFileName, "w");
	SetUpOutBuffer();

	bool hadKeyword = false;
	bool dict = false;

	while (std::getline(inFile, line))
	{
		columb = -1;
		lineCount++;
		int size = line.size();
		char last_none_whitespace_char = ' ';
		for (int i = 0; i < size; i++) {
			columb++;
			char c = Char(i);
			if (isWhiteSpace(c)) {
				if (isNumberOrLetter(last_none_whitespace_char) && isNumberOrLetter(Char(i + 1))) {
					//keep
					Buff(c);
				}
			}
			else {
				last_none_whitespace_char = c;
				if (isKeyword(i)) {
					hadKeyword = true;
					Buff(c);
				}
				else if (c == '{') {
					if (hadKeyword) {
						LEVEL++;
						hadKeyword = false;
						Buff(':');
						FlushOutBuffer();
						SetUpOutBuffer();
					}
					else {
						dict = true;
						Buff('{');
					}
				}
				else if (c == '}') {
					if (hadKeyword) {
						//error
						error = "PreProc Error: Expected '{'\n";
						return 1;
					}
					if(!dict) {
						if (LEVEL == 0) {
							error = "Extra \'}\'";
							return 1;
						}
						LEVEL--;
						SetUpOutBuffer();
					}else{
						Buff('}');
						dict = false;
					}
				}
				else if (c == ';') {
					if (dict) {
						//error
						error = "PreProc Error: ';' inside dict\n";
						return 1;
					}
					if (hadKeyword) {
						//error
						error = "PreProc Error: Expected '{'\n";
						return 1;
					}
					FlushOutBuffer();
					SetUpOutBuffer();
				}
				else {
					Buff(c);
				}
			}
		}
	}
	FlushOutBuffer();
	fclose(outFile);
	return 0;
}