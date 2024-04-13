// main.cpp
#include "BankerAlgorithm.hpp"
#include "ProgramSnapshotReader.hpp"

#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <vector>

#define READ_END 0
#define WRITE_END 1

// Forking can be disabled to allow for easier debugging
#define ENABLE_FORKING true

using std::string, std::cout, std::endl;

// Send ProgramSnapshotReader Object over Pipe
void sendObjectOverPipe(int fd[2], ProgramSnapshotReader& obj) {
    std::vector<size_t> availableVector = obj.getAvailableResources();
    std::vector<std::vector<size_t>> maximumMatrix = obj.getMaximumMatrix();
    std::vector<std::vector<size_t>> allocationMatrix = obj.getAllocationMatrix();

    size_t row_counter = 0;
    size_t col_counter = 0;

    // Send Available Resources Vector
    // First, send the size of the vector
    size_t size_availableVector = availableVector.size();
    if (write(fd[WRITE_END], &size_availableVector, sizeof(size_availableVector)) != sizeof(size_availableVector)) {
        throw std::runtime_error("availableVector size write to pipe failed.");
    }
    // Next, send each value of the vector
    for (auto value: availableVector) {
        if (write(fd[WRITE_END], &value, sizeof(value)) != sizeof(value)) {
            throw std::runtime_error("availableVector write to pipe failed.");
        }
    }

    
    // Send Maximum Matrix
    // First, send the number of rows in the vector
    size_t size_maximumMatrix = maximumMatrix.size();
    if (write(fd[WRITE_END], &size_maximumMatrix, sizeof(size_maximumMatrix)) != sizeof(size_maximumMatrix)) {
        throw std::runtime_error("maximumMatrix number of rows write to pipe failed.");
    }

    for (auto row: maximumMatrix) {
        // Next, send the size of the row
        size_t row_size = row.size();
        if (write(fd[WRITE_END], &row_size, sizeof(row_size)) != sizeof(row_size)) {
            throw std::runtime_error(row_counter + "th row of Maximum Matrix size write to pipe failed.");
        }
        
        // Finally, send the row's contents
        col_counter = 0;
        for (auto value: row) {
            if(write(fd[WRITE_END], &value, sizeof(value)) != sizeof(value)) {
                throw std::runtime_error(
                    std::to_string(col_counter) + "th element of " + std::to_string(row_counter) + 
                    "th row of Maximum Matrix write to pipe failed.");
            }

            col_counter++;
        }
        row_counter++;
    }


    // Send Allocation Matrix
    // First, send the number of rows in the vector
    size_t size_allocationMatrix = allocationMatrix.size();
    if (write(fd[WRITE_END], &size_allocationMatrix, sizeof(size_allocationMatrix)) != sizeof(size_allocationMatrix)) {
        throw std::runtime_error("allocationMatrix number of rows write to pipe failed.");
    }

    for (auto row: allocationMatrix) {
        // Next, send the size of the row
        size_t row_size = row.size();
        if (write(fd[WRITE_END], &row_size, sizeof(row_size)) != sizeof(row_size)) {
            throw std::runtime_error(row_counter + "th row of Allocation Matrix size write to pipe failed.");
        }
        
        // Finally, send the row's contents
        col_counter = 0;
        for (auto value: row) {
            if(write(fd[WRITE_END], &value, sizeof(value)) != sizeof(value)) {
                throw std::runtime_error(
                    std::to_string(col_counter) + "th element of " + std::to_string(row_counter) + 
                    "th row of Allocation Matrix write to pipe failed.");
            }

            col_counter++;
        }
        row_counter++;
    }

    return;
}

