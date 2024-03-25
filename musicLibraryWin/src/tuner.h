#pragma once
#include <fftw3.h>

extern "C" {
	double* makeFFT(double* input, size_t inputSize);
	double calculateCurrentRMS(double* input, size_t inputSize);
	int getNoteNumber(double* input, size_t inputSize, int m_rate);
}