
#include "mp3Converter.h"
#include "musicLibrary.h"
#include <iostream>


#include "musicxml/elements.h"
#include "musicxml/factory.h"
#include "musicxml/xml.h"
#include "musicxml/xmlfile.h"

int main(int argc, char* argv[]) {
    size_t size;
	const char* sourceName;
	const char* resultName;
	size_t kBeats, kType;
	if (argc == 5) {
		sourceName = argv[1];
		resultName = argv[2];
		kBeats = std::stoi(argv[3]);
		kType = std::stoi(argv[4]);
	}
	else {
		std::cout << "Please, enter 4 arguments: source filepath, result filepath, kBeats anf kType" << std::endl;
		std::cout << "This time program will be executed with default arguments" << std::endl;
		sourceName = "zvuk-notyi-do.mp3";
		resultName = "C:\\Users\\start\\source\\repos\\musicLibraryWin\\musicLibraryWin\\zvuk-notyi-do.xml";
		kBeats = 2;
		kType = 16;
	}
	
	makeMusic(sourceName, kBeats, kType, &size, resultName);
	return 0;
} 