// Receive ProgramSnapshotReader Object from Pipe
void receiveObjectOverPipe(int fd[2], ProgramSnapshotReader& obj) {
    std::vector<size_t> availableVector;
    std::vector<std::vector<size_t>> maximumMatrix;
    std::vector<std::vector<size_t>> allocationMatrix;

    // counters used for throwing errors
    size_t row_counter = 0;
    size_t col_counter = 0;

    // Receive available resources vector
    // First, receive the size of the vector
    size_t size_availableVector;
    if (read(fd[READ_END], &size_availableVector, sizeof(size_availableVector)) != sizeof(size_availableVector)) {
        throw std::runtime_error("availableVector size read from pipe failed.");
    }
    availableVector = vector<size_t>(size_availableVector, 0);
    // Next, receive each value of the vector
    for (auto &value: availableVector) {
        if (read(fd[READ_END], &value, sizeof(value)) != sizeof(value)) {
            throw std::runtime_error("availableVector read from pipe failed.");
        }
    }

    // Receive Maximum Matrix
    // First, receive the number of rows in the vector
    size_t size_maximumMatrix;
    if (read(fd[READ_END], &size_maximumMatrix, sizeof(size_maximumMatrix)) != sizeof(size_maximumMatrix)) {
        throw std::runtime_error("maximumMatrix number of rows read from pipe failed.");
    }
    maximumMatrix = std::vector<std::vector<size_t>>(size_maximumMatrix);

    for (auto& row: maximumMatrix) {
        // Next, receive the size of the row
        size_t row_size;
        if (read(fd[READ_END], &row_size, sizeof(row_size)) != sizeof(row_size)) {
            throw std::runtime_error(std::to_string(row_counter) + "th row of Maximum Matrix size read from pipe failed.");
        }
        row = std::vector<size_t>(row_size, 0);

        // Finally, receive the row's contents
        col_counter = 0;
        for (auto& value: row) {
            if(read(fd[READ_END], &value, sizeof(value)) != sizeof(value)) {
                throw std::runtime_error(
                    std::to_string(col_counter) + "th element of " + std::to_string(row_counter) + 
                    "th row of Maximum Matrix read from pipe failed.");
            }

            col_counter++;
        }
        row_counter++;
    }


    // Receive Allocation Matrix
    // First, receive the number of rows in the vector
    size_t size_allocationMatrix;
    if (read(fd[READ_END], &size_allocationMatrix, sizeof(size_allocationMatrix)) != sizeof(size_allocationMatrix)) {
        throw std::runtime_error("allocationMatrix number of rows read from pipe failed.");
    }
    allocationMatrix = std::vector<std::vector<size_t>>(size_allocationMatrix);

    for (auto& row: allocationMatrix) {
        // Next, receive the size of the row
        size_t row_size;
        if (read(fd[READ_END], &row_size, sizeof(row_size)) != sizeof(row_size)) {
            throw std::runtime_error(std::to_string(row_counter) + "th row of Allocation Matrix size read from pipe failed.");
        }
        row = std::vector<size_t>(row_size, 0);

        // Finally, receive the row's contents
        col_counter = 0;
        for (auto& value: row) {
            if(read(fd[READ_END], &value, sizeof(value)) != sizeof(value)) {
                throw std::runtime_error(
                    std::to_string(col_counter) + "th element of " + std::to_string(row_counter) + 
                    "th row of Allocation Matrix read from pipe failed.");
            }

            col_counter++;
        }
        row_counter++;
    }

    // Set the passed object's value to the ones read from fd
    obj.setAvailableResources(availableVector);
    obj.setMaximumMatrix(maximumMatrix);
    obj.setallocationMatrix(allocationMatrix);

    return;
}

// Send String over Pipe
void sendStringOverPipe(int fd[2], const string& message) {
    const char* msg = message.c_str();

    // First, send length of string to pipe
    size_t msglen = strlen(msg) + 1;
    if (write(fd[WRITE_END], &msglen, sizeof(msglen)) != sizeof(msglen)) {
        throw std::runtime_error("String length write to pipe failed.");
    }

    // Next, send the string's raw value
    if (write(fd[WRITE_END], msg, msglen) != msglen){
        throw std::runtime_error("String write to pipe failed.");
    }
}

// Receive String from Pipe
string receiveStringOverPipe(int fd[2]) {
    // First, receive length of string from pipe
    size_t msglen;
    if (read(fd[READ_END], &msglen, sizeof(msglen)) != sizeof(msglen)) {
        throw std::runtime_error("String length read from pipe failed.");
    }
    char* msg = new char[msglen];

    // Next, receive string from pipe
    if (read(fd[READ_END], msg, msglen) != msglen) {
        throw std::runtime_error("String read from pipe failed.");
    }

    // return C++ string representation of c string
    string out = string(msg);
    delete[] msg;
    return out;
}

