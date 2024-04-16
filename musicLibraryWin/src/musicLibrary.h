#pragma once
#include <stdint.h>

extern "C" {
	uint8_t* makeMusic(const char* fileName, size_t tactFirst, size_t tactSecond, size_t* xmlSize, const char* resultName);
}
