// ProgramSnapshotReader
#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>

using std::vector, std::string;

// Reads provided program snapshots from file and populates datastructures based on text input
class ProgramSnapshotReader {
public:
    ProgramSnapshotReader() = default;

    // Creates a ProgramSnapshotReader Object and perform read file_name
    ProgramSnapshotReader(const string &file_name) {
        read(file_name);
    }

    // Read data from plain text file and extract Program Snapshot data and populate available, max, and allocation datastructures
    void read(const string &file_name) {
        // check if file was downloaded and exit if it cannot find it
        if (!std::filesystem::exists(file_name))
        {
            throw std::runtime_error("File Not Found, " + file_name);
        }
        read_from_file(file_name);
        checkValidity();
        return;
    }

    auto getAvailableResources() const{
        return availableVector;
    }

    auto getMaximumMatrix() const{
        return maxMatrix;
    }

    auto getAllocationMatrix() const{
        return allocationMatrix;
    }

    void setAvailableResources(auto availableVector) {
        this->availableVector = availableVector;
    }

    void setMaximumMatrix (auto maximumMatrix) {
        this->maxMatrix = maximumMatrix;
    }

    void setallocationMatrix (auto allocationMatrix) {
        this->allocationMatrix = allocationMatrix;
    }

private:
    // Reads file from filename path and populates data using it
    void read_from_file(const string &file_name)
    {
        std::ifstream readFile(file_name);

        enum linereaderState
        {
            NONE,
            ALLOCATION,
            MAXIMUM,
            AVAILABLE
        };

        linereaderState state = NONE;
        vector<string> lines = separateLines(readFile);

        for (const auto line : lines)
        {
            // skip excess whitespace
            if (line == "") 
            {
                // skip
            }

            // allocation matrix demarquated by "// Allocation Matrix"
            else if (line == "// Allocation Matrix")
            {
                state = ALLOCATION;
            }

            // max matrix demarquated by "// Maximum Matrix"
            else if (line == "// Maximum Matrix")
            {
                state = MAXIMUM;
            }

            // available resources demarquated by "// Available Resources"
            else if (line == "// Available Resources")
            {
                state = AVAILABLE;
            }

            else {
                // put numbers belonging to each matrix/vector into their corresponding data structure
                switch (state) {
                    case ALLOCATION:
                        allocationMatrix.push_back(separateNumbers(line));
                        break;
                    case MAXIMUM:
                        maxMatrix.push_back(separateNumbers(line));
                        break;
                    case AVAILABLE:
                        availableVector = separateNumbers(line);
                        break;
                    default:
                        throw std::runtime_error("Invalid File Format");
                        break;
                }
            }
            
        }
        return;
    }

    // separates lines (text separated by '\n') in istream into a vector of strings
    vector<string> separateLines(std::istream &textData) const {
        vector<string> result;
        string line;

        while (getline(textData, line)) {
            result.push_back(line);
        }

        return result;
    }

    // turns string with numbers separated by spaces into vector of numbers
    vector<size_t> separateNumbers(const string &input) const {
        vector<size_t> result;
        string token;
        int num;
        char delimiter = ' ';
        std::istringstream iss(input);

        while (getline(iss, token, delimiter)) {
            try {
                num = std::stoi(token);
                if (num < 0) {
                    throw std::runtime_error("Negative Value in Program Snapshot File.");
                }
                result.push_back(static_cast<size_t>(num));
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("Invalid argument when converting number character to integer");
            } catch (const std::out_of_range& e) {
                throw std::out_of_range("Out of range error when converting number character to integer");
            }
        }

        return result;
    }

    // Check if the table sizes are compatible
    // Returns false if there are any errors in the input data
    bool checkValidity() {
        size_t numCol;
        try {
            numCol = allocationMatrix.at(0).size();
        } catch (...) {
            throw std::runtime_error("No Allocation Data");
            return false;
        }
        
        // check if table is rectangular
        for (auto vec: maxMatrix) {
            if (vec.size() != numCol) {
                throw std::runtime_error("Maximum Matrix is Missing an Entry");
                return false;
            }
        }

        for (auto vec: allocationMatrix) {
            if (vec.size() != numCol) {
                throw std::runtime_error("Allocation Matrix is Missing an Entry");
                return false;
            }
        }

        if (allocationMatrix.size() != maxMatrix.size()) {
            throw std::runtime_error("Allocation Matrix and Maximum Matrix Processes Mismatched");
            return false;
        }

        // Check if available vector matches resources
        if (numCol != availableVector.size()) {
            throw std::runtime_error("Available Vector is Incomplete");
            return false;
        }

        return true;
    }

    // Program Snapshot datastructures
    vector<size_t> availableVector;
    vector<vector<size_t>> maxMatrix;
    vector<vector<size_t>> allocationMatrix;
};