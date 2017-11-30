#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
typedef enum { false, true } bool;
#define STATUS_SUCCESS 0
#define BUFFER_SIZE 64
int fd;
unsigned long number;

//Opdracht functies
bool isPrime(unsigned long prime){
    if (prime==2)                                                       //If the input number equals 2, it is a prime number
        return true;                                                    //So return true

    if (prime%2==0 || prime == 1)                                       //When dividable by 2 or input is 1
        return false;                                                   //So return false

    for (int z=3;z<=sqrt(prime);z+=2){                                  //For z=3 to square of prime
        sleep(0);                                                       //Delay so the processor doesnt finish too quickly
        if (prime%z==0)                                                 //Condition for a non prime
            return false;                                               //Return false
    }

    return true;                                                        //Return true
}
void wrapper(int pipeToChild, int pipeFromChild){
    char buffer[BUFFER_SIZE];                                           //Create a buffer of BUFFER_SIZE
    while(true){                                                        //Infinite loop
        read(pipeToChild, buffer, sizeof(buffer));                      //Read whatever is passed into here
        if(strstr(buffer, "bye"))                                       //Check if bye exists in this situation
        {
            exit(STATUS_SUCCESS);                                       //If so, exit with STATUS_SUCCESS
        }
        else                                                            //Process number and send back wether prime or not
        {
            number = strtol(buffer, NULL, 10);                          //Convert string to unsigned long
            bool prime = isPrime(number);                               //Pass it into the isPrime function and get the return result (true or false)
            if(prime)                                                   //If true
            {
                write(pipeFromChild, "1", sizeof(char));                //Write "1" to the buffer
            }
            else                                                        //If false
            {
                write(pipeFromChild, "0", sizeof(char));                //Write "0" to the buffer
            }
        }
    }
}
