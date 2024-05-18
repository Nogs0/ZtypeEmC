#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>
#include <pthread.h>

// ******* Queue functions and typedefs. *********
typedef struct Node{
    char word[15];
    struct Node *next;
}Node;

typedef struct Queue{
    Node *first;
    Node *last;
}Queue;

// Retorna um ponteiro para o primeiro caractere da primeira string da fila.
char *return_new_word(Queue *); 
// Insere na fila
void insert(Queue *, char*);    
// Cria uma fila vazia
Queue *createQueue();
// Conta quantos tem na fila  
int countQueue(Queue *);        

// **********************************************

// ************** File functions ****************

// Lê do txt e armazena na global "buffer"
void readFromFile(); 
// Envia uma string pelo pipe
void send(int writeFd, char *stringToSend);
// Testa se o que foi escrito é válido e se for altera a letra para "*"
int testaDigito(char *baseWord, char *testWord, char readChar);
// Copia o que está no pipe (word) para o que vai printar na tela (returnWord), colocando em uppercase.
void trataPalavra(char *word, char *returnWord);

// **********************************************

// ************ Terminal functions **************

// Structs definidas em <termios.h>, servem para guardar o estado no terminal.
static struct termios old_termios;
static struct termios new_termios;
// Reseta o terminal para o estado salvo em old_termios
void reset_terminal();
// Desativa o modo canonico, desativa o ECHO e deixa o cursor invisível.
void preparing_terminal();

// **********************************************
void *input_handle(void *arg);
void *graphical_handle(void *arg);
void *fall_word(void *arg);
void clear_screen();
typedef struct{
    int pipe;
    char buff[15];
    char returnBuff[15];
} Input_handle_arguments;

typedef struct{
    char *wordPointer;
    pthread_mutex_t *mutex;
}Graphical_handle_arguments;

typedef struct{
    char *wordPointer;
}Fall_word_arguments;