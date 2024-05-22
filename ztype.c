#include "header.h"
int bytesInFile = 0;
unsigned int bytesRead = 0;
char buffer[MAXBUFF] = "";
int	pidPai;


void recv_sig(int sig) // funcionamento do signal (tratamento de sinais)
{
}

int	main(void) {
	
	int descritor, pipe1[2];  
	
	int descritor, pipe1[2];  

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
		preparing_terminal();
		pthread_t thread1;
		//pthread_t thread2;
		Input_handle_arguments input_args;
		input_args.pipe = pipe1[0];
		pthread_create(&thread1, NULL, input_handle, &input_args);
		// pthread_create(&thread2, NULL, graphical_handle, NULL);

		pthread_join(thread1, NULL);
		// pthread_join(thread2, NULL);

	}                    
}

void *input_handle(void *arg){
	char buff[15];
	char returnBuff[15] = "";
	
	int lifes = 5;
	int correctWords = 0;
	int level = 1;
	int wordsToChangeDificult = getChangeDificult(level, 0);
	unsigned long int dificult = DIFICULT_1;

	Input_handle_arguments *args = (Input_handle_arguments *)arg;
	int pipe1 = args->pipe;

	unsigned short int completed_word = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	printf("Endereco do mutex no input handle: %p\n", &mutex);
	pthread_t graphical_thread;

	Graphical_handle_arguments graphical_args;
	graphical_args.wordPointer = returnBuff;
	graphical_args.mutex = &mutex;
	graphical_args.lifes = &lifes;
	graphical_args.correctWords = &correctWords;
	graphical_args.dificult = &dificult;
	graphical_args.level = &level;
	graphical_args.wordsToChangeDificult = &wordsToChangeDificult;

	pthread_create(&graphical_thread, NULL, graphical_handle, &graphical_args);
	
	while(1){
		read(pipe1, buff, 15);
		verifyWord(buff, returnBuff);
		strcpy(buff, returnBuff);

		pthread_mutex_lock(&mutex);
		while(verifyDigit(buff, returnBuff, getchar()));
		pthread_mutex_unlock(&mutex);
		correctWords++;
		kill(pidPai, SIGUSR1); // acorda pai
	}
	close(pipe1); 
}

void *graphical_handle(void *arg){
	int rows = 68;
	int cols = 68;

	pthread_t print_word;
	pthread_t watch_terminal;
	pthread_t print_header;

	Graphical_handle_arguments *args = (Graphical_handle_arguments *)arg;
	pthread_mutex_t *mutex = args->mutex;
	char *wordPointer = (char *)args->wordPointer;
	
	Watch_terminal_arguments watch_term_args;
	watch_term_args.rows = &rows;
	watch_term_args.cols = &cols;
	
	pthread_create(&watch_terminal, NULL, watch_terminal_size, &watch_term_args);
	
	Fall_word_arguments fall_word_args;
	fall_word_args.wordPointer = wordPointer;
	fall_word_args.cols = &cols;
	fall_word_args.rows = &rows;
	fall_word_args.lifes = args->lifes;
	fall_word_args.correctWords = args->correctWords;
	fall_word_args.dificult = args->dificult;
	fall_word_args.level = args->level;
	fall_word_args.wordsToChangeDificult = args->wordsToChangeDificult;

	while(1){
		pthread_create(&print_word, NULL, fall_word, &fall_word_args);
		pthread_mutex_lock(mutex);
		pthread_cancel(print_word);
		pthread_mutex_unlock(mutex);
	}
}

void *watch_terminal_size(void *arg){
	Watch_terminal_arguments *args = (Watch_terminal_arguments *)arg;
	int *rows; 
	int *cols; 
	rows = args->rows;
	cols = args->cols;

	struct winsize w;
	// Fica atualizando o tamanho do terminal para ficar mais dinamico rsrs.
	while (1){
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		*rows = w.ws_row;
		*cols = w.ws_col;
	}
}

