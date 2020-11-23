#include "lib.h"

int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    WINDOW *win = newwin(LINES, COLS, 0, 0);
    waddstr(win, "-----------------\n|    Arrozy   |\n| ncurses Demo  |\n-----------------\n\n");
    wrefresh(win);
    waddstr(win, "\npress any key to exit...");
    wrefresh(win);
    wgetch(win);
    delwin(win);
    endwin();
    return EXIT_SUCCESS;
}