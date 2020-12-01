#include "lib.h"


int main()
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    //addstr("Test ncurses");

    int scrcols = 70, scrlines = 30;
    int yMax, xMax;
    WINDOW *win;
    // set x and y max value
    getmaxyx(stdscr, yMax, xMax);

    //create window
    win = newwin(scrlines, scrcols, (yMax / 2) - (scrlines / 2) - (scrlines % 2), (xMax / 2) - (scrcols / 2) - (scrcols % 2));
    box(win, 0, 0);
    refresh();
    wrefresh(win);
    keypad(win, true);

    char menu[3][50] = {"1-PLAY", "2-SELECT_LEVEL", "3-QUIT"};
    int choice;
    int highlight = 0;

    mvwprintw(win, 5, 10, "WELCOME TO CLI BUBBLE SHOOTER");
    mvwprintw(win, 6, 10, "=============================");

    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (i == highlight)
            {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, i + 9, 15, menu[i]);
            wattroff(win, A_REVERSE);
        }
        choice = wgetch(win);
        switch (choice)
        {
        case KEY_UP:
            highlight--;
            if (highlight == -1)
            {
                highlight = 0;
            }
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight == 3)
            {
                highlight = 2;
            }
            break;
        default:
            break;
        }
        if (choice == 10)
        {
            break;
        }
    }
    wprintw(win, "Your choices : %s", menu[highlight]);
    wclear(win);
    wrefresh(win);
    if (highlight == 0)
        game_loop(win, 1);
    wgetch(win);
    delwin(win);
    /*
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
    getch();
    endwin();
    printf("%s", errbuffer.buffer);
    free(errbuffer.buffer);
    return EXIT_SUCCESS;
}