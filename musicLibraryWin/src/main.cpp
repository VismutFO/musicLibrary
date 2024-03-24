#include "mp3Converter.h"
#include "musicLibrary.h"
#include <iostream>


using namespace std;


int main() {
    // decodeMp3("in.mp3", "out.wav");
	size_t size;
	// makeMusic("in.mp3", 0, 0, &size);
	makeMusic("zvuk-notyi-re.mp3", 0, 0, &size);
	cout << "------------------\n\n\n";
	// cout << size << endl;
	return 0;
}