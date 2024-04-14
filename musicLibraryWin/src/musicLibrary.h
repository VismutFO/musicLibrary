#pragma once
#include <stdint.h>

extern "C" {
	uint8_t* decodeMP3ToPCM(const char* fileName, size_t* pcmSize, int* m_bits, int* m_rate, int* m_channels);
	double* processPCMData(uint8_t* data, size_t dataSize, int m_bits, int m_channels, size_t* resultSize);
	uint8_t* makeMusic(const char* fileName, size_t tactFirst, size_t tactSecond, size_t* xmlSize, const char* resultName);
}
