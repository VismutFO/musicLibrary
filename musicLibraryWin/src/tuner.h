#pragma once
#include <fftw3.h>

extern "C" {
	double* makeFFT(double* input, size_t inputSize);
	double calculateCurrentRMS(double* input, size_t inputSize);
	void Find_n_Largest(long long* output, long long* input, size_t n_out, size_t n_in, bool ignore_clumped = true);
	double getFrequency(double* input, size_t inputSize, int m_rate);
	long long getPitch(double frequency);
}