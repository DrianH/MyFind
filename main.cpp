#include <iostream>
#include <cassert>
#include <cctype>
#include <vector>
#include <filesystem>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>

using namespace std;

namespace fs = std::filesystem;

char* toLower(char *currentArg){
    char *p = currentArg;
    for(; *p != '\0'; p++){
        *p = tolower((*p));
    }
    return currentArg;
}

void fileIterator(const std::string &path, const std::string &filename) {
    for (const auto &entry : fs::directory_iterator(path))
        cout << entry.path() << endl;
}

void printFileName(string name){
    cout << "File: " << name << " - pid: " << getpid() << endl;
}

int main(int argc, char **argv) {
    //Terminate program with insufficient arguments
    if (argc < 3) {
        std::cerr << "insufficient argc. abort program execution" << std::endl;
        return EXIT_FAILURE;
    }

    int status;
    pid_t pid;

    int error = 0;
    int opt = 0;
    bool cOptionR = false, cOptionI = false; //'R' = recursive, 'i' = caseInsensitive

    //argument parsing -> permutes argv after completion!
    while ((opt = getopt(argc, argv, "Ri")) != EOF ) {
        switch (opt) {
            case 'R':
                std::cout << "Option -R (recursive) set!" << std::endl;
                //checks for duplicates
                if(cOptionR){
                    error = 1;
                    break;
                }
                cOptionR = true;
                break;
            case 'i':
                std::cout << "Option -i (case-insensitive) set!" << std::endl;
                //checks for duplicates
                if(cOptionI){
                    error = 1;
                    break;
                }
                cOptionI = true;
                break;
            case '?':
                error = 1;
                break;
            default:
                //unsure what to do here...
                assert(0);
        }
        if (error){
            cerr << "Multiple occurrences of opt parameters - program terminated" << endl;
            return EXIT_FAILURE;
        }
    }

    //set path for all processes and increment optind = points to the first filename
    string filePath = argv[optind++];
    vector<string> fileNames;

    for (int i = optind; i < argc; ++i) {
        if(cOptionI){
            argv[i] = toLower(argv[i]);
            fileNames.emplace_back(argv[i]);
        } else {
            fileNames.emplace_back(argv[i]);
        }
    }

    vector<pid_t> pidVector(fileNames.size());

    for(long unsigned int i = 0; i < pidVector.size(); ++i){
        if((pidVector[i] = fork()) < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pidVector[i] == 0){
            printFileName(fileNames[i]);
            exit(EXIT_SUCCESS);
        }
    }

    //both versions (top and bottom) work fine -> is a vector even necessary?
    /*
    for(;optind < argc; optind++){
        if((pid = fork()) < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0){
            pid = getpid();
            printFileName(argv[optind]);
            exit(EXIT_SUCCESS);
        }
    }*/

    /*int n = pidVector.size();
    while(n > 0){
        pid = wait(&status);
        cout << "Child with PID " << (long)pid << " exited with status 0x" << status << endl;
        n--;
    }*/
    wait(NULL);
    return EXIT_SUCCESS;
}
