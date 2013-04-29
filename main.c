#include <curses.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


// This function shows the top 5 players
int great_wall( WINDOW* wnd )
{
    clear();
    init_pair( 1, COLOR_WHITE, COLOR_BLACK );
    init_pair( 2,COLOR_RED, COLOR_BLACK );
    init_pair( 3,COLOR_GREEN, COLOR_BLACK );
    wbkgd( wnd, COLOR_PAIR(1) );
    attron( COLOR_PAIR(2) );
    attron( A_BOLD );
    mvaddstr( 1, 35, "GREAT WALL OF FAME" );
    attroff( A_BOLD );
    attroff( COLOR_PAIR(2) );
    attroff( COLOR_PAIR(3) );
    mvaddstr( 3, 10, "Top 5" );
    int i = 0;
    FILE* scores = fopen( "scores", "a+" );
    int old_max = 10000;
    char maxc[5];
    char BUFF[16];
    for ( i = 0; i < 5; i++ )
    {
        fseek( scores, 0, SEEK_SET );
        int j = 11;
        int max = 0;
        int nr = 0;
        while ( fgets(BUFF,16,scores) != NULL )
        {
            nr = 0;
            j= 11;
            while ( (BUFF[j] >= 48) && (BUFF[j] <= 57) )
            {
                nr = nr * 10 + BUFF[j] - 48;
                j++;
            }

            if ( nr > max && nr < old_max )
            {
                max = nr;
                strcpy( maxc, &BUFF[11] );
            }
        }
        if ( nr == old_max )
        {
            break;
        }
        fseek( scores, 0, SEEK_SET );
        int x = i - 1;
        while ( fgets( BUFF, 16, scores ) != NULL )
        {
            if ( strstr(BUFF, maxc) != NULL )
            {
                x++;
                j = 11;
                while ( BUFF[j] == 48 )
                {
                    BUFF[j] = '\040';
                    j++;
                }
                for ( j = 0; j < 10; j++ )
                {
                    BUFF[j] = toupper( BUFF[j] );
                }
                attron( A_BOLD );
                attron( COLOR_PAIR(3) );
                mvaddstr ( 5 + 2 * x, 25, BUFF );
                attroff( A_BOLD );
                attroff( COLOR_PAIR(3) );
            }
        }
        old_max = max;
        i = x;
        fseek( scores, 0, SEEK_SET );
    }
    fclose( scores );
    attron( COLOR_PAIR(2) );
    attron( A_BOLD );
    mvaddstr( 23, 5, "Press ENTER to go back" );
    attroff( COLOR_PAIR(2) );
    attroff( A_BOLD );
    while ( getch() != '\n' )
    {}
    main_menu(6);
    return 0;
}


//Saves current game info in a binary file with the name player1_vs_player2
int save( int board[6][7], char nume_jucatori[2][12], int joc[3], int culori[2], int player )
{
    FILE* save_game;
    char file_name[25];
    strcpy( &file_name[0], nume_jucatori[0] );
    strcat( file_name, "_vs_" );
    strcat( file_name, nume_jucatori[1] );
    save_game = fopen( file_name, "wb" );
    fwrite( &board[0][0], sizeof(int), 6*7, save_game );
    fwrite( &nume_jucatori[0][0], sizeof(char), 2 * 12, save_game );
    fwrite( &joc[0], sizeof(int), 3, save_game );
    fwrite( &culori[0], sizeof(int), 2, save_game );
    fwrite( &player, sizeof(int), 1, save_game );
    fclose( save_game );
    FILE* saved_games;
    saved_games = fopen( "saved_games", "a+" );
    int j = 0;
    char buff[25], count = 0;
    while( fgets(buff, 25, saved_games) != NULL )
    {
        if ( strstr(buff, file_name) != NULL )
            j = 1;
        count++;
    }
    fclose( saved_games );
    if (j == 0)
    {
        if ( count >= 10 )
        {
            saved_games = fopen( "saved_games", "r" );
            char games[10][25];
            int i = 0;
            for ( i = 0; i < 10; i++ )
            {
                fgets( games[i], 25, saved_games );
            }
            fclose( saved_games );
            remove( "saved_games" );
            saved_games=fopen( "saved_games","w" );
            for ( i = 1; i < 10; i++ )
            {
                fprintf( saved_games, "%s", games[i] );
            }
            fprintf( saved_games, "%s\n", file_name );
            fclose( saved_games );
        }
        else
        {
            saved_games = fopen( "saved_games", "a+" );
            fprintf( saved_games, "%s\n", file_name );
            fclose( saved_games );
        }
    }
    return 0;
}

//Initializes board matrix with value 0
void init_board( int* board )
{
    int i = 0;
    int j = 0;
    for ( i = 0; i < 6; i++)
        for ( j = 0; j < 7; j++)
        {
            ( *board ) = 0;
            board++;
        }
}

//Shows the winning combination. It will make the combination blink
int show_winner ( WINDOW* wnd, int ln, int col, int dir, int culoare, int player, char nume[11] )
{
    start_color();
    int j = 0;
    char winner[11];
    for (j = 0; j <= strlen( nume ); j++)
    {
        winner[j] = toupper( nume[j] );
    }
    init_pair( 10, COLOR_WHITE, COLOR_GREEN );
    init_pair( 11, COLOR_WHITE, COLOR_BLUE );
    init_pair( 12, COLOR_WHITE, COLOR_RED );
    init_pair( 13, COLOR_WHITE, COLOR_YELLOW );
    init_pair( 14, COLOR_WHITE, COLOR_MAGENTA );
    init_pair( 15, COLOR_WHITE, COLOR_CYAN );
    init_pair( 1, COLOR_WHITE, COLOR_WHITE );
    init_pair( 2, COLOR_RED, COLOR_WHITE );
    ln = 4 + 3 * ln;
    col = 2 + 5 * col;
    int i = 0;
    cbreak();
    noecho();
    nodelay( wnd,TRUE );
    if ( dir == 11 )
    {
        int x = 1;
        while ( x == 1 )
        {
            for ( i = 0; i < 4; i++ )
            {
                attron( COLOR_PAIR(1) );
                mvaddstr( ln, col, "    " );
                mvaddstr ( ln+1, col, "    " );
                attroff( COLOR_PAIR(1) );
                ln = ln + 3;
                col = col + 5;
            }
            attron( COLOR_PAIR(1) );
            mvaddstr( 14, 52, "This round was won by " );
            mvaddstr( 15, 60, winner );
            attroff( COLOR_PAIR(1) );
            refresh();
            ln = ln - 12;
            col = col - 20;
            usleep( 400000 );
            for ( i = 0; i < 4; i++ )
            {
                attron( COLOR_PAIR(culoare) );
                mvaddstr( ln, col, "    " );
                mvaddstr( ln + 1, col, "    " );
                attroff( COLOR_PAIR(culoare) );
                ln = ln + 3;
                col = col + 5;
            }
            ln = ln - 12;
            col = col - 20;
            attron( A_BOLD );
            attron( COLOR_PAIR(2) );
            mvaddstr( 14, 52, "This round was won by " );
            mvaddstr( 15, 60, winner );
            mvaddstr( 17, 52, "Press ENTER key" );
            attroff( A_BOLD );
            attroff( COLOR_PAIR(2) );
            refresh();
            usleep( 400000 );
            int t = getch();
            if ( tolower(t) == '\n' )
            {
                x = 0;
            }
        }
        return 0;
    }
    else
    {

        if (dir == 12)
        {
            int x = 1;
            while ( x == 1 )
            {
                for ( i = 0; i < 4; i++ )
                {
                    attron( COLOR_PAIR(1) );
                    mvaddstr( ln, col, "    " );
                    mvaddstr( ln + 1, col, "    " );
                    attroff( COLOR_PAIR(1) );
                    col = col + 5;
                }
                attron( COLOR_PAIR(1) );
                mvaddstr( 14, 52, "This round was won by " );
                mvaddstr( 15, 60, winner );
                attroff( COLOR_PAIR(1) );
                refresh();
                col = col - 20;
                usleep( 400000 );
                for (i = 0; i < 4; i++)
                {
                    attron( COLOR_PAIR(culoare) );
                    mvaddstr( ln, col, "    " );
                    mvaddstr( ln + 1, col, "    " );
                    attroff( COLOR_PAIR(culoare) );
                    col = col + 5;
                }
                attron( A_BOLD );
                attron( COLOR_PAIR(2) );
                mvaddstr( 14, 52, "This round was won by" );
                mvaddstr( 17, 52, "Press ENTER key" );
                mvaddstr( 15, 60, winner );
                attroff( A_BOLD );
                attroff( COLOR_PAIR(2) );
                refresh();
                col = col - 20;
                usleep( 400000 );
                int t = getch();
                if( tolower(t) == '\n' )
                {
                    x = 0;
                }
            }
            return 0;
        }
        else
        {
            if (dir == 13)
            {
                int x = 1;
                while( x == 1 )
                {
                    for (i = 0; i < 4; i++)
                    {
                        attron( COLOR_PAIR(1) );
                        mvaddstr( ln, col, "    " );
                        mvaddstr( ln + 1, col, "    " );
                        attroff( COLOR_PAIR(1) );
                        col = col + 5;
                        ln = ln - 3;
                    }
                    attron( COLOR_PAIR(1) );
                    mvaddstr( 14, 52, "This round was won by " );
                    mvaddstr( 15, 60, winner);
                    attroff( COLOR_PAIR(1) );
                    refresh();
                    col = col - 20;
                    ln = ln + 12;
                    usleep( 400000 );
                    for ( i = 0; i < 4; i++ )
                    {
                        attron( COLOR_PAIR(culoare) );
                        mvaddstr( ln, col, "    " );
                        mvaddstr( ln + 1, col, "    " );
                        attroff( COLOR_PAIR(culoare) );
                        col = col + 5;
                        ln = ln - 3;
                    }
                    attron( A_BOLD );
                    attron( COLOR_PAIR(2) );
                    mvaddstr( 14, 52, "This round was won by " );
                    mvaddstr( 17, 52, "Press ENTER key" );
                    mvaddstr( 15, 60, winner );
                    attroff( A_BOLD );
                    attroff( COLOR_PAIR(2) );
                    refresh();
                    col = col - 20;
                    ln = ln + 12;
                    usleep( 400000 );
                    int t = getch();
                    if( tolower(t) == '\n' )
                    {
                        x = 0;
                    }
                }
                return 0;
            }
            else
            {
                int x = 1;
                while( x == 1 )
                {
                    for ( i = 0; i < 4; i++ )
                    {
                        attron( COLOR_PAIR(1) );
                        mvaddstr( ln, col, "    " );
                        mvaddstr( ln + 1, col, "    " );
                        attroff( COLOR_PAIR(1) );
                        ln = ln + 3;
                    }
                    attron( COLOR_PAIR(1) );
                    mvaddstr( 14, 52, "This round was won by " );
                    mvaddstr( 15, 60, winner);
                    attroff( COLOR_PAIR(1) );
                    refresh();
                    ln = ln - 12;
                    usleep( 400000 );
                    for (i = 0; i < 4; i++)
                    {
                        attron( COLOR_PAIR(culoare) );
                        mvaddstr( ln, col, "    " );
                        mvaddstr( ln + 1, col, "    " );
                        attroff( COLOR_PAIR(culoare) );
                        ln = ln + 3;
                    }
                    attron( A_BOLD );
                    attron( COLOR_PAIR(2) );
                    mvaddstr( 14, 52, "This round was won by " );
                    mvaddstr( 17, 52, "Press ENTER key" );
                    mvaddstr( 15, 60, winner );
                    attroff( A_BOLD );
                    attroff( COLOR_PAIR(2) );
                    refresh();
                    ln = ln - 12;
                    usleep( 400000 );
                    int t = getch();
                    if( tolower(t) == '\n' )
                    {
                        x = 0;
                    }
                }
                return 0;
            }
        }
    }
}


