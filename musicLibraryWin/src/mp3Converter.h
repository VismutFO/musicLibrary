#pragma once
#include <string>

int decodeMp3(std::string filename, std::string outString);
uint8_t* decodeMP3ToPCM(const char* fileName, size_t* pcmSize, int* m_bits, int* m_rate, int* m_channels);
double* processPCMData(uint8_t* data, size_t dataSize, int m_bits, int m_channels, size_t* resultSize);