// Formats a vector into an ordered list of numbers and returns string
// Ex: (1, 2, 3, 4, 5)
string vectorToString (const vector<size_t>& input) {
    string out = "(";
    for (auto i = 0; i < input.size(); i++) {
        out += std::to_string(input[i]);
        if (i < input.size() - 1) {
            out += ", ";
        }
    }
    out += ")";

    return out;
}

int main(int argc, char *argv[])
{
    const string default_file_name = "sample.txt";
    string file_name;

    // Extract file name from executable arguments
    // If there are none, use the default file name
    if (argc < 2) {
        file_name = default_file_name;
        cout << "No file name arguments given, defaulting to " << file_name << endl;
    } else {
        file_name = argv[1];
        cout << file_name << " has been loaded and will be analyzed" << endl;
    }
    
    // Set up two pipes, one for child to parent and one for parent to child
    int childToParentFD[2], parentToChildFD[2];
    if (pipe(childToParentFD) == -1 || pipe(parentToChildFD) == -1) {
        throw std::runtime_error("Pipe Creation Failed");
    }

    // Fork program
    pid_t cpid;
    if (ENABLE_FORKING) {
        cpid = fork();
        if (cpid == -1) {
            throw std::runtime_error("Fork Failed");
        }
    }

    // Child process is in charge of reading piped data from parent and invoking Banker's Algorithm
    // to determine if the current state is safe. Send safe sequence back to parent or notify that there is a deadlock in the Program Snapshot.
    if (cpid == 0) {
        close(childToParentFD[READ_END]);       // close unused read end
        close(parentToChildFD[WRITE_END]);      // close unused write end
        
        if (ENABLE_FORKING) {
            // Create a ProgramSnapshotReader object to accept piped data from parent process
            ProgramSnapshotReader reader;
            receiveObjectOverPipe(parentToChildFD, reader);

            // Initialize BankerAlgorithm Object and perform Banker's Algorithm
            BankerAlgorithm bankers(reader.getAvailableResources(), reader.getMaximumMatrix(), reader.getAllocationMatrix());
            auto safeSequence = bankers.findSafeSequence();

            // Send safe sequence to Parent as a string or notify that there is a deadlock
            if (safeSequence.empty()) {
                sendStringOverPipe(childToParentFD, "Oops! Looks like we're stuck in a deadlock!");
            } else {
                sendStringOverPipe(childToParentFD, vectorToString(safeSequence));
            }
        }

        // Close read and write ends after Child is finished using them
        close(childToParentFD[WRITE_END]);
        close(parentToChildFD[READ_END]);
        
        // Exit child before parent
        return 0;
    }

    // Parent process is in charge of reading data from text file and piping it to child
    // Once received safe sequence from child, print it out
    else {
        close(childToParentFD[WRITE_END]);      // close unused write end
        close(parentToChildFD[READ_END]);       // close unused read end

        if (ENABLE_FORKING) {
            // Create a Program Snapshot Reader Object and send to Child over pipe
            ProgramSnapshotReader reader(file_name);
            sendObjectOverPipe(parentToChildFD, reader);

            // Receive Child's safe sequence or string message
            string message = receiveStringOverPipe(childToParentFD);

            // Close read and write ends after Parent is finished using them
            close(parentToChildFD[WRITE_END]);
            close(childToParentFD[READ_END]);

            // Print string received by Parent from Child
            cout << "Parent Process Received: " << message << endl;
        } else {
            // If Forking is disabled, perform Banker's Algorithm in Parent Process
            ProgramSnapshotReader reader(file_name);
            BankerAlgorithm bankers(reader.getAvailableResources(), reader.getMaximumMatrix(), reader.getAllocationMatrix());
            auto safeSequence = bankers.findSafeSequence();
            cout << "Threading Disabled, Parent performing Banker's Algorithm" << endl;
            if (safeSequence.empty()) {
                cout << "Oops! Looks like we're stuck in a deadlock!" << endl;
            } else {
                cout << "Parent Process found safe sequence: " << vectorToString(safeSequence) << endl;
            }
        }

        // Exit once Child has exited
        wait(NULL);
        return 0;
    }
}