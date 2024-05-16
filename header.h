#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>

void readFromFile();

void send(int writeFd, char *stringToSend);

int testaDigito(char *baseWord, char *testWord, char readChar);
void trataPalavra(char *word, char *returnWord);