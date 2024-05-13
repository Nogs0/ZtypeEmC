#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void readFromFile();

void send(int writeFd, char *stringToSend);