//check_winner verifies if there is a winner and returns the winning combinaion/ draw/ no winner
int check_winner( int pos, int board[6][7], int* win_ln, int* win_col, int *player )
{
    int i = 0;
    int j = 0;
    int x = 0;
    int nr_p = 1;
    int linie = 0;
    int coloana = 0;
    int winner[2];
    winner[0] = 0;
    winner[1] = 0;
    for ( x = 5; x >= 0; x--)
    {
        linie = 0;
        coloana = 0;

        j = pos;
        i = x;

        while ( ((i > 0) && (j > 0)) && (board[i-1][j-1] == board[i][j]) )
        {
            i = i - 1;
            j = j - 1;
        }
            linie = i;
            coloana = j;

        while ((i < 5 && j<6) && (board[i + 1][j + 1] == board[i][j]) && (nr_p < 4))
        {
            nr_p++;
            i = i + 1;
            j = j + 1;
        }

        if (nr_p == 4)
        {
            winner[ board[x][pos] - 1 ] = 11;
            *win_ln = linie;
            *win_col = coloana;
        }


        j = pos;
        i = x;
        nr_p = 1;

        while ( (j > 0) && (board[i][j - 1] == board[i][j]) )
        {
            j = j - 1;
        }

        linie = i;
        coloana = j;


        while ((j < 6) && (board[i][j + 1] == board[i][j]) && (nr_p < 4))
        {
            nr_p++;
            j = j + 1;
        }

        if (nr_p == 4)
        {
            winner[board[x][pos] - 1] = 12;
            *win_ln = linie;
            *win_col = coloana;
        }

        j = pos;
        i = x;
        nr_p = 1;

        while ( ((i < 5) && (j > 0) ) && (board[i + 1][j - 1] == board[i][j]) )
        {
            i = i + 1;
            j = j - 1;
        }
        linie = i;
        coloana = j;


        while ( ((i > 0)  && (j < 6)) && (board[i - 1][j + 1] == board[i][j]) && (nr_p < 4))
        {
            nr_p++;
            i = i - 1;
            j = j + 1;
        }

        if (nr_p == 4)
        {
            winner[board[x][pos] - 1] = 13;
            *win_ln = linie;
            *win_col = coloana;
        }

        j = pos;
        i = x;
        nr_p = 1;

        linie = i;
        coloana = j;

        while ((i < 5) && (board[i + 1][j] == board[i][j]) && (nr_p < 4))
        {
            nr_p++;
            i = i + 1;
        }

        if (nr_p == 4)
        {
            winner[board[x][pos] - 1] = 14;
            *win_ln = linie;
            *win_col = coloana;
        }
        if ( board[x - 1][pos] == 0 )
            x = -1;
    }
    if ( winner[0]!= 0 && winner[1] != 0 )
        return 0;
    else
        if ( winner[ ((*player) - 1) % 2 ] != 0 )
            return winner[ ((*player) - 1) % 2 ];
        else
            if ( winner[ (*player) %2 ] != 0 )
            {
                *player = *player - 1;
                return winner[ ((*player) + 1) % 2 ];
            }
            else
                return 5;
}


//Puts the game on pause
void pauza(time_t* pause_time)
{
  start_color();
  noecho();
  time_t start_pause = time( NULL );
  cbreak();
  init_pair( 1, COLOR_RED, COLOR_WHITE );
  init_pair( 3, COLOR_BLACK, COLOR_WHITE );
  attron( COLOR_PAIR(1) );
  attron( A_BOLD );
  mvaddstr( 15, 58, "The game is paused" );
  mvaddstr( 22, 52, "To resume game press P key" );
  mvaddstr( 23, 52, "                                " );
  mvaddstr( 21, 52, "                                 " );
  refresh();
  int x = 0;
  while ( tolower(x) != '\160' )
  {
	x = getch();
  }
  attroff( A_BOLD );
  attroff( COLOR_PAIR(1) );
  mvaddstr( 15, 58, "                     " );
  *pause_time = *pause_time + ( time( NULL ) - start_pause );
}


//Drops coin from the bottom of the board
int drop_coin( WINDOW* wnd, int pos, int player, int *board, int *culori )
{
    noecho();
    cbreak();
    keypad( wnd, TRUE );
    start_color();
    init_pair( 1, COLOR_GREEN, COLOR_WHITE );
    init_pair( 2, COLOR_RED, COLOR_WHITE );
    init_pair( 3, COLOR_BLACK, COLOR_WHITE );
    init_pair( 9, COLOR_WHITE, COLOR_WHITE );
    init_pair( 10, COLOR_WHITE, COLOR_GREEN );
    init_pair( 11, COLOR_WHITE, COLOR_BLUE );
    init_pair( 12, COLOR_WHITE, COLOR_RED );
    init_pair( 13, COLOR_WHITE, COLOR_YELLOW );
    init_pair( 14, COLOR_WHITE, COLOR_MAGENTA );
    init_pair( 15, COLOR_WHITE, COLOR_CYAN );
    int clri[3];
    clri[0] = -1;
    clri[1] = *culori;
    clri[2] = *(culori + 1);
    int i = 5;
    int count = 0;
    while ((i != 0))
    {
        (*(board + i*7 + pos / 5 )) = (*( board + (i - 1) * 7 + pos / 5));
        i--;
        count++;
    }
    (*(board + pos / 5)) = 0;
    i = 5;
    for (i = 0; i < 6; i++)
    {
        attron( COLOR_PAIR(clri[*(board + i * 7 + pos / 5)] + 10));
        mvaddstr( 4 + i * 3, pos, "    " );
        mvaddstr( 5 + i * 3, pos, "    " );
        attroff( COLOR_PAIR(clri[*(board + i * 7 + pos / 5)] + 10) );
    }
    attroff( COLOR_PAIR(3) );
    attron( COLOR_PAIR(*(culori + (player + 1) % 2) + 10) );
    mvaddstr( 1, pos, "    " );
    mvaddstr( 2, pos, "    " );
    attroff( COLOR_PAIR(*(culori + (player + 1) % 2) + 10) );
    refresh();
    return 1;

}


