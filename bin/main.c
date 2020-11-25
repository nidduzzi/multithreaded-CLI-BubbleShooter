#include "lib.h"

int main()
{
    char str[256] = "";
    initscr();
    cbreak();
    // noecho();
    if (has_colors())
    {
        if(LINES > 50 || COLS > 1000)
        {
            addstr("Terminal too small\n");
            refresh();
            getch();
        }
        else
        {

            if (start_color() == OK)
            {
                init_pair(1, COLOR_YELLOW, COLOR_RED);
                init_pair(2, COLOR_GREEN, COLOR_GREEN);
                init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

                attrset(COLOR_PAIR(1));
                addstr("Yellow and red\n\n");
                refresh();
                attroff(COLOR_PAIR(1));

                attrset(COLOR_PAIR(2) | A_BOLD);
                addstr("Green and green A_BOLD\n\n");
                refresh();
                attroff(COLOR_PAIR(2));
                attroff(A_BOLD);

                attrset(COLOR_PAIR(3));
                addstr("Magenta and cyan\n");
                refresh();
                addstr("press any key for new window...");
                attroff(COLOR_PAIR(3));
                wgetch(stdscr);

                WINDOW *win = newwin(LINES, COLS, 0, 0);
                keypad(win, false);
                wclear(win);
                wattrset(win, COLOR_PAIR(1));
                sprintf(str, "-----------------\n|     -test-    |\n| ncurses Demo %d|\n-----------------\n\n", 1);
                waddstr(win, str);
                waddstr(win, "\nType any string you want...\n");
                wrefresh(win);
                wgetnstr(win, str, 255);
                wmove(win, 10, 10);
                
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
    }
    else
    {
        addstr("Not colour capable\n");
        refresh();
    }

    endwin();
    return EXIT_SUCCESS;
}