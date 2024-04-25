# Bankers Algorithm

## Description
This program was created to demonstrate using Banker's Algorithm to determine if a Program Snapshot is safe. This emulates the process that an Operated System kernel uses to detects deadlocks--where there isn't enough processing resources for the number of queued processes.
This program also demonstrates using forks and pipes to create a subprocess that will communicate with the main process to complete the program's operations.
The program first reads a Program Snapshot file which is read from a plain text file. Then, it will send the information to a child process that will find a safe sequence, if there is any, and send it back to the parent process.

## Problem
The number of resources on an Operating System is fundamentally limited, and there could be any number of processes waiting to run. How does the Operating System decide which processes to run first?
The program could solve this by using Banker's Algorithm, by finding the a process that could be finished first, therefore releasing resources for other processes.
If no such sequence of processes can be found, the program is in a deadlock which must be resolved by the kernel.

## Running
Install make on a Linux machine
Then run the following commands to execute the program:
> make bankers
> ./bankers.out

If you want, you can create your own Program Snapshot, following the format of the provided text files.
The program will throw errors if the snapshot is invalid!

## Credits
Julian Matuszewski    
Email: julianmatuszewski@csu.fullerton.edu