//Inserts a coin from the upper part of the board
int ins_coin(WINDOW* wnd, int pos, int player, int *board, int *culori)
{
    noecho();
    cbreak();
    keypad( wnd, TRUE );
    start_color();
    init_pair( 1, COLOR_GREEN, COLOR_WHITE );
    init_pair( 2, COLOR_RED, COLOR_WHITE );
    init_pair( 3, COLOR_BLACK, COLOR_WHITE );
    init_pair( 10, COLOR_WHITE, COLOR_GREEN );
    init_pair( 11, COLOR_WHITE, COLOR_BLUE );
    init_pair( 12, COLOR_WHITE, COLOR_RED );
    init_pair( 13, COLOR_WHITE, COLOR_YELLOW );
    init_pair( 14, COLOR_WHITE, COLOR_MAGENTA );
    init_pair( 15, COLOR_WHITE, COLOR_CYAN );
    int i;
    int f_s = 0;
    for (i = 5; i >= 0; i--)
    {

        if ((*(board + i * 7 + pos / 5)) == 0)
        {
            f_s = i + 1;
            (*(board + i * 7 + pos / 5)) = player + 1;
            i = -1;
        }
    }
    if (f_s == 0)
        return 0;
    int ln = 1;
    int count = 0;
    nodelay( wnd, TRUE );
    while ( count != f_s )
    {
        usleep( 100000 );
        attron( COLOR_PAIR(3) );
        mvaddstr( ln, pos, "    " );
        mvaddstr( ln + 1, pos, "    " );
        ln = ln + 3;
        attroff( COLOR_PAIR(3) );
        attron( COLOR_PAIR(*(culori + player) + 10) );
        mvaddstr( ln, pos, "    " );
        mvaddstr( ln + 1, pos, "    " );
        count++;
        refresh();
    }
    attron( COLOR_PAIR(*(culori + (player + 1) % 2) + 10) );
    mvaddstr( 1, pos, "    " );
    mvaddstr( 2, pos, "    " );
    attroff( COLOR_PAIR(*(culori + (player + 1) % 2) + 10) );
    return 1;
}

int get_key_pressed() {
  int ch = getch();
  if (ch != ERR) {
    return ch;
  }
  return -1;
}

