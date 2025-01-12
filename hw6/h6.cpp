#include <iostream>
#include <fstream>
#include <string>
#include <tbb/tbb.h>

int main() {
    std::ifstream file("random.txt");
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }
 
    file.close();

    tbb::parallel_for_each(lines.begin(), lines.end(), [](std::string& line) {
        tbb::parallel_for_each(line.begin(), line.end(), [](char& ch) {
            ch = toupper(ch);
            });
    });

    for (int i{ 0 }; i < lines.size(); i++) {
        std::cout << lines[i] << std::endl;
    }
    return 0;
}