#include <iostream>
#include <cassert>
#include <cctype>
#include <filesystem>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>
#include <algorithm>

using namespace std;

namespace fs = std::filesystem;

char* toLower(char *currentArg){
    char *p = currentArg;
    for(; *p != '\0'; p++){
        *p = tolower((*p));
    }
    return currentArg;
}

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); }
    );
    return s;
}

void searchFile(const std::string &path, const std::string &filename, bool searchRecursive, bool searchInsensitive) {
    fs::path aPath{path};
    if (searchRecursive){ //recursive Iteration through filesystem
        for (const auto &entry : fs::recursive_directory_iterator(path)) {
            string dirEntry = entry.path().filename().string();
            if (searchInsensitive){
                dirEntry = str_tolower(dirEntry);
            }
            if(dirEntry == filename){
                cout << getpid() << " : " << filename << " : " << entry.path().string() << endl;
            }
        }
    } else { // simple search in defined path
        for (const auto &entry : fs::directory_iterator(path)){
            string dirEntry = entry.path().filename().string();
            if(searchInsensitive){
                dirEntry = str_tolower(dirEntry);
            }
            if(dirEntry == filename){
                cout << getpid() << " : " << filename << " : " << entry.path().string() << endl;
            }
        }
    }
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

    //set path for all processes
    string filePath = argv[optind];

    //if case-insensitive search mode is set, convert
    if(cOptionI){
        for (int i = (optind + 1); i < argc ; ++i) {
            argv[i] = toLower((argv[i]));
        }
    }

    optind++;
    for(;optind < argc; optind++){
        if((pid = fork()) < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0){
            searchFile(filePath, argv[optind], cOptionR, cOptionI);
            exit(EXIT_SUCCESS);
        }
    }
    /*
    vector<string> fileNames;
    vector<pid_t> pidVector(fileNames.size());
    for(long unsigned int i = 0; i < pidVector.size(); ++i){
        if((pidVector[i] = fork()) < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pidVector[i] == 0){
            printFileName(fileNames[i]);
            //searchFile(filePath, fileNames[i]);
            exit(EXIT_SUCCESS);
        }
    }*/

    //parent process waiting for child processes to finish before terminating
    while((pid = wait(&status)) > 0){
        if(pid == -1){
            perror("Failed to wait for child");
        } else if (WIFEXITED(status) && !WEXITSTATUS(status)) {
            printf("Child %d has finished normally\n", pid);
        } else if (WIFEXITED(status)) {
            printf("Child %d has finished with exit code: %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)){
            printf("Child %d has terminated du to uncaught signal: %d\n", pid, WTERMSIG(status));
        } else if (WIFSTOPPED(status)){
            printf("Child %d stopped due to signal: %d\n", pid, WSTOPSIG(status));
        }
    }
    //wurde nach der Aufnahme vom Video noch hinzugefügt um den Ablauf nochmal zu verdeutlichen :)
    cout << "parent process finishing last!" << endl;
    return EXIT_SUCCESS;
}
