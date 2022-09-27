#include <iostream>
#include <getopt.h>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char **argv) {

    /*
    std::cout << "Argument Counter = " << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument Vector: " << argv[i] << std::endl;
    }*/

    char opt = 0;
    bool recSearch = false;
    bool isCaseInsensitive = false;

    while ((opt = getopt(argc, argv, "Ri:")) != EOF ) {
        switch (opt) {
            case 'R':
                recSearch = true;
                break;
            case 'i':
                isCaseInsensitive = true;
                break;
            default:
                //nothing found
                std::cerr << "no valid option found!" << std::endl;
                return EXIT_FAILURE;
        }
    }

    std::cout << recSearch << std::endl;
    std::cout << isCaseInsensitive << std::endl;

    return EXIT_SUCCESS;
}

void fileIterator(std::string path, std::string filename) {
    using namespace std;

    for (const auto &entry : fs::directory_iterator(path))
        cout << entry.path() << endl;
}
