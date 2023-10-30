#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

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

void displayFilesAndContent(const string &path) {
    for (const auto &entry : fs::directory_iterator(path)) {
        string filename = entry.path().filename();
        if (entry.is_regular_file() && !shouldIgnoreFile(filename)) {
            displayFileContent(entry.path());
        } else if (entry.is_directory() && filename != "node_modules" && filename != "UNIT") {
            cout << "Directory: " << entry.path() << "\n";
            displayFilesAndContent(entry.path()); // Recursively explore directories
        }
    }
}

int main() {
    string directoryPath;
    cout << "Enter the directory path: ";
    getline(cin, directoryPath);

    if (fs::is_directory(directoryPath)) {
        displayFilesAndContent(directoryPath);

        char saveToFile;
        cout << "Do you want to save the content to a file? (y/n): ";
        cin >> saveToFile;

        if (saveToFile == 'y' || saveToFile == 'Y') {
            auto now = chrono::system_clock::now();
            time_t now_c = chrono::system_clock::to_time_t(now);
            tm current_time = *localtime(&now_c);

            string filename = "promp_" + to_string(current_time.tm_year + 1900) + "-" +
                              to_string(current_time.tm_mon + 1) + "-" +
                              to_string(current_time.tm_mday) + "/" +
                              to_string(current_time.tm_hour) + ":" +
                              to_string(current_time.tm_min) + ":" +
                              to_string(current_time.tm_sec) + ".txt";

            string filePath = directoryPath + "/" + filename;

            ofstream outputFile(filePath);
            if (outputFile.is_open()) {
                outputFile << "Content from directory: " << directoryPath << "\n";
                outputFile << "Additional information, if any...\n";
                outputFile.close();
                cout << "The file " << filename << " has been saved in the directory: " << directoryPath << endl;
            } else {
                cout << "Unable to create/open the file for writing." << endl;
            }
        } else {
            cout << "Content will not be saved to a file." << endl;
        }
    } else {
        cout << "The given path is not a directory.\n";
    }

    return 0;
}