// This makes the players select the colors they want to play with
int alege_culori( WINDOW* wnd, int *culori, int tp, char* juc_1, char* juc_2 )
{
    char tip[8];
    if (tp == 1)
        strcpy( tip, "classic" );
    else
        strcpy( tip, "pop-out" );
    curs_set(0);
    clear();
    noecho();
    cbreak();
    int nr_col, nr_rnd;
    getmaxyx(wnd, nr_rnd, nr_col);
    keypad( wnd, TRUE );
    start_color();
    init_pair( 1, COLOR_GREEN, COLOR_BLACK );
    init_pair( 2, COLOR_RED, COLOR_BLACK );
    init_pair( 3, COLOR_WHITE, COLOR_BLACK );
    init_pair( 10, COLOR_WHITE, COLOR_GREEN );
    init_pair( 11, COLOR_WHITE, COLOR_BLUE );
    init_pair( 12, COLOR_WHITE, COLOR_RED );
    init_pair( 13, COLOR_WHITE, COLOR_YELLOW );
    init_pair( 14, COLOR_WHITE, COLOR_MAGENTA );
    init_pair( 15, COLOR_WHITE, COLOR_CYAN );
    wbkgd( wnd, COLOR_PAIR(3) );
    attron( COLOR_PAIR(2) );
    attron( A_BOLD );
    mvaddstr( 2, ((nr_col / 2) - 13), "Start a new");
    mvaddstr( 2, ((nr_col / 2) - 1), tip);
    mvaddstr( 2, ((nr_col / 2) - 1 + strlen(tip)), " game" );
    attroff( A_BOLD );
    attroff( COLOR_PAIR(2) );
    mvaddstr( 6, 2, "Select the colours and names you want to play with" );
    attron( COLOR_PAIR(1) );
    attron( A_BOLD );
    mvaddstr( 8, ((nr_col / 2) - 29), "PLAYER 1" );
    mvaddstr( 10, 2, "Name:" );
    attroff( A_BOLD );
    int i = 0;
    char nume_1[12];
    keypad( wnd,FALSE );
    attron( COLOR_PAIR(3) );
    attron( A_BOLD );
    mvaddstr( 10, 10, "_" );
    while( i < 10 )
    {
        int x = getch();
        x = tolower(x);
        if ( (x >= 97 && x <= 122) || (x >= 48 && x <= 57) )
        {
            nume_1[i] = (char)x;
            mvaddch( 10, 10 + i, toupper(x) );
            mvaddstr( 10, 11 + i, "_" );
            i++;
            if (i == 10)
            {
                nume_1[i] = '\000';
                strcpy( juc_1, nume_1 );
            }
        }
        else
        {
            if ( x == '\n' && i != 0 )
            {
                mvaddstr( 10, 10 + i, " " );
                nume_1[i] = '\000';
                strcpy( juc_1, nume_1 );
                i = 11;
            }
            else
            {
                if ( i - 1 >= 0 )
                {
                    i = i - 1;
                    mvaddstr( 10, 10 + i, "_  " );
                }
            }
        }
    }
    keypad( wnd, TRUE );
    mvaddstr( 10, 20, " " );
    attroff( COLOR_PAIR(3) );
    attroff( A_BOLD );
    attroff( COLOR_PAIR(1) );
    attron( COLOR_PAIR(10) );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    attroff( COLOR_PAIR(10) );
    mvaddstr( 14, 18, ">" );

    int c = 1;
    int cul = 0;
    while(c == 1)
    {
        int p = getch();
        switch ( tolower(p) )
        {
            case KEY_RIGHT:
                if ( cul + 1 <= 5)
                    cul++;
                attron( COLOR_PAIR(cul + 10) );
                mvaddstr( 13, 13, "    " );
                mvaddstr( 14, 13, "    " );
                mvaddstr( 15, 13, "    " );
                attroff( COLOR_PAIR(cul + 10) );
                if (cul != 5)
                    {
                        mvaddstr( 14, 18, ">" );
                        mvaddstr( 14, 11, "<" );
                    }
                else
                    {
                        mvaddstr( 14, 18, " " );
                        mvaddstr( 14, 11, "<" );
                    }
                break;
            case KEY_LEFT:
                if ( cul - 1 >= 0 )
                    cul--;
                attron( COLOR_PAIR(cul + 10) );
                mvaddstr( 13, 13, "    " );
                mvaddstr( 14, 13, "    " );
                mvaddstr( 15, 13, "    " );
                attroff( COLOR_PAIR(cul + 10) );
                if (cul != 0)
                    {
                        mvaddstr( 14, 11, "<" );
                        mvaddstr( 14, 18, ">" );
                    }
                else
                    {
                        mvaddstr( 14, 11, " " );
                        mvaddstr( 14, 18, ">" );
                    }
                break;
            case '\n':
            case '\040':
                (*culori) = cul;
                c = 2;
                break;
        }
    }
    mvaddstr( 14, 11, " " );
    mvaddstr( 14, 18, " " );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    refresh();
    usleep( 450000 );
    attron( COLOR_PAIR((*culori) + 10) );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    attroff( COLOR_PAIR((*culori) + 10) );
    refresh();
    usleep( 450000 );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    refresh();
    usleep( 450000 );
    attron( COLOR_PAIR((*culori) + 10) );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    attroff( COLOR_PAIR((*culori) + 10) );
    refresh();
    usleep( 450000 );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    refresh();
    usleep( 450000 );
    attron( COLOR_PAIR((*culori) + 10) );
    mvaddstr( 13, 13, "    " );
    mvaddstr( 14, 13, "    " );
    mvaddstr( 15, 13, "    " );
    attroff( COLOR_PAIR((*culori) + 10) );
    refresh();

    attron( COLOR_PAIR(1) );
    attron( A_BOLD );
    mvaddstr( 8, ((nr_col / 2) + 20), "PLAYER 2" );
    mvaddstr( 10, ((nr_col / 2) + 11), "Name:" );
    attroff( A_BOLD );
    attroff( COLOR_PAIR(1) );
    attron( COLOR_PAIR(3) );
    attron( A_BOLD );
    mvaddstr( 10, ((nr_col / 2) + 19), "_" );
    i = 0;
    char nume_2[12];
    keypad( wnd, FALSE );
    while(i < 10)
    {
        int x = getch();
        x = tolower(x);
        if ( (x >= 97 && x <= 122) || (x >= 48 &&  x<= 57) )
        {
            nume_2[i] = (char) x;
            mvaddch( 10, ((nr_col / 2) + 19) + i, toupper(x) );
            mvaddstr( 10, ((nr_col / 2) + 19) + 1 + i, "_" );
            i++;
            if (i == 10)
            {
                if ( strncmp(nume_1, nume_2, 10) == 0)
                    {
                        i--;
                        mvaddstr( 10, ((nr_col / 2) + 19) + i, "_  " );
                    }
                else
                {
                    nume_2[i] = '\000';
                    strcpy( juc_2, nume_2 );
                }
            }
        }
        else
        {
            if (x == '\n' && i != 0)
            {
                nume_2[i] = '\000';
                if ( strcmp(nume_1, nume_2) != 0)
                {
                    mvaddstr( 10, ((nr_col / 2) + 19) + i, " " );
                    strcpy( juc_2, nume_2 );
                    i = 11;
                }
            }
            else
            {
                if (i - 1 >= 0)
                {
                    i = i - 1;
                    mvaddstr( 10, ((nr_col / 2) + 19) + i, "_  " );
                }
            }
        }
    }
    keypad( wnd, TRUE );
    mvaddstr( 10, ((nr_col / 2) + 29), "  " );
    attroff( COLOR_PAIR(3) );
    attroff( A_BOLD );
    int cul2 = 0;
    if (cul == 0)
    {
        attron( COLOR_PAIR(11) );
        mvaddstr( 13, 62, "    " );
        mvaddstr( 14, 62, "    " );
        mvaddstr( 15, 62, "    " );
        attroff( COLOR_PAIR(11) );
        mvaddstr( 14, 67, ">" );
        cul2 = 1;
    }
    else
    {
        attron( COLOR_PAIR(10) );
        mvaddstr( 13, 13 + 49, "    " );
        mvaddstr( 14, 13 + 49, "    " );
        mvaddstr( 15, 62 , "    " );
        attroff( COLOR_PAIR(10) );
        mvaddstr( 14, 67, ">" );
        cul2 = 0;
    }
    c = 1;
    while(c == 1)
    {
        int p = getch();
        switch ( tolower(p) )
        {
            case KEY_RIGHT:
                if (((cul2 + 1) <= 5) && ((cul2 + 1) != cul))
                    cul2++;
                else
                    if ((cul2 + 1 == cul) && (cul2 + 2 <=5))
                        cul2 = cul2 + 2;
                attron( COLOR_PAIR(cul2 + 10) );
                mvaddstr( 13, 62, "    " );
                mvaddstr( 14, 62, "    " );
                mvaddstr( 15, 62, "    " );
                attroff( COLOR_PAIR(cul2 + 10) );
                if ( cul2 != 5 )
                {
                    mvaddstr( 14, 67, ">" );
                    mvaddstr( 14, 60, "<" );
                }
                else
                    {
                        mvaddstr( 14, 67, " " );
                        mvaddstr( 14, 60, "<" );
                    }
                refresh();
                break;
            case KEY_LEFT:
                if ((cul2 - 1 >= 0) && (cul2 - 1 != cul))
                    cul2--;
                else
                    if ((cul2 - 1 == cul) && (cul2 - 2 >= 0))
                        cul2 -= 2;
                attron( COLOR_PAIR(cul2 + 10) );
                mvaddstr( 13, 62, "    " );
                mvaddstr( 14, 62, "    " );
                mvaddstr( 15, 62, "    " );
                attroff( COLOR_PAIR(cul2 + 10) );
                if (cul2 != 0)
                    {
                        mvaddstr( 14, 60, "<" );
                        mvaddstr( 14, 67, ">" );
                    }
                else
                    {
                        mvaddstr( 14, 60, " " );
                        mvaddstr( 14, 67, ">" );
                    }
                break;
            case '\n':
            case '\040':
                culori++;
                (*culori) = cul2;
                c = 2;
                break;
        }
    }
    mvaddstr( 14, 60, " " );
    mvaddstr( 14, 67, " " );
    mvaddstr( 13, 62, "    " );
    mvaddstr( 14, 62, "    " );
    mvaddstr( 15, 62, "    " );
    refresh();

    usleep( 450000 );
    attron( COLOR_PAIR((*culori) + 10) );
    mvaddstr( 13, 62, "    " );
    mvaddstr( 14, 62, "    " );
    mvaddstr( 15, 62, "    " );
    attroff( COLOR_PAIR((*culori) + 10) );
    refresh();

    usleep( 450000 );
    mvaddstr( 13, 62, "    " );
    mvaddstr( 14, 62, "    " );
    mvaddstr( 15, 62, "    " );
    refresh();

    usleep( 450000 );
    attron( COLOR_PAIR((*culori) + 10) );
    mvaddstr( 13, 62, "    " );
    mvaddstr( 14, 62, "    " );
    mvaddstr( 15, 62, "    " );
    attroff( COLOR_PAIR((*culori) + 10) );
    refresh();

    usleep( 450000 );
    mvaddstr( 13, 62, "    " );
    mvaddstr( 14, 62, "    " );
    mvaddstr( 15, 62, "    " );
    refresh();

    usleep( 450000 );
    attron( COLOR_PAIR((*culori) + 10) );
    mvaddstr( 13, 62, "    " );
    mvaddstr( 14, 62, "    " );
    mvaddstr( 15, 62, "    " );
    attroff( COLOR_PAIR((*culori) + 10) );
    refresh();

    usleep( 450000 );
    return 0;
}

//Draws an empty game board if a round has been won
int game_won (WINDOW* wnd)
{
    curs_set(0);
    noecho();
    cbreak();
    keypad(wnd,TRUE);
    clear();
    wnd=initscr();
    start_color();
    char strings[2][17];
    strcpy(strings[0],"Runda urmatoare");
    strcpy(strings[1],"Opreste jocul");
    init_pair(3,COLOR_BLACK,COLOR_WHITE);
    init_pair(2,COLOR_RED,COLOR_WHITE);
    init_pair(31,COLOR_GREEN,COLOR_BLACK);
    init_pair(32,COLOR_RED,COLOR_BLACK);
    init_pair(33,COLOR_WHITE,COLOR_BLACK);
    wbkgd(wnd,COLOR_PAIR(33));
    attron(COLOR_PAIR(31));
    attron(A_BOLD);
    mvaddstr(6,15,strings[0]);
    mvaddstr(6,12,"->");
    attroff(A_BOLD);
    mvaddstr(8,15,strings[1]);
    attroff(COLOR_PAIR(31));
    int rnd=6;
    nodelay(wnd,FALSE);
    while(1)
    {
        int x=getch();
        switch(tolower(x))
        {
            case KEY_DOWN:
            case KEY_UP:
            if (rnd==6)
            {
                rnd=8;
                attron(A_BOLD);
                attron(COLOR_PAIR(31));
                mvaddstr(8,15,strings[1]);
                mvaddstr(8,12,"->");
                attroff(A_BOLD);
                mvaddstr(6,15,strings[0]);
                mvaddstr(6,12,"  ");
                attroff(COLOR_PAIR(31));
            }
            else
            {
                rnd=6;
                attron(A_BOLD);
                attron(COLOR_PAIR(31));
                mvaddstr(6,15,strings[0]);
                mvaddstr(6,12,"->");
                attroff(A_BOLD);
                mvaddstr(8,15,strings[1]);
                mvaddstr(8,12,"  ");
                attroff(COLOR_PAIR(31));

            }
            break;
            case '\n':
            case '\040':
            case KEY_RIGHT:
            if (rnd==8)
            {
                return 0;
            }
            else
            {
                clear();
                wnd=initscr();
                wbkgd(wnd,COLOR_PAIR(3));
                attron(COLOR_PAIR(2));
                attron(A_BOLD);
                mvaddstr(1,61,"CONNECT 4");
                attroff(COLOR_PAIR(2));
                attroff(A_BOLD);
                int i=0,j=0;
                for (i=1;i<=36;i++)
                {
                    for (j=3;j<=21;j+=3)
                    {
                        mvaddstr(j,i,"-");
                    }
                }
                for (i=3;i<=21;i++)
                {
                    for (j=1;j<=36;j+=5)
                    {
                        mvaddstr(i,j,"|");
                    }
                }
                for (i=0;i<=24;i++)
                {
                    mvaddstr(i,50,"|");
                }
                return 5;
            }
            break;
        }
    }
}

