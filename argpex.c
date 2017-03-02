#include <stdio.h>
#include "handle_arguments.h"
#include <stdlib.h>
#include <time.h>
#include "file_utils.h"
#include <sys/stat.h>

/* Function Prototypes */
void clearScreen();

void validateUserInput(char* userInput, int width, char** gameBoard);

void userMove(char* p_userInput, char** gameBoard, int height);

int isValidMove(char** gameBoard, int width, int column);

void computerMove(char** gameBoard, int width, int height);

void checkIfSomebodyWon(char** gameBoard, int width, int height, int* userWon, int* computerWon, int connect);

void resetBoard(char** gameBoard, int width, int height);

void cleanup(char** gameBoard, int height, char* userInput);

void drawBoard(char** gameBoard, int width, int height);

int boardFull(char** gameBoard, int width, int height);

void printPadding(int padding, int width);

void saveGame(char* filename, int width, int height, int connect, char** gameBoard);

int power(int x, int y);


/*
 * Main class
 */
int main(int argc, char** argv){
	struct arguments arguments;
	char* userInput = (char*)malloc(sizeof(char)*10);
	char** gameBoard;
	int userWon = 0;
	int computerWon = 0;
	int width = 7;
	int height = 7; 
	int connect = 4;
	char* filename;
	char* saveFile;
	char* buffer;
	char saveFileName[] = "Connect4SaveFile.txt";
	char c;
	int i, j, index;
	int size;
	FILE* ptr_file;
	struct stat st;

	srand(time(NULL));

	setup(argc, argv, &arguments);

	filename = arguments.load;

	/* Load a game from command line argument */
	if (filename != NULL){
		/* Set all game parameters to 0 */
		width = 0;
		height = 0;
		connect = 0;

		ptr_file = fopen(filename, "r");

		/* Check that file has been opened */
		if (!ptr_file){
			fprintf(stderr, "Input file could not be opened.");
		}

		/* Get file size */
		stat(filename, &st);
		size = st.st_size;

		buffer = malloc(sizeof(char) * size);

		/* Read from file and store into buffer */
		fgets(buffer, size, ptr_file);

		/* Translate buffer into game parameters and board positions */
		c = buffer[0];

		// get width
		index = 1;
		while (c!=' '){
			width = width * 10;
			width += c - '0';
			c = buffer[index];
			index++;
		}
		c = buffer[index];
		index++;

		// get height
		while (c!=' '){
			height = height * 10;
			height += c - '0';
			c = buffer[index];
			index++;
		}
		c = buffer[index];
		index++;

		// get connect
		while (c!=' '){
			connect = connect * 10;
			connect += c - '0';
			c = buffer[index];
			index++;
		}
		c = buffer[index];
		index++;
		
		/* Place gameboard in memory */
		gameBoard = (char**)malloc(sizeof(char*) * width);
		for (i=0; i < width; i++){
			gameBoard[i] = malloc(sizeof(char) * height);
		}

		/* Initialize gameboard with values from file */
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				gameBoard[j][i] = c;
				c = buffer[index];
				index++;
			}
			
		}
		free(buffer);
	} else {
		/* Set parameters with command line arguments if applicable */
		if (arguments.square != 0){
			width = arguments.square;
			height = arguments.square;
		} else {
			if (arguments.width != 0)
				width = arguments.width;
			if (arguments.height != 0)
				height = arguments.height;
		}

		if (arguments.connect != 0)
			connect = arguments.connect;
	
		if(connect > width && connect > height){
			clearScreen();
			printf("A %d-piece long string will not fit on a %d by %d size board. Nobody can win!\n\n\n", connect, width, height);
			return 0;
		}
	
		gameBoard = (char**)malloc(sizeof(char*) * width);
		for (i=0; i < width; i++){
			gameBoard[i] = malloc(sizeof(char) * height);
		}
	
		resetBoard(gameBoard, width, height);
	}

	*userInput = 'x';

	while (*userInput != 'n'){
		clearScreen();

		if (userWon || computerWon){
			resetBoard(gameBoard, width, height);
			userWon = 0;
			computerWon = 0;
		}

		drawBoard(gameBoard, width, height);

		printf("\n\nEnter column number where you'd like to drop your piece\n(Enter 'q' to quit)\n");
		scanf(" %s", userInput);

		validateUserInput(userInput, width, gameBoard);

		if (*userInput == 'q'){
			cleanup(gameBoard, height, userInput);
			return 0;
		}

		if (*userInput == 's'){
			saveGame(saveFileName, width, height, connect, gameBoard);
			printf("\n\nYour game has been saved in the file %s\n\nType 'c' to continue...", saveFileName);
			scanf(" %s", userInput);
			continue;
		}
		
		userMove(userInput, gameBoard, height);

		checkIfSomebodyWon(gameBoard, width, height, &userWon, &computerWon, connect);
		
		if (!boardFull(gameBoard, width, height) && !userWon){
			computerMove(gameBoard, width, height);
			checkIfSomebodyWon(gameBoard, width, height, &userWon, &computerWon, connect);
		}

		if (userWon){
			clearScreen();
			drawBoard(gameBoard, width, height);
			printf("\n\n  You win!");
			printf("\n\nWould you like to play again? (y or n)");
			scanf(" %s", userInput);
		}
		if (computerWon){
			clearScreen();
			drawBoard(gameBoard, width, height);
			printf("\n\n  You lose...");
			printf("\n\n  You win!");
			printf("\n\nWould you like to play again? (y or n)");
			scanf(" %s", userInput);
		}
		if (boardFull(gameBoard, width, height)){
			clearScreen();
			drawBoard(gameBoard, width, height);
			printf("\n\n  Tie game!");
			printf("\n\nWould you like to play again? (y or n)");
			scanf(" %s", userInput);
		}
	}

	return 0;
}

