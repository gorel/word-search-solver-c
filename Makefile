all: wordsearch

wordsearch: wordsearch.c
	gcc -o wordsearch wordsearch.c -std=gnu99 -lpthread -O3

clean:
	rm wordsearch
