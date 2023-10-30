#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>

using namespace std;
namespace fs = std::filesystem;

bool shouldIgnoreFile(const string &filename) {
    return filename == "node_modules" || filename == ".DS_Store";
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

void processDirectoryForSaving(const string &directoryPath, ofstream &outputFile);

void displayFileContentToStream(const string &path, ofstream &outputFile) {
    ifstream file(path);
    if (file.is_open()) {
        string line;
        outputFile << path << ":\n";
        while (getline(file, line)) {
            outputFile << line << '\n';
        }
        outputFile << "----------------------------------------\n";
        file.close();
    } else {
        outputFile << "Unable to open file: " << path << '\n';
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
        processDirectoryForSaving(directoryPath, outputFile);
        outputFile.close();
        cout << "The file " << filename << " has been saved in the directory: " << directoryPath << endl;
    } else {
        cout << "Unable to create/open the file for writing." << endl;
    }
}

void processDirectoryForSaving(const string &directoryPath, ofstream &outputFile) {
    for (const auto &entry : fs::directory_iterator(directoryPath)) {
        string filename = entry.path().filename();
        if (entry.is_regular_file() && !shouldIgnoreFile(filename)) {
            displayFileContentToStream(entry.path(), outputFile);
        } else if (entry.is_directory() && filename != "node_modules" && filename != "UNIT") {
            outputFile << "Directory: " << entry.path() << "\n";
            processDirectoryForSaving(entry.path(), outputFile); // Recursively explore directories
        }
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
