#include <iostream>
#include <fstream>
#include <mpg123.h>

int decodeMp3(std::string filename, std::string outString)
{
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

    /* set the output format and open the output device */
    int m_bits = mpg123_encsize(encoding);
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