//This is where all the fun happens
int game(int cond, int game_type)
{
    int win_l=0,win_c=0;
    WINDOW* wnd;
    wnd=initscr();
    curs_set(0);
    clear();
    noecho();
    cbreak();
    keypad(wnd,TRUE);
    int culori[2];
    int joc[3];
    char nume_jucatori[2][12];
    int player=0;
    int board[6][7];
    init_board(&board[0][0]);
    if (cond==0)
    {
        joc[0]=1;
        joc[1]=0;
        joc[2]=0;
        alege_culori(wnd, &culori[0],game_type, &nume_jucatori[0][0], &nume_jucatori[1][0]);
    }
    if (cond==1)
    {
        FILE* load_game;
        FILE* saved_games;
        if (fopen("saved_games","r")!=NULL)
        {
            saved_games=fopen("saved_games","r");
            char buffer[25];
            int i=0;
            char games[10][25];
            clear();
            start_color();
            init_pair(30,COLOR_GREEN,COLOR_BLACK);
            while (fgets(buffer, 28, saved_games)!=NULL)
            {
                attron(COLOR_PAIR(30));
                mvaddstr(i+4,10,buffer);
                attroff(COLOR_PAIR(30));
                strcpy(games[i],buffer);
                i++;
            }
            fclose(saved_games);
            int c=1;
            int row=4;
            init_pair(31,COLOR_RED,COLOR_BLACK);
            init_pair(32,COLOR_GREEN, COLOR_BLACK);
            attron(COLOR_PAIR(31));
            attron(A_BOLD);
            mvaddstr(21,5,"Press ENTER to load selected game");
            mvaddstr(22,5,"Press D to delete selected game");
            mvaddstr(23,5,"Press Q to go back");
             attroff(COLOR_PAIR(31));
            nodelay(wnd,FALSE);
            if (i!=0)
            {
                attron(A_BOLD);
                attron(COLOR_PAIR(30));
                mvaddstr(4,7,"->");
                mvaddstr(4,10,games[row-4]);
                attroff(A_BOLD);
                attroff(COLOR_PAIR(30));
                while (c==1)
                {
                    int x=0;
                    char *term;
                    int p=getch();
                    switch(tolower(p))
                    {
                        case 'q':
                            main_menu(7);
                            return 0;
                        case KEY_UP:
                            attron(COLOR_PAIR(32));
                            mvaddstr(row,7,"  ");
                            mvaddstr(row,10,games[row-4]);
                            if (row-1<=3)
                            {
                                row=i+4-1;
                            }
                            else row--;
                            attron(A_BOLD);
                            mvaddstr(row,7,"->");
                            mvaddstr(row,10,games[row-4]);
                            attroff(A_BOLD);
                            attroff(COLOR_PAIR(32));
                            break;
                        case KEY_DOWN:
                            attron(COLOR_PAIR(32));
                            mvaddstr(row,7,"  ");
                            mvaddstr(row,10,games[row-4]);
                            if(row+1>=i+4)
                            {
                                row=4;
                            }
                            else row++;
                            attron(A_BOLD);
                            mvaddstr(row,7,"->");
                            mvaddstr(row,10,games[row-4]);
                            attroff(A_BOLD);
                            attroff(COLOR_PAIR(32));
                            break;
                        case '\n':
                        case '\040':
                            term=strchr(games[row-4],'\n');
                            (*term)='\000';
                            load_game=fopen(games[row-4],"r");
                            c=0;
                            break;
                        case 'd':
                            term=strchr(games[row-4],'\n');
                            (*term)='\000';
                            remove(games[row-4]);
                            for (x=row-4;x<i-1;x++)
                            {
                                strcpy(games[x],games[x+1]);
                            }
                            i--;
                            clear();
                            if (i==0)
                            {
                                remove("saved_games");
                                init_pair(26,COLOR_RED,COLOR_BLACK);
                                attron(COLOR_PAIR(26));
                                attron(A_BOLD);
                                mvaddstr(10,15,"There are no saved games!");
                                mvaddstr(23,5,"Press any key to go back to the main menu");
                                attroff(A_BOLD);
                                attroff(COLOR_PAIR(26));
                                getch();
                                main_menu(7);
                                return 0;
                            }
                            else
                            {
                                saved_games=fopen("saved_games","w");
                                for (x=0;x<i;x++)
                                {
                                    fprintf(saved_games, "%s", games[x]);
                                    attron(COLOR_PAIR(30));
                                    mvaddstr(x+4,10,"           ");
                                    mvaddstr(x+4,10,games[x]);
                                }
                                fclose(saved_games);
                                attron(A_BOLD);
                                mvaddstr(4,7,"->");
                                mvaddstr(4,10,games[0]);
                                row=4;
                                attroff(A_BOLD);
                                attron(COLOR_PAIR(31));
                                attron(A_BOLD);
                                mvaddstr(21,5,"Press ENTER to load selected game");
                                mvaddstr(22,5,"Press D to delete selected game");
                                mvaddstr(23,5,"Press Q to go back to the main menu");
                                attroff(COLOR_PAIR(31));
                            }
                            break;
                    }
                }
                fread (&board[0][0], sizeof(int), 6*7, load_game);
                fread (&nume_jucatori[0][0], sizeof(char), 2*12, load_game);
                fread (&joc[0], sizeof(int), 3, load_game);
                fread (&culori[0], sizeof (int), 2, load_game);
                fread (&player, sizeof(int), 1, load_game);
                fclose(load_game);
            }
            else
            {
                mvaddstr(10,15,"There are no saved games!");
                mvaddstr(23,5,"Press any key to go back to the main menu");
                getch();
                main_menu(7);
                return 0;
            }
        }
        else
        {
            init_pair(55,COLOR_RED,COLOR_BLACK);
            attron(COLOR_PAIR(55));
            attron(A_BOLD);
            mvaddstr(10,15,"There are no saved games!");
            mvaddstr(23,5,"Press any key to go back to the main menu");
            attroff(COLOR_PAIR(55));
            attroff(A_BOLD);
            getch();
            main_menu(7);
            return 0;
        }

    }
    int start_pts[2];
    start_pts[0]=joc[1];
    start_pts[1]=joc[2];
    clear();
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_WHITE);
    init_pair(2,COLOR_RED,COLOR_WHITE);
    init_pair(3,COLOR_BLACK,COLOR_WHITE);
    init_pair(10,COLOR_WHITE,COLOR_GREEN);
    init_pair(11,COLOR_WHITE,COLOR_BLUE);
    init_pair(12,COLOR_WHITE,COLOR_RED);
    init_pair(13,COLOR_WHITE,COLOR_YELLOW);
    init_pair(14,COLOR_WHITE,COLOR_MAGENTA);
    init_pair(15,COLOR_WHITE,COLOR_CYAN);
    init_pair(20,COLOR_GREEN,COLOR_WHITE);
    init_pair(21,COLOR_BLUE,COLOR_WHITE);
    init_pair(22,COLOR_RED,COLOR_WHITE);
    init_pair(23,COLOR_YELLOW,COLOR_WHITE);
    init_pair(24,COLOR_MAGENTA,COLOR_WHITE);
    init_pair(25,COLOR_CYAN,COLOR_WHITE);
    init_pair(30,COLOR_BLACK,COLOR_BLACK);
    wbkgd(wnd,COLOR_PAIR(3));
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddstr(1,61,"CONNECT 4");
    attroff(COLOR_PAIR(2));
    attroff(A_BOLD);
    refresh();
    int i=0,j=0;
    for (i=1;i<=36;i++)
    {
        for (j=3;j<=21;j+=3)
        {
        mvaddstr(j,i,"-");
        }

    }
    for (i=3;i<=21;i++)
    {
        for (j=1;j<=36;j+=5)
        {
            mvaddstr(i,j,"|");
        }
    }
    for (i=0;i<=24;i++)
    {
        mvaddstr(i,50,"|");
    }
    char buffer[16];
    char name[2][11];
    for (i=0;i<10;i++)
    {
        name[0][i]='\040';
        name[1][i]='\040';
    }
    name[0][10]='\000';
    name[1][10]='\000';
    strncpy(name[0],nume_jucatori[0],strlen(nume_jucatori[0]));
    strncpy(name[1],nume_jucatori[1],strlen(nume_jucatori[1]));
    FILE* scores_file = fopen("scores","a+");
    int scoruri[2];
    scoruri[1]=0;
    scoruri[0]=0;
    while(fgets(buffer, 16, scores_file) != NULL)
    {
        if (strstr(buffer, name[0])!=NULL)
        {
            char *pts=buffer+11;
            while(((*pts)>=48) && ((*pts)<=57))
            {
                scoruri[0]=scoruri[0]*10 + (*pts)-48;
                pts++;
            }
        }
        else
            if (strstr(buffer, name[1])!=NULL)
            {
                char *pts=buffer+11;
                while(((*pts)>=48) && ((*pts)<=57))
                {
                    scoruri[1]=scoruri[1]*10+(*pts)-48;
                    pts++;
                }
            }
    }
    fclose(scores_file);

    attron(COLOR_PAIR(culori[player%2]+10));
    mvaddstr(1,2,"    ");
    mvaddstr(2,2,"    ");
    attroff(COLOR_PAIR(culori[player%2]+10));
    refresh();
    int c=5,pos=2;
    nodelay(wnd,TRUE);
    time_t start_time=time(NULL);
    time_t current_time=time(NULL);
    struct tm *ora_hr, *chron;
    char timp[6],ora_crr[6];
    if (cond==1)
    {
        int rnd=0,col=0;
        for (rnd=0;rnd<6;rnd++)
        {
            for (col=0;col<7;col++)
            {
                if (board[rnd][col] != 0)
                {
                    attron(COLOR_PAIR(culori[(board[rnd][col]+1)%2]+10));
                    mvaddstr (rnd*3+4,col*5+2, "    ");
                    mvaddstr (rnd*3+5,col*5+2, "    ");
                    attroff(COLOR_PAIR(culori[(board[rnd][col]+1)%2]+10));
                }
            }
        }
    }
    int ln=1;
    time_t pause_time=0;
    while(c==5)
    {
        nodelay(wnd,TRUE);
        current_time=time(NULL);
        mvaddstr(3,52,"Time:");
        ora_hr = localtime(&current_time);
        sprintf(ora_crr,"%02d:%02d", ora_hr->tm_hour, ora_hr->tm_min);
        ora_crr[5]=0;
        mvaddstr(3,59,ora_crr);
        mvaddstr(4,52, "You have been playing for:");
        time_t playtime=current_time-start_time-pause_time;
        chron = localtime(&playtime);
        sprintf(timp,"%02d:%02d", chron->tm_min, chron->tm_sec);
        timp[5]=0;
        mvaddstr(5,63, timp);
        char buff[9];
        sprintf(buff,"Round %d", joc[0]);
        mvaddstr(6,52,buff);
        attron(COLOR_PAIR(2));
        attron(A_BOLD);
        mvaddstr(8,62,"SCORES");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(culori[0]+20));
        mvaddstr(9,52,nume_jucatori[0]);
        int len=strlen(nume_jucatori[0]);
        sprintf(buff,"%d", joc[1]);
        mvaddstr(10,52+len/2,buff);
        sprintf(buff,"%d", scoruri[0]);
        mvaddstr(11,52+len/2,buff);
        attroff (COLOR_PAIR(culori[0]+20));
        mvaddstr (10,52+len/2+5, "GAME");
        mvaddstr (11,52+len/2+5, "ALLTIME");
        attron (COLOR_PAIR(culori[1]+20));
        mvaddstr(9,68,nume_jucatori[1]);
        len = strlen(nume_jucatori[1]);
        sprintf(buff,"%d",joc[2]);
        mvaddstr(10,68+len/2,buff);
        sprintf(buff,"%d",scoruri[1]);
        mvaddstr(11,68+len/2,buff);
        attroff (COLOR_PAIR(culori[1]+20));
        attroff(A_BOLD);
        attron(COLOR_PAIR(2));
        attron(A_BOLD);
        mvaddstr(21,52,"To save game press S");
        mvaddstr(22,52,"To pause game press P");
        mvaddstr(23,52,"To quit game press Q");
        attroff(COLOR_PAIR(2));
        attroff(A_BOLD);


        int p = get_key_pressed();
        if (p != -1)
        {
            switch(tolower(p))
            {
                case '\161':
                nodelay(wnd,FALSE);
                clear();
                init_pair(40,COLOR_WHITE, COLOR_BLACK);
                wbkgd(wnd, COLOR_PAIR(40));
                init_pair(41,COLOR_RED, COLOR_BLACK);
                init_pair(42,COLOR_GREEN, COLOR_BLACK);
                attron (A_BOLD);
                attron(COLOR_PAIR(41));
                mvaddstr (10,20,"Do you want to save this game?");
                attroff(COLOR_PAIR(41));
                attron(COLOR_PAIR(42));
                mvaddstr(15,15,"Yes");
                mvaddstr(15,12,"->");
                attroff(A_BOLD);
                mvaddstr(16,15,"No");
                attroff(COLOR_PAIR(42));
                nodelay(wnd,FALSE);
                int y=15;
                while (y!=0)
                {
                    int tasta=getch();
                    switch (tasta)
                    {
                        case KEY_DOWN:
                        case KEY_UP:
                            if (y==15)
                            {
                                y=16;
                                attron(COLOR_PAIR(42));
                                mvaddstr(15,12,"  ");
                                mvaddstr(15,15,"Yes");
                                attron(A_BOLD);
                                mvaddstr(16,12,"-> No");
                                attroff(A_BOLD);
                                attroff(COLOR_PAIR(42));
                            }
                            else
                            {
                                y=15;
                                attron(COLOR_PAIR(42));
                                mvaddstr(16,12,"  ");
                                mvaddstr(16,15,"No");
                                attron(A_BOLD);
                                mvaddstr(15,12,"-> Yes");
                                attroff(A_BOLD);
                                attroff(COLOR_PAIR(42));
                            }
                            break;
                        case '\n':
                        case '\040':
                        case KEY_RIGHT:
                            if (y==15)
                            {
                                save(board, nume_jucatori, joc, culori, player%2);
                                y=0;
                            }
                            else
                            {
                                scores_file = fopen("scores", "r+");
                                int k[2];
                                k[0]=0;
                                k[1]=0;
                                while (fgets(buffer, 16, scores_file) != NULL)
                                {
                                    if ((strstr(buffer,name[0])!=NULL) && (joc[1]!=0))
                                    {
                                        fseek(scores_file, -4, SEEK_CUR);
                                        fprintf(scores_file, "%04d", joc[1]+scoruri[0]);
                                        k[0]=1;
                                    }
                                    if ((strstr(buffer,name[1])!=NULL) && (joc[2]!=0))
                                    {
                                        fseek(scores_file, -4, SEEK_CUR);
                                        fprintf(scores_file, "%04d", joc[2]+scoruri[1]);
                                        k[1]=1;
                                    }
                                }
                                fclose(scores_file);
                                scores_file=fopen("scores","a");
                                if ((k[0]==0) && (joc[1]!=0))
                                {
                                    fprintf(scores_file, "%s %04d\n", name[0],joc[1]);
                                }
                                if ((k[1]==0) && (joc[2]!=0))
                                {
                                    fprintf(scores_file, "%s %04d\n", name[1],joc[2]);
                                }
                                fclose(scores_file);
                                y=0;
                            }
                            break;
                    }
                }
                c=32;
                break;
                case 's':
                save(board,nume_jucatori,joc,culori,player%2);
                attron(A_BOLD);
                attron(COLOR_PAIR(2));
                mvaddstr(14,52, "The game was saved!");
                refresh();
                usleep(300000);
                mvaddstr(14,52, "                    ");
                refresh();
                usleep(300000);
                mvaddstr(14,52, "The game was saved!");
                refresh();
                usleep(300000);
                mvaddstr(14,52, "                    ");
                refresh();
                mvaddstr(14,52, "The game was saved!");
                refresh();
                usleep(300000);
                mvaddstr(14,52, "                    ");
                refresh();
                attroff(A_BOLD);
                attroff(COLOR_PAIR(2));
                break;
                case KEY_UP:
                    if(game_type==2)
                    {
                        if (ln==1)
                        {
                            int player_coin=0;
                            int pos2=0;
                            for (i=0;i<7;i++)
                            {
                                if (board[5][i]==player%2+1)
                                    {
                                        player_coin=1;
                                        pos2=2+5*i;
                                        i=8;
                                    }
                            }
                            if (player_coin==1)
                            {
                                attron(COLOR_PAIR(3));
                                mvaddstr(1,pos,"    ");
                                mvaddstr(2,pos,"    ");
                                attroff(COLOR_PAIR(3));
                                pos=pos2;
                                attron(COLOR_PAIR(30));
                                mvaddstr(19,pos,"    ");
                                mvaddstr(20,pos,"    ");
                                attroff(COLOR_PAIR(30));
                                ln=19;
                            }
                        }
                    else
                        if (ln==19)
                        {

                                attron(COLOR_PAIR(culori[player%2]+10));
                                mvaddstr(1,pos,"    ");
                                mvaddstr(2,pos,"    ");
                                mvaddstr(19,pos,"    ");
                                mvaddstr(20,pos,"    ");
                                attroff(COLOR_PAIR(culori[player%2]+10));
                                ln=1;
                        }
                    }
                    break;
                case KEY_RIGHT:
                    if (ln==1)
                    {
                        if (pos + 5 < 36)
                            {
                                mvaddstr(1,pos,"    ");
                                mvaddstr(2,pos,"    ");
                                pos = pos+5;
                            }
                        attron(COLOR_PAIR(culori[player%2]+10));
                        mvaddstr(1,pos,"    ");
                        mvaddstr(2,pos,"    ");
                        attroff(COLOR_PAIR(culori[player%2]+10));
                    }
                    else
                    if (ln==19)
                    {
                        for (i=(pos+3)/5;i<7;i++)
                        {
                           if (board[5][i]==player%2+1)
                            {
                                attron(COLOR_PAIR(culori[player%2]+10));
                                mvaddstr(19,pos,"    ");
                                mvaddstr(20,pos,"    ");
                                attroff(COLOR_PAIR(culori[player%2]+10));
                                pos=i*5+2;
                                attron(COLOR_PAIR(30));
                                mvaddstr(19,pos,"    ");
                                mvaddstr(20,pos,"    ");
                                attroff(COLOR_PAIR(30));
                                i=7;
                            }
                        }
                    }
                    break;
                case KEY_LEFT:
                    if(ln==1)
                    {
                        if (pos - 5 > 1)
                        {
                            mvaddstr(1,pos,"    ");
                            mvaddstr(2,pos,"    ");
                            pos = pos-5;
                        }
                        attron(COLOR_PAIR(culori[player%2]+10));
                        mvaddstr(1,pos,"    ");
                        mvaddstr(2,pos,"    ");
                        attroff(COLOR_PAIR(culori[player%2]+10));
                    }
                    else
                        if (ln==19)
                        {
                            for (i=(pos-7)/5;i>=0;i--)
                            {
                                if (board[5][i]==player%2+1)
                                {
                                    attron(COLOR_PAIR(culori[player%2]+10));
                                    mvaddstr(19,pos,"    ");
                                    mvaddstr(20,pos,"    ");
                                    attroff(COLOR_PAIR(culori[player%2]+10));
                                    pos=i*5+2;
                                    attron(COLOR_PAIR(30));
                                    mvaddstr(19,pos,"    ");
                                    mvaddstr(20,pos,"    ");
                                    attroff(COLOR_PAIR(30));
                                    i=-1;
                                }
                            }
                        }
                    break;
                case KEY_DOWN:
                case '\040':
                case '\n':
                    if (ln==1)
                    {
                        player=player + ins_coin(wnd,pos,player%2,&board[0][0],&culori[0]);
                        win_c=0;
                        win_l=0;
                        c = check_winner(pos/5,board, &win_l, &win_c, &player);
                    }
                    else
                    if (ln==19)
                    {
                        player=player+drop_coin(wnd,pos,player%2,&board[0][0],&culori[0]);
                        win_c=0;
                        win_l=0;
                        ln=1;
                        c=check_winner(pos/5,board,&win_l, &win_c, &player);
                    }
                    if ((c!=5)&&(c!=0))
                            {
                            time_t win_st_time=time(NULL);
                            show_winner(wnd,win_l,win_c,c,culori[(player-1)%2]+10,player%2, nume_jucatori[(player-1)%2]);
                            refresh();
                            joc[0]++;
                            joc[(player-1)%2+1]++;
                            c=game_won(wnd);
                            if (c==0)
                            {
                                scores_file = fopen("scores", "r+");
                                int k[2];
                                k[0]=0;
                                k[1]=0;
                                while (fgets(buffer, 16, scores_file) != NULL)
                                {
                                    if ((strstr(buffer,name[0])!=NULL) && (joc[1]!=0))
                                    {
                                        fseek(scores_file, -4, SEEK_CUR);
                                        fprintf(scores_file, "%04d", joc[1]+scoruri[0]-start_pts[0]);
                                        k[0]=1;
                                    }
                                    if ((strstr(buffer,name[1])!=NULL) && (joc[2]!=0))
                                    {
                                        fseek(scores_file, -4, SEEK_CUR);
                                        fprintf(scores_file, "%04d", joc[2]+scoruri[1]-start_pts[1]);
                                        k[1]=1;
                                    }
                                }
                                fclose(scores_file);
                                scores_file=fopen("scores","a");
                                if ((k[0]==0) && (joc[1]!=0))
                                {
                                    fprintf(scores_file, "%s %04d\n", name[0],joc[1]-start_pts[0]);
                                }
                                if ((k[1]==0) && (joc[2]!=0))
                                {
                                    fprintf(scores_file, "%s %04d\n", name[1],joc[2]-start_pts[1]);
                                }
                                fclose(scores_file);
                                nodelay(wnd,FALSE);
                                clear();
                                init_pair(40,COLOR_WHITE, COLOR_BLACK);
                                wbkgd(wnd, COLOR_PAIR(40));
                                init_pair(41,COLOR_RED, COLOR_BLACK);
                                init_pair(42,COLOR_GREEN, COLOR_BLACK);
                                attron (A_BOLD);
                                attron(COLOR_PAIR(41));
                                mvaddstr (10,20,"Do you want to save this game?");
                                attroff(COLOR_PAIR(41));
                                attron(COLOR_PAIR(42));
                                mvaddstr(15,15,"Yes");
                                mvaddstr(15,12,"->");
                                attroff(A_BOLD);
                                mvaddstr(16,15,"No");
                                attroff(COLOR_PAIR(42));
                                int y=15;
                                while (y!=0)
                                {
                                    int tasta=getch();
                                    switch (tasta)
                                    {
                                        case KEY_DOWN:
                                        case KEY_UP:
                                            if (y==15)
                                            {
                                                y=16;
                                                attron(COLOR_PAIR(42));
                                                mvaddstr(15,12,"  ");
                                                mvaddstr(15,15,"Yes");
                                                attron(A_BOLD);
                                                mvaddstr(16,12,"-> No");
                                                attroff(A_BOLD);
                                                attroff(COLOR_PAIR(42));
                                            }
                                            else
                                            {
                                                y=15;
                                                attron(COLOR_PAIR(42));
                                                mvaddstr(16,12,"  ");
                                                mvaddstr(16,15,"No");
                                                attron(A_BOLD);
                                                mvaddstr(15,12,"-> Yes");
                                                attroff(A_BOLD);
                                                attroff(COLOR_PAIR(42));
                                            }
                                            break;
                                        case '\n':
                                        case '\040':
                                        case KEY_RIGHT:
                                            if (y==15)
                                            {
                                                init_board(&board[0][0]);
                                                save(board, nume_jucatori, joc, culori, player%2);
                                                y=0;
                                            }
                                            else
                                            {
                                                y=0;
                                            }
                                            break;
                                    }
                                }
                            }
                            init_board(&board[0][0]);
                            pos=2;
                            player=(joc[0]-1)%2;
                            attron(COLOR_PAIR(culori[player%2]+10));
                            mvaddstr(1,2,"    ");
                            mvaddstr(2,2,"    ");
                            attroff(COLOR_PAIR(culori[player%2]+10));
                            pause_time=pause_time + (time(NULL)-win_st_time);
                            refresh();
                        }
                    else
                    {
                        if (c==0)
                        {
                            time_t rem_time=time(NULL);
                            attron(A_BOLD);
                            attron(COLOR_PAIR(2));
                            mvaddstr(14,52, "DRAW!");
                            mvaddstr(15,52, "Press ENTER key");
                            attroff(A_BOLD);
                            attroff(COLOR_PAIR(2));
                            refresh();
                            while(get_key_pressed()!='\n');
                            c=game_won(wnd);
                            init_board(&board[0][0]);
                            pos=2;
                            player=(joc[0]-1)%2;
                            attron(COLOR_PAIR(culori[player%2]+10));
                            mvaddstr(1,2,"    ");
                            mvaddstr(2,2,"    ");
                            attroff(COLOR_PAIR(culori[player%2]+10));
                            pause_time=pause_time+(time(NULL)-rem_time);
                            refresh();
                        }
                    }
                    break;
                case 'p':
                    pauza(&pause_time);
                    mvaddstr(22,52,"To pause the game press P    ");
                    mvaddstr(21,52,"To save the game press S     ");
                    mvaddstr(23,52,"To quit the game press Q     ");
                    nodelay(wnd,TRUE);
                    break;
            }
        }
    }
    main_menu(6);
    return 0;
}

