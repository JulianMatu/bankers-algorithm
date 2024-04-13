// BankerAlgorithm.hpp
#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

#define RETURN_UNSAFE_SEQUENCES false

using std::vector, std::string;

// Encapsulates the functionality of Banker's Algorithm
// This object is designed to take information extracted from the text file by the Program Snashot Reader
// Then, it performs Banker's Algorithm to find a safe sequence
class BankerAlgorithm {
    public:
        // Construct Banker Algorithm, assigning the supplied availableVector, maxMatrix, and allocationMatrix
        BankerAlgorithm(vector<size_t> availableVector, vector<vector<size_t>> maxMatrix, vector<vector<size_t>> allocationMatrix) :
                        availableVector(availableVector), maxMatrix(maxMatrix), allocationMatrix(allocationMatrix){

            numProcesses = maxMatrix.size();
            numResources = maxMatrix[0].size();

            // Resize the needMatrix to the same length and width as the maxMatrix
            needMatrix = vector<vector<size_t>>(numProcesses, vector<size_t>(numResources));

            // Calculate need matrix from the maxMatrix - allocationMatrix
            calculateNeedMatrix();
        }

        vector<size_t> findSafeSequence() {
            // Backup availableVector before algorithm is applied
            const auto availableVectorCopy = availableVector;

            // Fake boolean used for an int-based boolean vector
            enum BOOL {
                FALSE,
                TRUE
            };

            // Vector that keeps track of a safe sequence
            vector<size_t> safeSequence;

            // Vector that keeps track of which processes are finished
            vector<BOOL> finished(numProcesses, FALSE);

            // Keep track of the number of processes finished in each cycle
            size_t changeInFinishedProcesses = 1;

            // Start the algorithm
            while (changeInFinishedProcesses > 0) {
                // At the start of each cycle, the change of finished processes is zero
                changeInFinishedProcesses = 0;

                // Iterate through each process FIFO
                for (auto i = 0; i < numProcesses; i++) {
                    if (finished[i] == FALSE) {
                        if (compareVector(needMatrix[i], availableVector)) {
                            finished[i] = TRUE;
                            safeSequence.push_back(i);
                            changeInFinishedProcesses++;

                            // Free the resources no longer used by the process that just finished
                            // This is represented by adding the allocated resources of the freed process to the available resources
                            freeResources(allocationMatrix[i]);
                        }
                    }
                }
            }

            // Restore availableVector from before algorithm
            availableVector = availableVectorCopy;

            // If there are still processes that did not complete, there is a deadlock
            // Return empty vector
            for (auto status: finished) {
                if (status == FALSE) {
                    if (RETURN_UNSAFE_SEQUENCES) {
                        return safeSequence;
                    }
                    return vector<size_t>();
                }
            }

            // If all processes are complete at the end of the algorithm, the Program Snapshot is safe
            // Return safe sequence
            return safeSequence;
        }

    private:
        // Need Matrix is calculated by finding the difference of each element of the same position in the
        // Max Matrix and Allocation Matrix (ie. maxMatrix(i, j) - allocationMatrix(i, j) for all i, j in range)
        void calculateNeedMatrix() {
            // Nested for loop to find the difference of each position in the matrices
            for (auto row = 0; row < maxMatrix.size() && row < allocationMatrix.size() && row < needMatrix.size(); row++) {
                for (auto col = 0; col < maxMatrix[row].size() && col < allocationMatrix[row].size() && col < needMatrix[row].size(); col++) {

                    // Difference must be calculated after casting to integers to preserve negative values
                    int difference = static_cast<int>(maxMatrix[row][col]) - static_cast<int>(allocationMatrix[row][col]);

                    // If the difference is negative, it indicates that there are more resources allocated than possible
                    // This means that the Program Snapshot provided by the text file is invalid
                    if (difference < 0) {
                        throw std::runtime_error(
                            "Invalid Program Snapshot, at row " + std::to_string(row) + " col " + std::to_string(col) +
                            ", the value of maxMatrix (" + std::to_string(maxMatrix[row][col]) +
                            ") is less than the value of allocationMatrix (" + std::to_string(allocationMatrix[row][col]) + ")");
                    }
                    needMatrix[row][col] = static_cast<size_t>(difference);
                }
            }
        }

        // Returns true if every element on the lhs is less than or equal to the rhs
        bool compareVector (const vector<size_t>& lhs, const vector<size_t>& rhs) const{
            if (lhs.size() != rhs.size()) {
                throw std::runtime_error("Attempted comparison of vectors of different lengths.");
            }

            // If just one element of the lhs is greater than another in the same position as the rhs, return false
            for (auto i = 0; i < lhs.size(); i++) {
                if (lhs[i] > rhs[i]) {
                    return false;
                }
            }

            return true;
        }

        // Adds elements from the allocationVector into availableVector
        void freeResources(const vector<size_t>& allocationVector) {
            for (auto i = 0; i < allocationVector.size(); i++) {
                availableVector[i] += allocationVector[i];
            }
        }
        
        // Datastructures used for calculating safe sequence
        vector<size_t> availableVector;
        vector<vector<size_t>> maxMatrix;
        vector<vector<size_t>> allocationMatrix;
        vector<vector<size_t>> needMatrix;

        // Other variables
        size_t numProcesses;
        size_t numResources;
};