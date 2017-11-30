#ifndef API_H
#define API_H
typedef enum { false, true } bool;
void wrapper(int pipeToChild, int pipeFromChild);
bool isPrime(unsigned long prime);
#endif