//Helps the user select between classic game or pop-out game
int new_game(int load)
{
    char game_type[2][8];
    strcpy(game_type[0], "Classic");
    strcpy(game_type[1], "Pop-out");
    WINDOW* wnd;
    wnd=initscr();
    int nr_rnd, nr_col;
    getmaxyx(wnd,nr_rnd,nr_col);
    curs_set(0);
    clear();
    noecho();
    cbreak();
    keypad(wnd,TRUE);
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    init_pair(3,COLOR_WHITE,COLOR_BLACK);
    wbkgd(wnd,COLOR_PAIR(3));

    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddstr(2,((nr_col/2)-8), "Start a new game");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(2));
    mvaddstr(6,((nr_col/2)-22), "Select your desired game mode");
    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    int rnd=8;
    mvaddstr(8,((nr_col/2)-15), game_type[0]);
    mvaddstr(8,((nr_col/2)-15-3), "->");
    attroff(A_BOLD);
    mvaddstr(9,((nr_col/2)-15), game_type[1]);
    refresh();
    while(1)
    {
        int p=getch();
        switch (tolower(p))
        {
            case KEY_UP:
                mvaddstr(rnd,((nr_col/2)-15), game_type[rnd-8]);
                mvaddstr(rnd,((nr_col/2)-18), "   ");
                if (rnd -1 < 8)
                    rnd=9;
                else
                    rnd--;
                attron(A_BOLD);
                mvaddstr(rnd,((nr_col/2)-15), game_type[rnd-8]);
                mvaddstr(rnd,((nr_col/2)-18), "->");
                attroff (A_BOLD);
                refresh();
                break;
            case KEY_DOWN:
                mvaddstr(rnd,((nr_col/2)-15), game_type[rnd-8]);
                mvaddstr(rnd,((nr_col/2)-18), "   ");
                if (rnd + 1 > 9)
                    rnd=8;
                else
                    rnd++;
                attron(A_BOLD);
                mvaddstr(rnd,((nr_col/2)-15), game_type[rnd-8]);
                mvaddstr(rnd,((nr_col/2)-18), "->");
                attroff (A_BOLD);
                refresh();
                break;
            case '\n':
            case '\040':
                if (load==0)
                {
                    if (rnd==8)
                    {
                        game(0,1);
                    }
                    else
                        game(0,2);
                    return 0;
                }
                else
                {
                    if (rnd==8)
                    {
                        game(1,1);
                    }
                    else
                        game(1,2);
                }
                return 0;
        }
    }
    return 0;
}