void clearScreen(){
//	system("cls");
	system("clear");
}

void validateUserInput(char* p_userInput, int width, char** gameBoard){
	int column;
	int userInput;
	char c_userInput;

	while (1){
		c_userInput = *p_userInput;

		if (c_userInput == 'q' || c_userInput == 's')
			return;

		userInput = (int)atoi(p_userInput);

		if (isValidMove(gameBoard, width, userInput))
			return;

		printf("Column is full. Please choose another.\n\n");
		
		printf("\n\nEnter column number where you'd like to drop your piece\n(Enter 'q' to quit)\n");
		scanf(" %s", p_userInput);
	}
}

void userMove(char* p_userInput, char** gameBoard, int height){
	int userInput = (int)atoi(p_userInput);
	int h;
	
	for (h=0; h<height; h++){
		if (gameBoard[userInput-1][h] != '.')
			break;
	}

	gameBoard[userInput-1][h-1] = 'x';
}

int isValidMove(char** gameBoard, int width, int column){
	if (column > 0 && column <= width){
		if (gameBoard[column-1][0] == '.'){
			return 1;
		}
	}
	return 0;
}

void computerMove(char** gameBoard, int width, int height){
	//random column
	int r = (rand() % width) + 1;
	int h;

	while (!isValidMove(gameBoard, width, r)){
		r = (rand() % width) + 1;
	}

	for (h=0; h<height; h++){
		if (gameBoard[r-1][h] != '.')
			break;
	}

	gameBoard[r-1][h-1] = 'o';
	
	return;
}

void checkIfSomebodyWon(char** gameBoard, int width, int height, int* userWon, int* computerWon, int connect){
	int w, h, i, j, count;
	char c;
	
	for (h=0; h<height; h++){
		for (w=0; w<width; w++){
			if (gameBoard[w][h] != '.'){
				c = gameBoard[w][h];

				//check horizontal				
				count = 1;
				for (i=w+1; i<width; i++){
					if(gameBoard[i][h] != c){
						break;
					}
					count++;
				}

				if (count == connect){
					if (c == 'x'){
						*userWon = 1;
					}
					if (c == 'o')
						*computerWon = 1;
				}

				//check vertical
				count = 1;
				for (j=h+1; j<height; j++){
					if(gameBoard[w][j] != c){
						break;
					}
					count++;
				}
				if (count == connect){
					if (c == 'x')
						*userWon = 1;
					if (c == 'o')
						*computerWon = 1;
				}

				//check diagonal left
				count = 1;
				i = w-1;
				for (j=h+1; j<height; j++){
					if (i<0){
						break;
					}
					if (gameBoard[i][j] != c){
						break;
					}	
					count++;
					i--;
				}
				if (count == connect){
					if (c == 'x')
						*userWon = 1;
					if (c == 'o')
						*computerWon = 1;
				}

				//check diagonal right
				count = 1;
				i = w+1;
				for (j=h+1; j<height; j++){
					if (i>=width){
						break;
					}
					if(gameBoard[i][j] != c){
						break;
					}
					count++;
					i++;
				}
				if (count == connect){
					if (c == 'x'){
						*userWon = 1;
					}
					if (c == 'o')
						*computerWon = 1;
				}
			}
		}
	}
}

