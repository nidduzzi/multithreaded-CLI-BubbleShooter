#include "lib.h"

int main()
{
    FILE *bullet_sprite = fopen("bullet.txt", "r");
    int i = 0;
    char str[256] = "";
    char c = '\0';
    while(i < 4)
    {
        c = fgetc(bullet_sprite);
    }
    initscr();
    cbreak();
    if (has_colors())
    {
        if (start_color() == OK)
        {
            init_pair(1, COLOR_YELLOW, COLOR_RED);
            init_pair(2, COLOR_GREEN, COLOR_BLUE);
            init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

            attrset(COLOR_PAIR(1));
            addstr("Yellow and red\n\n");
            refresh();
            attroff(COLOR_PAIR(1));

            attrset(COLOR_PAIR(2) | A_BOLD);
            addstr("Green and blue A_BOLD\n\n");
            refresh();
            attroff(COLOR_PAIR(2));
            attroff(A_BOLD);

            attrset(COLOR_PAIR(3));
            addstr("Magenta and cyan\n");
            refresh();
            addstr("press any key for new window...");
            attroff(COLOR_PAIR(3));
            getch();

            WINDOW *win = newwin(LINES, COLS, 0, 0);
            keypad(win, TRUE);
            wattrset(win, COLOR_PAIR(1));
            sprintf(str, "-----------------\n|     Arrozy    |\n| ncurses Demo %d|\n-----------------\n\n", 1);
            waddstr(win, str);
            waddstr(win, "\nType any string you want...\n");
            wrefresh(win);
            wgetnstr(win, str, 255);
            wmove(win, 0, 0);
            wclear(win);
            waddstr(win, str);
            waddstr(win, "\npress any key to exit...");
            wrefresh(win);
            wgetch(win);
            wattroff(win, COLOR_PAIR(1));
            delwin(win);
        }
        else
        {
            addstr("Cannot start colours\n");
            refresh();
        }
    }
    else
    {
        addstr("Not colour capable\n");
        refresh();
    }

    endwin();
    return EXIT_SUCCESS;
}