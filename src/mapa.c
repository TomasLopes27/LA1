#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "mapa.h"


void gerar(STATE *s) {
    int row, col;
    getmaxyx(stdscr, row, col);

    srand(time(NULL));
	s->playerX = rand() % (col);
	s->playerY = rand() % (row);
}

void mapa() {



    initscr();
    cbreak();
    noecho();

    int row, col;       
    getmaxyx(stdscr, row, col);
	
  for (int x = 2; x < col-2; x++) {
    for (int y = 1; y < row-2; y++) {
        mvaddch(y, x, ' ' | A_BOLD);
    }
}


    for (int i = 0; i < row - 1; i++)
    {
        mvaddch(i, 0, ' ');
        mvaddch(i, col - 1, ' ');       
    }

    for (int j = 0; j < col; j++)
    {
        mvaddch(0, j, ' ');
    }

	for (int a = 2; a < row - 2; a++)
    {
        mvaddch(a, 1, '#');
        mvaddch(a, col - 2, '#');
    }

    for (int b = 0; b < col; b++)
    {
        mvaddch(1, b, '#');
        mvaddch(row - 2, b, '#');
    }

    for (int c = 0; c < row; c++)
    {
        mvaddch(c , 0, ' ');
    }

    for (int g = 0; g < row; g++)
    {
        mvaddch(g, col-1, ' ');
    }
    

    int numSquares = 15;
    int squareSizeMin = 8;
    int squareSizeMax = 10;
    int x, y, w, h;
    for (int i = 0; i < numSquares; i++) {
        w = rand() % (squareSizeMax - squareSizeMin + 1) + squareSizeMin;
        h = rand() % (squareSizeMax - squareSizeMin + 1) + squareSizeMin;
        x = rand() % (col - w - 2) + 2;
        y = rand() % (row - h - 2) + 2;

        for (int j = y; j < y + h; j++) {
            for (int k = x; k < x + w; k++) {
                mvaddch(j, k, '#');
            }
        }
    }
}