//meniu in care sunt afisate cateva reguli ale jocului
int rules()
{
    char menu_strings[6][22];
    strcpy(menu_strings[0],"Start a new game");
    strcpy(menu_strings[1],"Load a saved game");
    strcpy(menu_strings[2],"Game rules");
    strcpy(menu_strings[3],"Great Wall of Fame");
    strcpy(menu_strings[4],"Quit");
    WINDOW* wnd;
    wnd=initscr();
    int nr_rnd, nr_col;
    getmaxyx(wnd, nr_rnd, nr_col);
    curs_set(0);
    clear();
    noecho();
    cbreak();
    start_color();
    keypad(wnd,TRUE);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    init_pair(3,COLOR_WHITE,COLOR_BLACK);
    wbkgd(wnd, COLOR_PAIR(3));
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddstr(2,((nr_col/2)-8), "Game rules");
    attroff(COLOR_PAIR(2));
    attroff(A_BOLD);
    mvaddstr(4,3,"Connect 4 is a logic and strategy game meant for 2 players.");
    mvaddstr(5,3,"Each player will try to arrange for of his coins in a row, column or");
    mvaddstr(6,3,"diagonally. Before the game starts the two players can select their desired");
    mvaddstr(7,3,"names and the colours they want to play with. Also, there are two game");
    mvaddstr(8,3,"modes available: classic, where the players can enter coins from the top;");
    mvaddstr(9,3,"pop-out, where they are allowed to extract coins from the bottom.");
    mvaddstr(13,3,"How to play:");
    mvaddstr(14,19,"Move coin to the left");
    mvaddstr(15,19,"Move coin to the right");
    mvaddstr(16,19,"Select extract/insert mode");
    mvaddstr(17,19,"Insert/Extract coin");
    mvaddstr(19,6,"Quit");
    mvaddstr(20,6,"Save current game");
    mvaddstr(21,6,"Pause");
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddstr(nr_rnd-1,3,"Press any key to go back to the main menu");
    mvaddstr(14,3,"Left arrow");
    mvaddstr(15,3,"Right arrow");
    mvaddstr(16,3,"Up arrow");
    mvaddstr(17,3,"Down arrow");
    mvaddstr(19,3,"Q");
    mvaddstr(20,3,"S");
    mvaddstr(21,3,"P");
    attroff(COLOR_PAIR(2));
    attroff(A_BOLD);
    refresh();
    getch();
    main_menu(8);
    return 0;
}