void *fall_word(void *arg){
    char *returnBuff;
	int *rows;
	int *cols;
	int *lifes;
	int *correctWords;
	unsigned long int *dificult;
	int *level;
	int *wordsToChangeDificult;
	
	Fall_word_arguments *args = (Fall_word_arguments *)arg;
	rows = args->rows;
	cols = args->cols;
	lifes = args->lifes;
	correctWords = args->correctWords;
	returnBuff = args->wordPointer;
	dificult = args->dificult;
	level = args->level;
	wordsToChangeDificult = args->wordsToChangeDificult;

	srand(time(NULL));
	int random = (rand() % *cols) + 1;
	//int random = 75;
	if (random + strlen(returnBuff) > *cols) {
		// Evita que a palavra separe no meio no canto da tela.
		random = random - strlen(returnBuff);
	}	

	int j;
	while(1){
		if ((*wordsToChangeDificult) == (*correctWords)){
			(*level)++;
			*wordsToChangeDificult = getChangeDificult(*level, *wordsToChangeDificult);

			*dificult = getDificult(*level);
		}

		if ((*lifes) == 0){
			printf("Segmentation fault 0x00000000\n eh brinks, ce perdeu mano!\n");
			exit(0);
		}

		char color[15];
		for (int i = 0; i < *rows - 1; i++) {
			j = i;
			if ((random + j) + strlen(returnBuff) - 1 >= *cols) {
				// Rebate no canto da tela
				while(i < *rows){
					i++;
					j--;
					config_next_color(*rows - 1, i, color);
					print_header(*lifes, *correctWords, *level, *dificult, *wordsToChangeDificult, color);
					printf("\e[%d;%ldH%s\n", i + 1, ((random + j) - strlen(returnBuff) + 1) , returnBuff); // anscii mais rapido que \n
					fflush(stdout);  
					usleep(*dificult);  
					clear_screen();
				}
				break;
			}

			j = i;
			config_next_color(*rows, i, color);  
			print_header(*lifes, *correctWords, *level, *dificult, *wordsToChangeDificult, color);
			printf("\e[%d;%dH%s\n", i + 1, random + j, returnBuff); // anscii mais rapido que \n
			fflush(stdout);  
			usleep(*dificult);  
			clear_screen();
    	}
		(*lifes)--;
	}
}

void print_header(int lifes, int correctWords, int level, unsigned long int dificult, int wordsToChangeDificult, char *color){
	printf("%sPONTUACAO: %d \t VIDAS: %d NIVEL: %d DIFICULDADE: %ld ALEATORIO: %d %s", "\e[38;5;255m", correctWords, lifes, level, dificult, wordsToChangeDificult, color);
}

int config_next_color(int total_rows, int current_row, char *color){
	int um_terco = total_rows / 3;
	int dois_terco = um_terco * 2;

	int reds[5] = {196, 160, 124, 88, 52};
	int yellows[5] = {226, 184, 178, 143, 100};
	int greens[5] = {118, 82, 70, 28, 22};
	
	if (current_row > dois_terco){
		// red
		sprintf(color, "\e[38;5;%dm", reds[return_current_color(um_terco, current_row /3)]);
		return 0;
	} else if (current_row > um_terco) {
		// yellow
		sprintf(color, "\e[38;5;%dm", yellows[return_current_color(um_terco, current_row / 2)]);
		return 0;
	}

	//green
	sprintf(color, "\e[38;5;%dm", greens[return_current_color(um_terco, current_row )]);
	return 0;
}

int return_current_color(int terco, int current_row){
	int quintuplo_1 = terco / 5;
	int quintuplo_2 = quintuplo_1 * 2;
	int quintuplo_3 = quintuplo_1 * 3;
	int quintuplo_4 = quintuplo_1 * 4;
	int quintuplo_5 = quintuplo_1 * 5;
	
	if (current_row >= quintuplo_5 ){
		return 4;
	}
	if (current_row >= quintuplo_4 ){
		return 3;
	}
	if (current_row >= quintuplo_3 ){
		return 2;
	}
	if (current_row >= quintuplo_2 ){
		return 1;
	}
	
	return 0;
}

