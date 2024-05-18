#include "header.h"

#define MAXBUFF 15
#define FILE_PATH "roteiroShrek.txt"

int bytesInFile = 0;
unsigned int bytesRead = 0;
char buffer[MAXBUFF] = "";
int	pidPai;

void recv_sig(int sig) // funcionamento do signal (tratamento de sinais)
{
}

int	main(void) {
	
	int descritor,
		pipe1[2];  

	if (pipe(pipe1) < 0) {
		printf("We can't exceute pipe\n");
		exit(0);
	}
	pidPai = getpid();

	if ((descritor = fork()) < 0) {
		printf("We can't execute FORK\n");
		exit(0);
	}
	else if (descritor > 0) { // PROCESSO PAI
		close(pipe1[0]);
		while (1) {
			
			readFromFile(FILE_PATH);
			send(pipe1[1], buffer);
			signal(SIGUSR1, recv_sig);
			pause();
		}
		close(pipe1[1]); 
	}                    
	else {                 // PROCESSO FILHO
		close(pipe1[1]); 
        char buff[16] = "";
	    char returnBuff[15] = "";
      
		preparing_terminal();
		pthread_t thread1;
		pthread_t thread2;
		Input_handle_arguments input_args;

		input_args.pipe = pipe1[0];
		strcmp(input_args.buff, buff);
		
		pthread_create(&thread1, NULL, input_handle, &input_args);
		// pthread_create(&thread2, NULL, graphical_handle, NULL);
		
		pthread_join(thread1, NULL);
		// pthread_join(thread2, NULL);

	}                    
}
void *input_handle(void *arg){
	Input_handle_arguments *args = (Input_handle_arguments *)arg;
	int pipe1 = args->pipe;
	char buff[15];
	strcpy(buff,args->buff);
	char returnBuff[15];
	strcpy(buff,args->returnBuff);
	while(1){
			read(pipe1, buff, 15);
			trataPalavra(buff, returnBuff);

            strcpy(buff, returnBuff);
		    printf("%s\n", returnBuff);
			char c = 's';
		
            while(testaDigito(buff, returnBuff, (c = getchar())))
			 {
				printf("\e[1;1H\e[2J"); 
				printf("%s\n", returnBuff);
			 }
			
			kill(pidPai, SIGUSR1); // acorda pai
	}
	close(pipe1); 

	printf("INPUT HANDLE STARTS");
}
void *graphical_handle(void *arg){
	while(1){

	}
	printf("Igraphical_handle STARTS");
}

void readFromFile(void) {
	FILE	*pFile;

	pFile = fopen(FILE_PATH, "r");
	if (pFile == NULL)
	{
		printf("We can't open the file: %s\n", FILE_PATH);
		exit(0);
	}
	if (bytesInFile == 0)
	{
		while (fgets(buffer, 15, pFile) != NULL)
		{
			bytesInFile += strlen(buffer);
		}
	}
	if (bytesRead < bytesInFile)
	{
		fseek(pFile, bytesRead, 0);
		fgets(buffer, 15, pFile);
		bytesRead += strlen(buffer);
	}
	fclose(pFile);
}

void send(int writeFd, char *stringToSend) {
	write(writeFd, stringToSend, strlen(stringToSend));
}

void trataPalavra(char *word, char *returnWord) {
	for (int i = 0; i < strlen(word); i++)
	{
		if (word[i] == '\n')
		{
			word[i] = '\0';
			returnWord[i] = '\0';
			break ;
		}
		word[i] = toupper(word[i]);
		returnWord[i] = toupper(word[i]);
	}
}

int	testaDigito(char *baseWord, char *testWord, char readChar) {
	
	if (strlen(baseWord) == 0)
	{
		return (0);
	}
	if (baseWord[0] == toupper(readChar))
	{
		printf("%c", readChar);
		testWord[(strlen(testWord) - 1) - (strlen(baseWord) - 1)] = '*';
		
		for (int i = 0; i < strlen(baseWord); i++)
		{
			baseWord[i] = baseWord[i + 1];
		}
		if (testWord[strlen(testWord) -2] == '*') { printf("\e[1;1H\e[2J"); return(0);}
	}
	return (1);
}

Queue	*createQueue(void) {
	Queue	*temp;

	// Inicializa fila vazia
	temp = (Queue *)malloc(sizeof(Queue));
	temp->first = temp->last = NULL;
	return (temp);
}

void	insert(Queue *q, char *input_word) {
	Node	*temp;

	temp = (Node *)malloc(sizeof(Node));
	memset(temp->word, '\0', sizeof(temp->word));
	strcpy(temp->word, input_word);
	temp->next = NULL;
	if (q->last == NULL)
	{
		// Primeiro input
		q->first = q->last = temp;
		return ;
	}
	q->last->next = temp;
	q->last = temp;
}

char *return_new_word(Queue *q) {
	if (q->first == NULL)
		return (NULL);

	char *returnWord = (char *)malloc(16 * sizeof(char));
	memset(returnWord, '\0', 16);

	Node *temp = q->first;
	strncpy(returnWord, temp->word, 15);
	returnWord[15] = '\0';

	q->first = q->first->next;
	if (q->first == NULL)
		q->last = NULL;

	free(temp);

	return (returnWord);
}

int countQueue(Queue *q) {
    if (q->first == NULL)
        return 0;
    int count = 1;
    while(q->first->next != NULL)
        count++;
    return count;
}

void preparing_terminal(){
	tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios;

	new_termios.c_lflag &= ~(ICANON | ECHO);
	new_termios.c_cc[VMIN] = 1;
	new_termios.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
	printf("\e[?25l");
	atexit(reset_terminal);
}

void reset_terminal(){
	printf("\e[m");
	printf("\e[?25h");
	fflush(stdout);
	tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

}