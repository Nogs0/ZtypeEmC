#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>
#include <pthread.h>

void readFromFile();

void send(int writeFd, char *stringToSend);

int testaDigito(char *baseWord, char *testWord, char readChar);
void trataPalavra(char *word, char *returnWord);

typedef struct Node{
    char word[15];
    struct Node *next;
}Node;

typedef struct Queue{
    Node *first;
    Node *last;
}Queue;

void readFromFile();
void send(int writeFd, char *stringToSend);
char *return_new_word(Queue *);
void insert(Queue *, char*);
Queue *createQueue();

int countQueue(Queue *);