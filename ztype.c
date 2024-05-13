#include "header.h"

#define MAXBUFF 15
#define FILE_PATH "roteiroShrek.txt"

int bytesInFile = 0;
unsigned int bytesRead = 0;
char buffer[MAXBUFF] = "";
 
typedef struct pipe1Data {
    char string[15];
    int pid;
} pipe1Data;

int main()
{
    int descritor, // usado para criar o processo filho pelo fork
        pipe1[2];  // comunicacao pai -> filho

    if (pipe(pipe1) < 0){
        printf("We can't exceute pipe\n");
        exit(0);
    }

    //   Fork para criar o processo filho
    if ((descritor = fork()) < 0){
        printf("We can't execute FORK\n");
        exit(0);
    } 
    else if (descritor > 0) // PROCESSO PAI
    {
        close(pipe1[0]); // fecha leitura no pipe1
        while(1){
            readFromFile(FILE_PATH);
            send(pipe1[1], buffer);
        }
        
        close(pipe1[1]); // fecha pipe1
        exit(0);

    } // FIM DO PROCESSO PAI
    else // PROCESSO FILHO
    {
        close(pipe1[1]); // fecha escrita no pipe1

        while(1){
        char buff[15] = "";
            read(pipe1[0], buff, 15);
            printf(" \n  Received <- %s", buff);
            sleep(2);
        }

        close(pipe1[0]); // fecha leitura no pipe1
        exit(0);

    } // FIM DO PROCESSO FILHO
}

void readFromFile() {
    FILE *pFile;
    pFile = fopen(FILE_PATH, "r");
    if (pFile == NULL){
        printf("We can't open the file: %s\n", FILE_PATH);
        exit(0);
    }

    if (bytesInFile == 0){
        while(fgets(buffer, 15, pFile) != NULL){
            bytesInFile += strlen(buffer);            
        }
    }
    
    if(bytesRead < bytesInFile){
        sleep(2);
        fseek(pFile, bytesRead, 0);
        fgets(buffer, 15, pFile);
        bytesRead += strlen(buffer);
    }
    
    fclose(pFile);
}

void send(int writeFd, char *stringToSend){
    write(writeFd, stringToSend, strlen(stringToSend));
}