#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX 1024
#define NUMTHREADS 4

FILE *f;

char board[MAX][MAX];
int rows, cols;

char words[MAX][MAX];
char found[MAX];
int wordcount;

//Read up to n characters into buf from the opened file
//f must have already been opened
int
readline(char *buf, int n)
{
	int c, len = 0;

	while (len < n && (c = fgetc(f)) != EOF && c != '\n')
		buf[len++] = c;
	buf[len] = '\0';

	return len;
}

//Convert an x,y direction into human-readable form
char *
direction(int x, int y)
{
	switch (x) {
		case -1:
			switch (y) {
				case -1:	return "left and up";
				case 0:		return "up";
				case 1:		return "right and up";
			}
		case 0:
			switch (y) {
				case -1:	return "backwards";
				case 0:		return "error";
				case 1:		return "forwards";
			}
		case 1:
			switch (y) {
				case -1:	return "left and down";
				case 0:		return "down";
				case 1:		return "right and down";
			}
	}

	return "error";
}

//Find words[index] within the word search board by looping through each character and direction
void
findword(int index)
{
	char *word = words[index];
	int wordlen = strlen(word);
	int pos;
	
	//For each row
	for (int i = 0; i < rows; i++) {
		//For each column
		for (int j = 0; j < cols; j++) {
			//If the board here is equal to the first letter, search in all directions
			if (board[i][j] == word[0]) {
				//For each x direction
				for (int x = -1; x <= 1; x++) {
					//For each y direction
					for (int y = -1; y <= 1; y++) {
						//See if the word matches
						for (pos = 1; pos < wordlen; pos++) {
							if (word[pos] != board[i + pos*x][j + pos*y])
								break;
						}

						//word matched
						if (pos == wordlen) {
							printf("%s found at row %d, col %d, going %s\n", word, i, j, direction(x,y));
							found[index] = 1;
							return;
						}
						
					}
				}
			}
		}
	}
}

//Find all words in the words list
void *
findwords(void *data)
{
	int tnum = *(int *)data;
	for (int i = tnum * wordcount / NUMTHREADS; i < (tnum + 1) * wordcount / NUMTHREADS; i++)
		findword(i);
	return NULL;
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: ./wordsearch <filename> -threaded=[0/1]\n");
		return 0;
	}

	//Open file for reading
	f = fopen(argv[1], "r");

	//Read in board
	rows = 0;
	cols = readline(board[0]);
	rows++;
	while (readline(board[rows++]));

	//Read in words
	for (wordcount = 0; wordcount < MAX && readline(words[wordcount]); wordcount++);
	memset(found, 0, sizeof(char) * wordcount);

	//Print out the board again
	printf("\n");
	for (int i = 0; i < rows; i++)
		printf("%s\n", board[i]);
	printf("\n");

	//Find the words
	if (argc > 2 && !strcmp(argv[2], "-threaded=1")) {
		//Spawn threads
		pthread_t threads[NUMTHREADS];
		int startpos[NUMTHREADS];
		for (int i = 0; i < NUMTHREADS; i++) {
			startpos[i] = i;
			pthread_create(&threads[i], NULL, findwords, startpos + i);
		}
		
		//Join threads
		for (int i = 0; i < NUMTHREADS; i++)
			pthread_join(threads[i], NULL);
	}
	else {
		for (int i = 0; i < wordcount; i++)
			findword(i);
	}
	
	//Print out words that weren't found
	char printed = 0;
	for (int i = 0; i < wordcount; i++){
		if (!found[i]){
			if (!printed){
				printed = 1;
				printf("\nWORDS NOT FOUND:\n");
			}
			printf("\t%s\n", words[i]);
		}
	}

	fclose(f);
	return 0;
}