int	verifyDigit(char *baseWord, char *testWord, char readChar) {
	
	if (strlen(baseWord) == 0)
	{
		return (0);
	}
	if (baseWord[0] == toupper(readChar))
	{
		
		testWord[(strlen(testWord) - 1) - (strlen(baseWord) - 1)] = '*';
		
		for (int i = 0; i < strlen(baseWord); i++)
		{
			baseWord[i] = baseWord[i + 1];
		}
		if (testWord[strlen(testWord) -2] == '*') { clear_screen(); return(0);}
	}
	return (1);
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

void verifyWord(char *word, char *returnWord) {
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

void clear_screen(){
	printf("\e[1;1H\e[2J");
}

int getChangeDificult(int level, int wordsToChange) {
	srand(time(NULL));
	switch(level) {
		case 1:
			return rand() % 10;
		break;
 		default: 
			return (rand() % wordsToChange + 1) + wordsToChange;
	}
}

unsigned long int getDificult(int level){
	switch(level) {
		case 2:
			return DIFICULT_2;  	
		break;
		case 3: 
			return DIFICULT_3;
		break;
		case 4: 
			return DIFICULT_4;
		break; 
		case 5: 
			return DIFICULT_5;
		break; 
		case 6: 
			return DIFICULT_6;
		break; 
		case 7: 
			return DIFICULT_7;
		break; 
 		default: 
			return DIFICULT_8;
	}
}

// Queue	*createQueue(void) {
// 	Queue	*temp;

// 	// Inicializa fila vazia
// 	temp = (Queue *)malloc(sizeof(Queue));
// 	temp->first = temp->last = NULL;
// 	return (temp);
// }
// 	// Inicializa fila vazia
// 	temp = (Queue *)malloc(sizeof(Queue));
// 	temp->first = temp->last = NULL;
// 	return (temp);
// }

// void	insert(Queue *q, char *input_word) {
// 	Node	*temp;
// void	insert(Queue *q, char *input_word) {
// 	Node	*temp;

// 	temp = (Node *)malloc(sizeof(Node));
// 	memset(temp->word, '\0', sizeof(temp->word));
// 	strcpy(temp->word, input_word);
// 	temp->next = NULL;
// 	if (q->last == NULL)
// 	{
// 		// Primeiro input
// 		q->first = q->last = temp;
// 		return ;
// 	}
// 	q->last->next = temp;
// 	q->last = temp;
// }
// 	temp = (Node *)malloc(sizeof(Node));
// 	memset(temp->word, '\0', sizeof(temp->word));
// 	strcpy(temp->word, input_word);
// 	temp->next = NULL;
// 	if (q->last == NULL)
// 	{
// 		// Primeiro input
// 		q->first = q->last = temp;
// 		return ;
// 	}
// 	q->last->next = temp;
// 	q->last = temp;
// }

// char *return_new_word(Queue *q) {
// 	if (q->first == NULL)
// 		return (NULL);
// char *return_new_word(Queue *q) {
// 	if (q->first == NULL)
// 		return (NULL);

// 	char *returnWord = (char *)malloc(16 * sizeof(char));
// 	memset(returnWord, '\0', 16);
// 	char *returnWord = (char *)malloc(16 * sizeof(char));
// 	memset(returnWord, '\0', 16);

// 	Node *temp = q->first;
// 	strncpy(returnWord, temp->word, 15);
// 	returnWord[15] = '\0';
// 	Node *temp = q->first;
// 	strncpy(returnWord, temp->word, 15);
// 	returnWord[15] = '\0';

// 	q->first = q->first->next;
// 	if (q->first == NULL)
// 		q->last = NULL;
// 	q->first = q->first->next;
// 	if (q->first == NULL)
// 		q->last = NULL;

// 	free(temp);
// 	free(temp);

// 	return (returnWord);
// }
// 	return (returnWord);
// }

// int countQueue(Queue *q) {
//     if (q->first == NULL)
//         return 0;
//     int count = 1;
//     while(q->first->next != NULL)
//         count++;
//     return count;
// }
