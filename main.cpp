#include <iostream>
#include <getopt.h>
#include <cassert>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

//for testing purposes: invoked by child processes
void checkPID(int pid){
    std::cout << "hello from function with id: " << getpid() << std::endl;
}

void fileIterator(const std::string &path, const std::string &filename) {
    for (const auto &entry : fs::directory_iterator(path))
        cout << entry.path() << endl;
}

int main(int argc, char **argv) {
    //Terminate program with insufficient arguments
    if (argc < 3) {
        std::cerr << "insufficient argc. abort program execution" << std::endl;
        return EXIT_FAILURE;
    }

    int error = 0;
    bool cOptionR = false, cOptionI = false; //'R' = recursive, 'i' = caseInsensitive
    int opt = 0;

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
                cOptionR = 1;
                break;
            case 'i':
                std::cout << "Option -i (case-insensitive) set!" << std::endl;
                //checks for duplicates
                if(cOptionI){
                    error = 1;
                    break;
                }
                cOptionI = 1;
                break;
            case '?':
                error = 1;
                break;
            default:
                //unsure what to do here...
                assert(0);
        }
        if (error){
            std::cerr << "Multiple occurrences of optional parameters" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::string filePath = argv[optind++];
    std::string fileName;

    //remember parent: var ppid binds pID from main process
    pid_t ppid = getpid();
    // Start children.
    pid_t childpid = 0, pid;

    //following code is NOT CORRECT. After a debugging session: the error is caused
    //by child processes forking new processes concurrently to the main process! See debug log for
    //further information.
    //TODO: fix forking bug (prevent child process from forking!)
    while(optind < argc){
        childpid = fork();

        switch (childpid) {
            case -1:
                cerr << "Forking failed..." << endl;
                return EXIT_FAILURE;
            case 0:
                //child
                pid = getpid();
                fileName = argv[optind];
                break;
            default:
                //parent
                break;
        }
        optind++;
    }

    //cout << "Filename: " << fileName << " and pID = " << childpid << endl;
    wait(nullptr);
    return EXIT_SUCCESS;
}
