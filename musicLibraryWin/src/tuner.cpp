#include "tuner.h"

#include <cmath>
#include <cstring>
#include <iostream>

double* makeFFT(double* input, size_t inputSize) { // resultSize same as dataSize
    fftw_complex* fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * inputSize);
    memset(fftOutput, 0, sizeof(fftw_complex) * inputSize);
    const fftw_plan fftPlan = fftw_plan_dft_r2c_1d(inputSize, input, fftOutput, FFTW_ESTIMATE); // double* input, fftw_complex* output
    fftw_execute(fftPlan);
    double* result = new double[inputSize];
    for (size_t i = 0; i < inputSize; i++) {
        result[i] = fftOutput[i][0];
    }
    fftw_free(fftOutput);
    fftw_destroy_plan(fftPlan);
    return result;
}

double calculateCurrentRMS(double* input, size_t inputSize) {
    double sum = 0.;
    for (size_t i = 0; i < inputSize; i++) {
        sum += input[i] * input[i];
    }
    {
        // std::cout << "sum: " << sum << ", inputSize: " << inputSize << std::endl;
    }
    return sqrt(sum / inputSize);
}

/*
void Find_n_Largest(long long* output, long long* input, size_t n_out, size_t n_in, bool ignore_clumped) {
    long long min_pos = 0;

    for (int i = 0; i < n_in; i++) {
        if (input[i] < input[min_pos]) {
            min_pos = i;
        }
    }

    for (int i = 0; i < n_out; i++) {
        output[i] = min_pos;
    }

    /**
    Now, iterate through inputs and for each input:
    1.  Check whether it is part of a clump
        - If part of a clump store output index of the other clump-mate
    2.  Check whether it is greater than ANY output
    3.  If input i is greater than some output j and input i is not clumped
        - Insert index i before output j (shifting outputs j, j+1, j+2... to the right)
        Else if input i is greater than output j and its clump-mate is to the right of j
        - Delete the other clump-mate (it is the smaller of the two clump-mates) and insert input index i at output location j
        Else if input i is greater than output j and its clump-mate is to the left of j
        - Do nothing (clump already represented)
    ** /
    for (int i = 0; i < n_in; i++) {
        /// Check if the current input element is part of a clump of peaks
        /// i.e., check if the index of the previous input element has been added to the output
        bool part_of_clump = false;
        int OutputClumpMate;
        if (output[i] == min_pos + 1) {                                      /// The (min_pos)th element is a non-spike,
            part_of_clump = false;                                      /// so the (min_pos+1)th element cannot be part of a clump
        }
        else if (i > 0) {                                                   /// Only elements input[i>0] have a previous element to check
            for (int j = 0; j < n_out; j++) {
                if (output[j] == i - 1 && j != min_pos) {                      /// If part of clump
                    part_of_clump = true;                               /// Set clump flag and also
                    OutputClumpMate = j;                                /// store output index of clump-mate, i.e., previous input index
                }
            }
        }

        /// Check whether the current input is bigger than any output
        for (int j = 0; j < n_out; j++) {
            if (input[i] > input[output[j]] * 1.01) {
                /// If it is, check if clumping must be accounted for
                if (!(part_of_clump && ignore_clumped)) {
                    /// If not, just insert
                    for (int k = n_out - 1; k > j; k--) {
                        output[k] = output[k - 1];
                    }
                    output[j] = i;
                    break;
                }
                /// Otherwise, check if clump-mate is to the right of the would-be position of the new addition
                else if (OutputClumpMate > j) {
                    /// And if so, shift everything between the would-be position and the clump-mate position to the right
                    /// and insert the new addition where it belongs
                    for (int k = OutputClumpMate; k > j; k--) {
                        output[k] = output[k - 1];
                    }
                    output[j] = i;
                    break;
                }
            }
        }
    }
}
*/

void findMaxY(double* vData, size_t length, double* maxY, size_t* index) {
    *maxY = 0;
    *index = 0;
    // If sampling_frequency = 2 * max_frequency in signal,
    // value would be stored at position samples/2
    for (size_t i = 1; i < length; i++) {
        if ((vData[i - 1] < vData[i]) && (vData[i] > vData[i + 1])) {
            if (vData[i] > vData[*index]) {
                *index = i;
            }
        }
    }
    *maxY = vData[*index];
}

void majorPeak(double* input, size_t samples, double samplingFrequency, double* frequency) {
    double maxY = 0;
    size_t IndexOfMaxY = 0;
    findMaxY(input, (samples >> 1) + 1, &maxY, &IndexOfMaxY);

    double delta = 0.5 * ((input[IndexOfMaxY - 1] - input[IndexOfMaxY + 1]) /
        (input[IndexOfMaxY - 1] - (2.0 * input[IndexOfMaxY]) +
            input[IndexOfMaxY + 1]));
    double interpolatedX = ((IndexOfMaxY + delta) * samplingFrequency) / (samples - 1);
    if (IndexOfMaxY == (samples >> 1)) // To improve calculation on edge values
        interpolatedX = ((IndexOfMaxY + delta) * samplingFrequency) / (samples);
    // returned value: interpolated frequency peak apex
    *frequency = interpolatedX;
}

int getNoteNumber(double* input, size_t inputSize, int m_rate) {
    /*
    double* bufferCopy = new double[inputSize];
    memcpy(bufferCopy, input, inputSize);

    size_t r1 = 0;
    size_t r2 = inputSize - 1;
    double thres = 0.25;
    for (size_t i = 0; i < inputSize / 2; i++) {
        if (abs(bufferCopy[i]) < thres) {
            r1 = i;
            break;
        }
    }
    for (size_t i = 1; i < inputSize / 2; i++) {
        if (abs(bufferCopy[inputSize - i]) < thres) {
            r2 = inputSize - i;
            break;
        }
    }
    size_t newSize = r2 - r1;

    double* c = new double[newSize];
    //var c = new Array(this.bufferLength).fill(0);
    for (size_t i = 0; i < newSize; i++) {
        c[i] = 0.;
        for (size_t j = 0; j < newSize - i; j++) {
            c[i] = c[i] + (bufferCopy + r1)[j] * (bufferCopy + r1)[j + i];
        }
    }

    size_t d = 0;
    while (d < newSize - 1 && c[d] > c[d + 1]) {
        d++;
    }

    double maxval = -1;
    long long maxpos = -1;
    for (size_t i = d; i < newSize; i++) {
        if (c[i] > maxval) {
            maxval = c[i];
            maxpos = i;
        }
    }
    long long T0 = maxpos;

    double x1 = c[T0 - 1];
    double x2 = c[T0];
    double x3 = c[(T0 + 1) % newSize];
    double a = (x1 + x3 - 2. * x2) / 2.;
    double b = (x3 - x1) / 2.;
    if (a) {
        T0 = T0 - b / (2. * a);
    }

    if (T0 != -1) {
        return (double)m_rate / T0;
    }
    else {
        return -1.;
    }
    */
    double peak = -1.;
    majorPeak(input, inputSize, m_rate, &peak);
    int midi_num = round(12.0 * log2(peak / 440.0) + 69.0);
    // double expected_freq = pow(2.0, (midi_num - 69.0) / 12.0) * 440.0;
    // double cents = 1200 * log2(peak / expected_freq);
    return midi_num;
}
/*
function noteFromPitch(frequency) {
    var noteNum = 12 * (Math.log(frequency / 440) / Math.log(2));
    return Math.round(noteNum) + 69;
}
*/
