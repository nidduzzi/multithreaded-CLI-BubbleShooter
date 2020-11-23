#include "lib.h"

int main()
{
    char str[256] = "";
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    WINDOW *win = newwin(LINES, COLS, 0, 0);
    waddstr(win, "-----------------\n|    Arrozy   |\n| ncurses Demo  |\n-----------------\n\n");
    waddstr(win, "\nType in the string you want...\n");
    wrefresh(win);
    wgetnstr(win, str, 255);
    wmove(win, 0,0);
    wclear(win);
    waddstr(win, str);
    waddstr(win, "\npress any key to exit...");
    wrefresh(win);
    wgetch(win);
    delwin(win);
    endwin();
    return EXIT_SUCCESS;
}