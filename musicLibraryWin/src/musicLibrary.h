#pragma once
#include <stdint.h>

extern "C" {
	uint8_t* makeMusic(uint8_t* data, size_t dataSize, size_t* xmlSize);
}