void resetBoard(char** gameBoard, int width, int height){
	int i, j;

	for (j=0; j<width; j++){
			for (i=0; i<height; i++){
				gameBoard[j][i] = '.';
			}
	}
}

void cleanup(char** gameBoard, int height, char* userInput){
	int h;
	for (h=0; h<height; h++){
		free(gameBoard[h]);
	}
	free(gameBoard);
	free(userInput);
	return;
}

void drawBoard(char** gameBoard, int width, int height){
	int i, j;
	int padding = 38 - width;

	if (padding < 0)
		padding = 0;

	printf("\n\n");
	printf("  _|_|_|                                                      _|      _|  _|\n");
	printf("_|          _|_|    _|_|_|    _|_|_|      _|_|      _|_|_|  _|_|_|_|  _|  _|\n");
	printf("_|        _|    _|  _|    _|  _|    _|  _|_|_|_|  _|          _|      _|_|_|_|\n");
	printf("_|        _|    _|  _|    _|  _|    _|  _|        _|          _|          _|  \n");
	printf("_|_|_|_|    _|_|    _|    _|  _|    _|    _|_|_|    _|_|_|    _|          _|\n\n\n");

	printPadding(padding, width);
	for (i=0; i<width; i++){
		if (i == 10)
			break;
		printf(" %d", i+1);
	}

	for (; i<width; i++){
		printf("%d", i+1);
	}

	printf("\n");
	printPadding(padding, width);
	printf("|");

	for (j=0; j<height; j++){
		for (i=0; i<width; i++){
			printf("%c", gameBoard[i][j]);
			printf("|");
			if (((i+1) % width) == 0){
				printf("\n");
				printPadding(padding, width);
				if (j != height-1)
					printf("|");
			}

		}
	}

	for (i=0; i<width; i++){
		if (i == 10)
			break;
		printf(" %d", i+1);
	}

	for (; i<width; i++){
		printf("%d", i+1);
	}
}

void printPadding(int padding, int width){
	int i;
	for (i=0; i<padding; i++){
		printf(" ");
	}
}

int boardFull(char** gameBoard, int width, int height){
	int w, h;

	for (w=0; w<width; w++){
		for (h=0; h<height; h++){
			if (gameBoard[h][w] == '.')
				return 0;
		}
	}
	return 1;
}

int power(int x, int y){
	int val = x;
	int i = y;
	if (y == 0)
		return 1;
	if (y == 1)
		return val;
	while (i>1){
		val = val * x;
		i--;
	}
	return val;
}


void saveGame(char* filename, int width, int height, int connect, char** gameBoard){

	int i, j, k, back, index, pow;
	char* buffer;
	int w = width;
	int h = height;
	int con = connect;
	char c;

	//malloc the file contents
	buffer = (char*)malloc(sizeof(char) * ( (height * width) + 20));

	index = 0;

	//width
	pow = 0;
	while (w / 10 != 0){
		pow++;
		w = w / 10;
	}
	w = width;
	while (pow >= 0){
		c = (w / power(10,pow)) + 48;
		buffer[index] = c;
		w -= (c - 48) * power(10,pow);
		pow--;
		index++;
	}
	buffer[index] = ' ';
	index++;
	pow = 0;


	//height
	pow = 0;
	while (h / 10 != 0){
		pow++;
		h = h / 10;
	}
	h = height;
	while (pow >= 0){
		c = (h / power(10,pow)) + 48;
		buffer[index] = c;
		h -= (c - 48) * power(10,pow);
		pow--;
		index++;
	}
	buffer[index] = ' ';
	index++;
	pow = 0;


	//connect
	pow = 0;
	while (con / 10 != 0){
		pow++;
		con = con / 10;
	}
	con = connect;
	while (pow >= 0){
		c = (con / power(10,pow)) + 48;
		buffer[index] = c;
		con -= (c - 48) * power(10,pow);
		pow--;
		index++;
	}
	buffer[index] = ' ';
	index++;

	for (j=0; j<height; j++){
		for (k=0; k<width; k++){
			buffer[index] = gameBoard[k][j];
			index++;
		}
	}

	buffer[index] = '\0';
	
	printf("\n\n\n");
	for (i=0; i<(height * width + 20); i++){
		printf("%c", buffer[i]);
	}
	printf("\n\n\n");

	write_file(filename, buffer, ((height * width) + 20));
	
	free(buffer);

}	