//Main Menu
int main_menu(int rnd)
{
    WINDOW* wnd;
    wnd = initscr();
    int nr_rnd, nr_col;
    char menu_strings[6][22];
    strcpy(menu_strings[0],"Start a new game");
    strcpy(menu_strings[1],"Load a saved game");
    strcpy(menu_strings[2],"Game rules");
    strcpy(menu_strings[3],"Great Wall of Fame");
    strcpy(menu_strings[4],"Quit");
    getmaxyx(wnd, nr_rnd, nr_col);
    curs_set (0);
    clear();
    noecho();
    cbreak();
    keypad(wnd,TRUE);
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    wbkgd(wnd, COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddstr(2,((nr_col/2)-8), "Welcome!");
    attroff(COLOR_PAIR(2));
    attroff(A_BOLD);
    mvaddstr(6,((nr_col/2)-15), menu_strings[0]);
    mvaddstr(7,((nr_col/2)-15), menu_strings[1]);
    mvaddstr(8,((nr_col/2)-15), menu_strings[2]);
    mvaddstr(9,((nr_col/2)-15), menu_strings[3]);
    mvaddstr(10,((nr_col/2)-15), menu_strings[4]);
    attron(A_BOLD);
    mvaddstr(rnd,((nr_col/2)-15), menu_strings[rnd-6]);
    mvaddstr(rnd,((nr_col/2)-18), "->");
    attron(COLOR_PAIR(2));
    mvaddstr(nr_rnd-1,3, "To move through the menu use UP/DOWN or ENTER keys");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(2));
    refresh();
    int c=1;
    int p;
    while(c==1)
    {
        p=getch();
        switch (tolower(p))
        {
            case KEY_UP:
                mvaddstr(rnd,((nr_col/2)-15-3), "   ");
                mvaddstr(rnd,((nr_col/2)-15), menu_strings[rnd-6]);
                if (rnd-1 < 6)
                    rnd=10;
                else rnd--;
                attron(A_BOLD);
                mvaddstr(rnd,((nr_col/2)-15-3), "->");
                mvaddstr(rnd,((nr_col/2)-15), menu_strings[rnd-6]);
                attroff(A_BOLD);
                refresh();
                break;
            case KEY_DOWN:
                mvaddstr(rnd,((nr_col/2)-15-3), "   ");
                mvaddstr(rnd,((nr_col/2)-15), menu_strings[rnd-6]);
                if (rnd+1 > 10)
                    rnd=6;
                else rnd++;
                attron(A_BOLD);
                mvaddstr(rnd,((nr_col/2)-15-3), "->");
                mvaddstr(rnd,((nr_col/2)-15), menu_strings[rnd-6]);
                attroff(A_BOLD);
                refresh();
                break;
            case '\n':
            case '\040':
            case KEY_RIGHT:
                switch (rnd)
                {
                    case 6:
                        new_game(0);return 0;
                    case 7:
                        new_game(1);return 0;
                    case 8:
                        rules();return 0;;
                    case 9:
                        great_wall(wnd);return 0;
                    case 10:
                        c=2;endwin();return 0;
                }
                break;
        }

    }
    endwin();
    return 0;
}

//functia main
int main (void)
{

    main_menu(6);
    echo();
    endwin();
    return 0;
}
