#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

typedef struct {
    float I;
    float Q;
} floatIQ_t;

typedef struct {
    int16_t I;
    int16_t Q;
} int16IQ_t;

int main(int argc, char ** argv)
{
    struct stat statBuffer;

    if (argc != 3) {
        std::cerr << "Wrong params count\n";
        return -1;
    }

    std::string fileFrom(argv[1]);
    std::string fileTo(argv[2]);

    if (stat(fileFrom.c_str(), &statBuffer) != 0) {
        std::cerr << "File 'from' not exist\n";
        return -1;
    }

    if ((statBuffer.st_size % sizeof (int16IQ_t)) != 0) {
        std::cerr << "Target file size incompatible\n";
        return -1;
    }

    std::ifstream input(fileFrom, std::ios::binary);
    std::ofstream output(fileTo, std::ios::binary);

    if (input.is_open() && output.is_open()) {

        std::vector<int16IQ_t> startData(statBuffer.st_size / sizeof (int16IQ_t));
        std::vector<floatIQ_t> targetData(statBuffer.st_size / sizeof (int16IQ_t));
        input.read((char*)startData.data(), statBuffer.st_size);

        size_t target = startData.size();
        size_t counter = 0;
        std::transform(startData.begin(), startData.end(), targetData.begin(), [&](int16IQ_t & data) -> floatIQ_t {
            counter++;
            std::cout << target << " \\ " << counter << "\r" << std::flush;
            return floatIQ_t{(float)data.I / (float)INT16_MAX, (float)data.Q / (float)INT16_MAX};
        });

        std::cout << std::endl;

        output.write((char*)targetData.data(), targetData.size() * sizeof (floatIQ_t));

        input.close();
        output.close();

        std::cout << "Success reformat\n";
        std::cout << "Result saved to: " << fileTo << std::endl;

    } else {
        std::cerr << "File operations not permited\n";
        return -1;
    }

    return 0;
}
