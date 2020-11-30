#include "lib.h"

int main()
{
    
    setlocale(LC_ALL, "");
    game_loop(1);
    initscr();
    cbreak();
    addstr("Test ncurses");
    getch();
    /*
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
                WINDOW *win = newwin(LINES, COLS, 0, 0);
                keypad(win, true);
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
                }
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

    */
    endwin();
    printf("%s", errbuffer.buffer);
    free(errbuffer.buffer);
    return EXIT_SUCCESS;
}