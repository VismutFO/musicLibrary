#include "musicLibrary.h"
#include "xmlBuild.h"
#include "tuner.h"

#include <fstream>
#include <mpg123.h>
#include <complex>
#include <iostream>

#define FFT_WINDOW 8192
#define MIN_RMS 0.3

uint8_t* decodeMP3ToPCM(const char* fileName, size_t* pcmSize, int* m_bytes, int* m_rate, int* m_channels) {
    mpg123_handle* mh;
    //unsigned char *buffer;
    uint8_t* buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int channels, encoding;
    long rate;

    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = new uint8_t[buffer_size];//(uint8_t*)malloc(buffer_size * sizeof(uint8_t));

    /* open the file and get the decoding format */
    mpg123_open(mh, fileName);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    *m_bytes = mpg123_encsize(encoding);
    *m_rate = rate;
    *m_channels = channels;
    
    // std::ofstream out(outString.c_str(), std::ios::binary);
    uint8_t* result = (uint8_t*)malloc(buffer_size * sizeof(uint8_t));
    size_t resultSize = buffer_size, offset = 0;
    
    /* decode and play */
    for (; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK; )
    {
        if (offset + done > resultSize) { // need to get more memory
            resultSize += buffer_size; // TODO optimize
            result = (uint8_t*)realloc(result, resultSize);
        }
        memcpy(result + offset, buffer, done);
        offset += done;
    }
    //out << buffer[i];
    /*
    std::cout << "Buffer size " << buffer_size << std::endl;
    std::cout << "Done " << done << std::endl;
    std::cout << "err " << err << std::endl;
    std::cout << "channels " << channels << std::endl;
    std::cout << "encoding " << encoding << std::endl;
    std::cout << "Rate " << rate << std::endl;
    std::cout << "m_bits " << m_bits << std::endl;
    std::cout << "m_rate " << m_rate << std::endl;
    std::cout << "m_channels " << m_channels << std::endl;
    std::cout << "The size of buffer " << sizeof(buffer) << std::endl;
    */
    /* clean up */
    //free(buffer);
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    *pcmSize = offset;
    return result;
}

double* processPCMData(uint8_t* data, size_t dataSize, int m_bytes, int m_channels, size_t* resultSize) {
    // std::cerr << "pcmSize: " << dataSize << std::endl;
    // std::cerr << m_bytes << " " << m_channels << std::endl;
    *resultSize = dataSize / m_bytes / m_channels;
    // std::cerr << *resultSize << std::endl;
    // bool nonZero = true;
    /*
    for (size_t i = 0; i < dataSize; i++) {
        if (data[i] != 0) {
            std::cerr << "NonZero at " << i << std::endl;
        }
    }
    */
    double* result = new double[*resultSize];
    for (size_t i = 0; i < *resultSize; i++) {
        //long long temp = 0;
        result[i] = 0.;
        // for (size_t j = 0; j < m_bytes; j += m_bytes) {
        
            switch (m_bytes) {
            case 1:
                result[i] = (double)(*(char*)&data[i * m_bytes * m_channels]) / 128.;
                break;
            case 2:
                result[i] = (double)(*(short*)&data[i * m_bytes * m_channels]) / 32768.;
                break;
            case 4:
                result[i] = (double)(*(int*)&data[i * m_bytes * m_channels]) / 2147483648.;
                break;
            }
            result[i] /= 2.;
            // temp = (temp * 256) + data[i * m_bytes * m_channels + j];
        // }
        
        // result[i] = (double)temp;
        // std::cerr << temp << " " << result[i] << std::endl;
        // i += (m_channels - 1); // skipping all channels except first
    }
    // std::cerr << "processPCMData completed" << std::endl;
    return result;
}

void printDouble(double* input, size_t inputSize) {
    for (size_t i = 0; i < inputSize; i++) {
        std::cout << input[i] << " ";
    }
    std::cout << std::endl << std::endl;
}

bool isMeasureFull(const std::vector<std::pair<int, int>>& currentMeasure, size_t kBeats, size_t kType) {
    int sum = 0;
    for (auto x : currentMeasure) {
        sum += (16 / x.second);
    }
    if (sum > kBeats * (16 / kType)) {
        std::cerr << "error: measure is bigger than possible" << std::endl;
    }
    return sum == kBeats * (16 / kType);
}

bool updateMeasures(std::vector<std::vector<std::pair<int, int>>>& measures,
    std::vector<std::pair<int, int>>& currentMeasure, int note, size_t kBeats, size_t kType) {
    currentMeasure.push_back({ note, 16 });
    if (isMeasureFull(currentMeasure, kBeats, kType)) {
        measures.push_back(currentMeasure);
        return true;
    }
    return false;
}

