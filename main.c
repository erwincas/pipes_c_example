#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include "api.h"
#define NUM_THREADS 3
#define BUFFER_SIZE 64
#define WRITE 1
#define READ 0
typedef struct child{
    pid_t pidnumber;
    int pipeToChild[2];
    int pipeFromChild[2];
    int argvAttached;
} child;
int main(int argc, char **argv)
{
    argc--; 																											//Take 1 off argc as the main program argument doesnt count
    child childs[NUM_THREADS];																							//Make a struct array of strruct child with the size of NUM_THREADS
    int i, z, status, remaining;																						//Declare a few int variables
    pid_t pid[NUM_THREADS], currentpid, parentPID, startpid;															//Declare a few pid_t variables
    char buffer[BUFFER_SIZE];																							//Declare a buffer of chars with a size of BUFFER_SIZE
    bool prime;																											//Declare a boolean for the check of prime
    if(argc == 0){ 																										//If no arguments were specified there is no use running this program
        fprintf(stdout, "Please input at least one number as an argument\n");											//So dont run it
    }
    else																												//Else, go ahead
    {
        parentPID = getppid();																							//Get the parentPID (purely for debugging purposes)
        startpid = getpid();																							//Get the startPID (purely for debugging purposes)
        fprintf(stdout, "(PID: %d) I am the starting process!\n", startpid);											//Print the starting PID (purely for debugging purposes)
        for(i = 0; i<NUM_THREADS; i++){																					//Go through a loop that creates NUM_THREADS threads

            pipe(childs[i].pipeToChild);																				//Generate a pipe to the child
            pipe(childs[i].pipeFromChild);																				//Generate a pipe from the child

            pid[i] = fork();																							//Generate a fork for each loop

            if(pid[i] == -1)																							//Status -1 indicates an error
            {
                perror("fork");																							//So display that error
                exit(1);																								//And exit the program
            }

            if(pid[i] == 0)																								//Status 0 indicates the child process
            {
                close(childs[i].pipeToChild[WRITE]); 																	//Close the output pipe, because the child only uses the input (READ) on this pipe
                close(childs[i].pipeFromChild[READ]); 																	//Close input pipe, because the child only uses the output (WRITE) on this pipe

                wrapper(childs[i].pipeToChild[READ], childs[i].pipeFromChild[WRITE]); 									//Call wrapper and pass the pipeToChild and pipeFromChild variables
            }

            else 																										//Status bigger than 0 indicates the parent process
            {
                close(childs[i].pipeToChild[READ]); 																	//Close the input pipe, because the parent only uses the output (WRITE) on this pipe
                close(childs[i].pipeFromChild[WRITE]);																	//Close the output pipe, because the parent only uses the input (READ) on this pipe

                childs[i].pidnumber = pid[i]; 																			//Save process id from child with correct pipe descriptors
                fprintf(stdout, "Parent (PID: %d) created 2 pipes for child with PID %d\n", parentPID, pid[i]);			//Give the user a message that the pipes and child have been produced (purely for debugging purposes)
            }


        }
        i = 1;																											//Set i to 1 (Because the argv array is only useful from position 1 and on)
        remaining = argc;																								//Count back the remaining numbers. So set remaining to argc
        while(remaining > 0){																							//Loop while remaining is bigger than 0
            for(z = 0; z<NUM_THREADS; i++){																				//First loop NUM_THREADS time
            	if(remaining > 0){ 																						//Dont write more than needed. If remaining is bigger than 0
                write(childs[z].pipeToChild[WRITE], argv[i], sizeof(buffer));											//Write the number to child number z via pipeToChild
                childs[z].argvAttached = i;																				//Also attach the argv number to the child struct (This saves memory, instead of copying the whole number into the struct)
            	}
            	else{																									//Else if the remaining is 0
            		break;																								//Break, this loop is no longer needed
            	}
                z++;																									//Increment z at the end of the write loop, because if incremented at the start of the loop it would skip one thread
            }

            for(z = 0; z<NUM_THREADS;){																					//After the write loop it is time to read
            	remaining--;																							//Decrement the remaining numbers
                read(childs[z].pipeFromChild[READ], buffer, sizeof(buffer));											//Read from the pipeFromChild
                prime = atoi(buffer);																					//Convert buffer ("1" or "0") to an integer (1 or 0)
                if(prime){																								//If the number returned is a prime (true)
                    fprintf(stdout, "(Result from PID: %d) Number %s \tis \ta prime number\n", childs[z].pidnumber , argv[childs[z].argvAttached]);			//Print the results indicating it is a prime
                }
                else{																									//If the number returned is not a prime (false)
                    fprintf(stdout, "(Result from PID: %d) Number %s \tis not \ta prime number\n", childs[z].pidnumber, argv[childs[z].argvAttached]);		//Print the results indicating it is not a prime
                }

                if(remaining == 0){ 																					//If remaining == 0, break the loop.
            		break;																								//If true break the loop, after this the while loop recognizes that the remaining is 0 and the end has been reached
            	}
                z++;																									//Increment z at each read loop
            }
        }
        for(i = 0; i<NUM_THREADS; i++){																					//Loop NUM_THREADS
            write(childs[i].pipeToChild[WRITE], "bye", sizeof("bye"));													//Write "bye" to all of the childs to terminate them
            currentpid = waitpid(-1, &status, 0); 																		//Wait for a pid to finish
            fprintf(stdout, "Child with pid %d closed\n", currentpid);													//Print that the child has closed (purely for debugging purposes)
            close(childs[i].pipeToChild[1]); 																			//Close WRITE pipe of pipeToChild
            close(childs[i].pipeFromChild[0]);																			//Close READ pipe of pipeFromChild
        }
    }
    return 0;
}

