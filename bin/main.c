#include "lib.h"

int main()
{
    char str[256] = "";
    initscr();
    cbreak();
    // noecho();
    if (has_colors())
    {
        if (LINES > 50 || COLS > 1000)
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
                init_pair(2, COLOR_BLACK, COLOR_GREEN);
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
                keypad(win, true);
                wclear(win);
                wattrset(win, COLOR_PAIR(1));
                sprintf(str, "-----------------\n|     -test-    |\n| ncurses Demo %d|\n-----------------\n\n", 1);
                waddstr(win, str);
                waddstr(win, "\nType any string you want...\n");
                wrefresh(win);
                wgetnstr(win, str, 255);
                wmove(win, 0, 0);
                strcat(str, "\n");
                wclear(win);
                waddstr(win, str);
                waddstr(win, "move your cursor to determine where to paste the incoming file...(press enter to continue)\n");
                int row = 2, col = 0, key = 0;
                noecho();
                while ((key = wgetch(win)) != (int)'\n')
                {
                    switch (key)
                    {
                    case KEY_UP:
                        row -= (row > 1) ? 1 : 0;
                        break;
                    case KEY_DOWN:
                        row += (row < LINES) ? 1 : 0;
                        break;
                    case KEY_LEFT:
                        col -= (col >= 0) ? 1 : 0;
                        break;
                    case KEY_RIGHT:
                        col += (col < COLS) ? 1 : 0;
                        break;
                    case KEY_ENTER:
                        return 0;
                        break;
                    default:
                        break;
                    }
                    wmove(win, row, col);
                    addch((char)219);
                    wrefresh(win);
                    wdelch(win);
                }
                wattroff(win, COLOR_PAIR(1));
                wattron(win, COLOR_PAIR(2));
                int maxline = 0, maxrow = 0, i;
                char **tmp;
                textLoader("../data/l1/level.txt", &tmp, &maxrow, &maxline);
                for (i = 0; i < maxline; ++i)
                {
                    wmove(win, row + i, col);
                    waddstr(win, tmp[i]);
                }
                wattroff(win, COLOR_PAIR(2));
                wattron(win, COLOR_PAIR(3));
                wmove(win, row + maxline, 0);
                waddstr(win, "\npress any key to exit...");
                wrefresh(win);
                wattroff(win, COLOR_PAIR(3));
                wgetch(win);
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