void printMusicInConsole(const std::vector<std::vector<std::pair<int, int>>>& measures) {
    std::cout << "begin:\n";
    for (auto& measure : measures) {
        for (auto& note : measure) {
            if (note.first == -1) {
                std::cout << "pause";
            }
            else {
                std::cout << getStep(note.first % 12);
                if (getAlter(note.first % 12)) {
                    std::cout << "#";
                }
            }
            std::cout << "/" << note.second << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\nend" << std::endl;
}

uint8_t* makeMusic(const char* fileName, size_t kBeats, size_t kType, size_t* xmlSize) {
    /*
    size_t num_spikes = 5;                                             /// Number of fft spikes to consider for pitch deduction
    long long SpikeLocs[100];                                             /// Array to store indices in spectrum[] of fft spikes
    float SpikeFreqs[100];                                          /// Array to store frequencies corresponding to spikes

    Find_n_Largest(SpikeLocs, data, num_spikes, dataSize);      /// Find spikes

    for (int i = 0; i < num_spikes; i++)                                 /// Find spike frequencies (assumed to be harmonics)
        SpikeFreqs[i] = index2freq(SpikeLocs[i]);

    float pitch = approx_hcf(SpikeFreqs, num_spikes, 5, 50);        /// Find pitch as approximate HCF of spike frequencies

    if (pitch)                                                       /// If pitch found, update notenames and print
    {
        for (int i = 0; i < window_width; i++)                           /// First initialize notenames to all whitespace
            notenames[i] = ' ';

        /// Find pitch number (1 = A, 2 = A# etc.) and how many cents sharp or flat (centsOff<0 means flat)
        float centsOff;
        int pitch_num = pitchNumber(pitch, &centsOff);

        /// Find appropriate location for pitch letter name based on centsOff
        /// (centsOff = 0 means "In Tune", location exactly in the  middle of the window)
        int loc_pitch = window_width / 2 - centsOff * window_width / (span_semitones * 100);

        /// Write letter name corresponding to current pitch to appropriate location
        pitchName(notenames + loc_pitch, pitch_num);

        /// Write letter names of as many lower pitches as will fit on screen
        int loc_prev_pitch = loc_pitch - window_width / span_semitones;
        for (int i = 0; loc_prev_pitch > 0; i++)
        {
            pitchName(notenames + loc_prev_pitch, (22 - i + pitch_num) % 12 + 1);
            loc_prev_pitch -= window_width / span_semitones;
        }

        /// Write letter names of as many higher pitches as will fit on screen
        int loc_next_pitch = loc_pitch + window_width / span_semitones;
        for (int i = 0; loc_next_pitch < window_width; i++)
        {
            pitchName(notenames + loc_next_pitch, (pitch_num + i) % 12 + 1);
            loc_next_pitch += window_width / span_semitones;
        }

        notenames[window_width] = '\0';                             /// Terminate string

        std::cout << '\r' << notenames;                                 /// Print notenames
    }
    */
    size_t pcmSize;
    int m_bytes, m_rate, m_channels;
    uint8_t* pcmData = decodeMP3ToPCM(fileName, &pcmSize, &m_bytes, &m_rate, &m_channels);

    size_t fftInSize;
    double* fftIn = processPCMData(pcmData, pcmSize, m_bytes, m_channels, &fftInSize);

    delete[] pcmData;
    // std::cerr << "After processPCMData" << std::endl;

    std::vector<std::vector<std::pair<int, int>>> measures;
    std::vector<std::pair<int, int>> currentMeasure = std::vector<std::pair<int, int>>();

    for (size_t offset = 0; offset + FFT_WINDOW - 1 < fftInSize; offset += FFT_WINDOW) {
        double* fftOut = makeFFT(fftIn + offset, FFT_WINDOW);
        double currentRMS = calculateCurrentRMS(fftOut, FFT_WINDOW);
        
        if (currentRMS > MIN_RMS) {
            int note = getNoteNumber(fftOut, FFT_WINDOW, m_rate);
            // std::cout << note << std::endl;
            // place note(?) in xml?
            if (updateMeasures(measures, currentMeasure, note, kBeats, kType)) {
                currentMeasure = std::vector<std::pair<int, int>>();
            }
        }
        else {
            if (updateMeasures(measures, currentMeasure, -1, kBeats, kType)) {
                currentMeasure = std::vector<std::pair<int, int>>();
            }
        }
        //
        delete[] fftOut;
    }
    // printMusicInConsole(measures);
    printMusic(getMusicByMeasures(kBeats, kType, measures));

    return nullptr;
}