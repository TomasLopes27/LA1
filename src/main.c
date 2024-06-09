#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>
#include "mapa.h"


int ncols, nrows;
int playerSight = 0;
int iluminarCaminhoAtivo = 0;

void iluminarCaminho(STATE *st) {
    if (mvinch(st->playerX, st->playerY) != '#') {
        mvaddch(st->playerX, st->playerY, '.');
    }

    double angulo;
    for (int i = 0; i < 360; i++) {
        angulo = i * PI / 180.0;
        double dx = cos(angulo);
        double dy = sin(angulo);

        int distancia_da_visao = 10;
        int x = st->playerX;
        int y = st->playerY;
        int pontos_iluminados = 0;

        for (int j = 1; j < distancia_da_visao; j++) {
            x = st->playerX + j * dx;
            y = st->playerY + j * dy;
            if (mvinch(x, y) == '#' || mvinch(x, y) == ' ') {
                break;
            }
            mvaddch(x, y, '.' | A_BOLD);
            pontos_iluminados++;
        }
        pontos_iluminados = 0;

        if (distancia_da_visao < pontos_iluminados) {
            for (int j = distancia_da_visao; j < pontos_iluminados; j++) {
                x = st->playerX + j * dx;
                y = st->playerY + j * dy;
                if (mvinch(x, y) == '.') {
                    mvaddch(x, y, ' ');
                }
            }
        }
    }

    if (mvinch(st->playerX, st->playerY) != '#') {
        mvaddch(st->playerX, st->playerY, '@' | A_BOLD);
    }
}


void do_movement_action(STATE *st, int dx, int dy) {
    int new_x = st->playerX + dx;
    int new_y = st->playerY + dy;

    mvaddch(st->playerX, st->playerY, '.' | A_BOLD);

    if (mvinch(new_x, new_y) == '#' && (new_x != st->playerX || new_y != st->playerY)) {
        playerSight = 0;
        return;
    }

    st->playerX = new_x;
    st->playerY = new_y;
    playerSight = 1;
}

void apagarpontos() {
    int nrows, ncols;
    getmaxyx(stdscr, nrows, ncols);

    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if (mvinch(i, j) != ' ') {
                char k = mvinch(i, j);
                if (k == '.') {
                    mvaddch(i, j, ' ');
                }
            }
        }
    }

    refresh();
}



void update(STATE *st, MONSTER *monster) {
    int key = getch();
    monster->vivo = 1;

    mvaddch(st->playerX, st->playerY, ' ');

    switch(key) {
        case KEY_HOME:
        case '7': do_movement_action(st, -1, -1);  break;
        case KEY_UP:
        case '8': do_movement_action(st, -1, 0); break;
        case KEY_PPAGE:
        case '9': do_movement_action(st, -1, 1); break;
        case KEY_LEFT:
        case '4': do_movement_action(st, 0, -1); break;
        case KEY_B2:
        case '5': break;
        case KEY_RIGHT:
        case '6': do_movement_action(st, 0, 1); break;
        case KEY_END:
        case '1': do_movement_action(st, 1, -1); break;
        case KEY_DOWN:
        case '2': do_movement_action(st, 1, 0); break;
        case KEY_NPAGE:
        case '3': do_movement_action(st, 1, 1); break;
        case 'q': endwin(); exit(0); break;
        case 'k':
            if(st->playerX == monster->monsterX && st->playerY == monster->monsterY){
                if(monster->vivo){
                    monster->vivo = 0;
                    mvaddch(monster->monsterX,monster->monsterY, 'x' | A_BOLD);
                }
                return;
            } break;
        case 'i':
            iluminarCaminhoAtivo = !iluminarCaminhoAtivo;

            if (!iluminarCaminhoAtivo) {
                iluminarCaminho(st);
                refresh();
                apagarpontos();
            }
            break;
    }

    if (iluminarCaminhoAtivo) {
        iluminarCaminho(st);
    }


    if(monster->vivo){
        double distancia = sqrt(pow(st->playerX - monster->monsterX, 2) + pow(st->playerY    - monster->monsterY, 2));

        if(distancia <= 3){
            monster->vivo = 1;
            int d_x = (int)round((st->playerX - monster->monsterX) / distancia);
            int d_y = (int)round((st->playerY - monster->monsterY) / distancia);

            int new_m_x = monster->monsterX - d_x;
            int new_m_y = monster->monsterY - d_y;

            if(mvinch(new_m_x, new_m_y) != '#' && new_m_x > 0 && new_m_x < nrows && new_m_y > 0 && new_m_y < ncols){
                mvaddch(monster->monsterX, monster->monsterY, ' ');

                monster->monsterX = new_m_x;
                monster->monsterY = new_m_y;
            }
        } else {
            int dx = rand() % 3 - 1;
            int dy = rand() % 3 - 1;
            int new_x = monster->monsterX + dx;
            int new_y = monster->monsterY + dy;

            if(mvinch(new_x, new_y) != '#' && new_x > 0 && new_x < nrows && new_y > 0 && new_y < ncols){

            mvaddch(monster->monsterX, monster->monsterY, ' ');

            monster->monsterX = new_x;
            monster->monsterY = new_y;
            }
        }   
    }
    
    
    

    if(monster->vivo){
        mvaddch(monster->monsterX, monster->monsterY, 'M' | A_BOLD);
    }
    
    
    

}

int main() {
    WINDOW *wnd = initscr();
    getmaxyx(wnd, nrows, ncols);

    srandom(time(NULL));

    start_color();

    mapa();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, false);
    keypad(stdscr, true);

    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);

    struct state st;
    do {
        st.playerX = rand() % (nrows - 2 - 3 + 1) + 3;
        st.playerY = rand() % (ncols - 2 - 3 + 1) + 3;
    } while (mvinch(st.playerX, st.playerY) == '#');

    MONSTER monster;
    do {
        monster.monsterX = rand() % (nrows - 2 - 3 + 1) + 3;
        monster.monsterY = rand() % (ncols - 2 - 3 + 1) + 3;
    } while (mvinch(monster.monsterX, monster.monsterY) == '#');

    while (1) {
        if (mvinch(st.playerX, st.playerY) != '#') {
            mvaddch(st.playerX, st.playerY, '.');
        }

        if (mvinch(st.playerX, st.playerY) != '#') {
            mvaddch(st.playerX, st.playerY, '@' | A_BOLD);
        }

        move(nrows - 1, 0);
        attron(COLOR_PAIR(COLOR_BLUE));
        printw("(%d, %d) %d %d", st.playerX, st.playerY, ncols, nrows);
        attroff(COLOR_PAIR(COLOR_BLUE));
        attron(COLOR_PAIR(COLOR_YELLOW));

        refresh();

        if (playerSight) {
            attron(COLOR_PAIR(COLOR_BLACK));
        } else {
            attron(COLOR_PAIR(COLOR_BLACK));
        }

        mvaddch(monster.monsterX, monster.monsterY, 'M' | A_BOLD);


        attroff(COLOR_PAIR(COLOR_GREEN));
        attroff(COLOR_PAIR(COLOR_WHITE));
        move(st.playerX, st.playerY);

        update(&st, &monster);
        
    }

    return 0;
}