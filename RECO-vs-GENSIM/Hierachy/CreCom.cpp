#include <fstream>
#include <iostream>
#include <string>
using namespace std;

void CreCom(const char* arg1, const char* arg2) {
    std::ifstream inputFile(arg1);
    std::ofstream outputFile("temp3.txt");
    std::string line;

    if (inputFile.is_open() && outputFile.is_open()) {
        string previousline = "0";
        while (std::getline(inputFile, line)) {
            try {
                int number=stoi(line);
                if (number < 1) {
                    continue;
                }
            } catch (...) {
                continue;
            }
            if (previousline != "0") {
                outputFile << "sed -n '" << stoi(previousline)+3 << "," << stoi(line)-3 << "p' " << arg2  << endl;
            }
            previousline = line;
        }
        inputFile.close();
        outputFile.close();
    } else {
        std::cout << "Unable to open file";
    }
    exit(0);
}