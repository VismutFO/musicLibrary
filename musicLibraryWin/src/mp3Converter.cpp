#include <iostream>
#include <fstream>
#include <iostream>
#include <mpg123.h>

int decodeMp3(std::string filename, std::string outString) {
    mpg123_handle* mh;
    //unsigned char *buffer;
    unsigned char* buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int channels, encoding;
    long rate;

    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, filename.c_str());
    mpg123_getformat(mh, &rate, &channels, &encoding);
    std::cerr << "channels: " << channels << std::endl;

    /* set the output format and open the output device */
    int m_bits = mpg123_encsize(encoding) * 8;
    std::cerr << "bits: " << m_bits << std::endl;
    int m_rate = rate;
    int m_channels = channels;
    std::ofstream out(outString.c_str(), std::ios::binary);
    /* decode and play */
    for (int totalBtyes = 0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK; )
    {
        char* data = new char[done + 1];
        for (int i = 0; i != done; i++)
        {
            char tst = static_cast<char>(buffer[i]);
            //cout << done << " " << sizeof(data) << endl;
            out << tst;
        }

        totalBtyes += done;
    }
    //out << buffer[i];

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
    /* clean up */
    free(buffer);
    out.close();
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    return 0;
}

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