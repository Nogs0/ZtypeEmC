#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>
#include <pthread.h>
#include <sys/ioctl.h>

#define MAXBUFF 15
#define FILE_PATH "roteiroShrek.txt"
#define DIFICULT_1 200000
#define DIFICULT_2 100000
#define DIFICULT_3 80000
#define DIFICULT_4 60000
#define DIFICULT_5 40000
#define DIFICULT_6 30000
#define DIFICULT_7 20000
#define DIFICULT_8 10000
// ******* Queue functions and typedefs. *********
// typedef struct Node{
//     char word[15];
//     struct Node *next;
// }Node;

// typedef struct Queue{
//     Node *first;
//     Node *last;
// }Queue;

// // Retorna um ponteiro para o primeiro caractere da primeira string da fila.
// char *return_new_word(Queue *); 
// // Insere na fila
// void insert(Queue *, char*);    
// // Cria uma fila vazia
// Queue *createQueue();
// // Conta quantos tem na fila  
// int countQueue(Queue *);        

// **********************************************

// ************** File functions ****************

// Lê do txt e armazena na global "buffer"
void readFromFile(); 
// Envia uma string pelo pipe
void send(int writeFd, char *stringToSend);
// Testa se o que foi escrito é válido e se for altera a letra para "*"
int verifyDigit(char *baseWord, char *testWord, char readChar);
// Copia o que está no pipe (word) para o que vai printar na tela (returnWord), colocando em uppercase.
void verifyWord(char *word, char *returnWord);

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
void *watch_terminal_size(void *arg);
int return_next_color(int total_rows, int current_row);
int config_next_color(int total_rows, int current_row, char *color);
int return_current_color(int terco, int current_row);
void print_header(int lifes, int correctWords, int level, unsigned long int dificult, int wordsToChangeDificult, char *color);
int getChangeDificult(int level, int wordsToChange);
unsigned long int getDificult(int level);

typedef struct{
    int pipe;
} Input_handle_arguments;

typedef struct{
    char *wordPointer;
    pthread_mutex_t *mutex;
    int *lifes;
    int *correctWords;
    int *level;
    unsigned long int *dificult;
    int *wordsToChangeDificult;
}Graphical_handle_arguments;

typedef struct{
    int *rows;
    int *cols;
    char *wordPointer;
    int *lifes;
    int *correctWords;
    int *level;
    unsigned long int *dificult;
    int *wordsToChangeDificult;
}Fall_word_arguments;

typedef struct{
    int *rows;
    int *cols;
}Watch_terminal_arguments;
