#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <set> // Include set for line tracking

using namespace std;
namespace fs = std::filesystem;

bool shouldIgnoreFile(const string &filename) {
    return filename == "node_modules" || filename == ".DS_Store" || filename == "UNIT" || filename == "package-lock.json" || filename == "tsconfig.json";
}

void displayFileContent(const string &path) {
    ifstream file(path);
    if (file.is_open()) {
        string line;
        cout << path << ":\n";
        while (getline(file, line)) {
            cout << line << '\n';
        }
        cout << "----------------------------------------\n";
        file.close();
    } else {
        cout << "Unable to open file: " << path << '\n';
    }
}

void processDirectoryForSaving(const string &directoryPath, ofstream &outputFile, set<string> &encounteredLines) {
    for (const auto &entry : fs::directory_iterator(directoryPath)) {
        string filename = entry.path().filename();
        if (entry.is_regular_file() && !shouldIgnoreFile(filename)) {
            ifstream file(entry.path());
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    // Check for duplicates
                    if (encounteredLines.find(line) == encounteredLines.end()) {
                        outputFile << line << '\n';
                        encounteredLines.insert(line);
                    }
                }
                file.close();
            } else {
                outputFile << "Unable to open file: " << entry.path() << '\n';
            }
        } else if (entry.is_directory() && filename != "node_modules" && filename != "UNIT") {
            outputFile << "Directory: " << entry.path() << "\n";
            processDirectoryForSaving(entry.path(), outputFile, encounteredLines); // Recursively explore directories
        }
    }
}

void saveContentToFile(const string &directoryPath) {
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm current_time = *localtime(&now);

    string filename = "prompt_" + to_string(current_time.tm_year + 1900) + "-" +
                      to_string(current_time.tm_mon + 1) + "-" +
                      to_string(current_time.tm_mday) + "_" +
                      to_string(current_time.tm_hour) + ":" +
                      to_string(current_time.tm_min) + ":" +
                      to_string(current_time.tm_sec) + ".txt";

    string filePath = directoryPath + "/" + filename;

    ofstream outputFile(filePath);
    if (outputFile.is_open()) {
        outputFile << "Directory: " << directoryPath << "\n";
        set<string> encounteredLines; // Set to track encountered lines
        processDirectoryForSaving(directoryPath, outputFile, encounteredLines);
        outputFile.close();
        cout << "The file " << filename << " has been saved in the directory: " << directoryPath << endl;
    } else {
        cout << "Unable to create/open the file for writing." << endl;
    }
}

int main() {
    string directoryPath;
    cout << "Enter the directory path: ";
    getline(cin, directoryPath);

    if (fs::is_directory(directoryPath)) {
        saveContentToFile(directoryPath);
    } else {
        cout << "The given path is not a directory.\n";
    }

    return 0;
}
