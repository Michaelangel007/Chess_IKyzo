/*
You can download a .ttf Unicode chess font from
* https://github.com/xeyownt/chess_merida_unicode

Also see
* https://stackoverflow.com/questions/27483800/displaying-unicode-chess-pieces-in-windows-console
* https://superuser.com/questions/390933/how-to-add-a-font-to-the-cmd-window-choices-in-windows-7-64-bit/956818#956818

*/

#define VERIFY_SAFE_BOARD_ARRAY 1

// Shutup stupid MSVC crap
#ifdef _WIN32
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

// Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <locale.h>
    #include <wchar.h>
    #include <string.h>  // memcpy()
    #include <stdbool.h> // bool
    #include <time.h>
    #include <assert.h>
#ifdef _WIN32
    #include <conio.h>
    #include <io.h>    // _setmode()
    #include <fcntl.h> // _O_U16TEXT = 0x00020000
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

// Types
    // Eliminated pieces
    typedef struct eliminated_pieces
    {
        char                      piece_type;
        struct eliminated_pieces *next;
    } eliminated_piece;

    // Generating lists
    typedef struct eliminated_pieces_lists
    {
        eliminated_piece *start;
    } eliminated_pieces_list;

    enum
    {
        PLAYER_WHITE,
        PLAYER_BLACK
    };

// Globals

#if VERIFY_SAFE_BOARD_ARRAY
    char board[9][9];
    char possible_board[9][9];
#else
    char board[8][8];
    char possible_board[8][8];
#endif

    #define MAX_POSSIBLE_MOVES 100
    int ga_possible_moves[ MAX_POSSIBLE_MOVES ];
    int gn_possible_moves;
    int previous_position;
    int new_position = 0;

    eliminated_pieces_list eliminated_pieces_white;
    eliminated_pieces_list eliminated_pieces_black;

#if VERIFY_SAFE_BOARD_ARRAY
    const char BOARD_INIT[9][9] = // array 9x9 representing the board with padding for OOB detection
    {
        { 'R', 'H', 'C', 'Q', 'K', 'C', 'H', 'R', '?' },
        { 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', '?' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p', '?' },
        { 'r', 'h', 'c', 'q', 'k', 'c', 'h', 'r', '?' },
        { '?', '?', '?', '?', '?', '?', '?', '?', '?' }
    };
#else
    const char BOARD_INIT[8][8] = // array 8x8 representing the board.
    {
        { 'R', 'H', 'C', 'Q', 'K', 'C', 'H', 'R' },
        { 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' },
        { 'r', 'h', 'c', 'q', 'k', 'c', 'h', 'r' }
    };
#endif

// Prototypes
    void bishop( int , int, int );
    bool cell_has_black_piece( int row, int col );
    bool cell_has_white_piece( int row, int col );
    void change( int , int , int , int, int);
    void clear_screen();
    eliminated_piece* create_piece(char value);
    void delay(int);
    void display_board();
    int  display_convert(char ); // convert array to Unicode
    void display_eliminated_list(eliminated_pieces_list l);
    void display_possible_board();
    void insert_eliminated_list(char value, eliminated_pieces_list *l);
    void intro();
    void king( int , int , int);
    void knight(int , int, int );
    void pawn(int , int );
    void pawnb( int , int );
    void player_black();
    void player_white();
    void queen( int , int , int );
    void rook(int , int, int);
    void showcase_board(char, char, char, char, int);
    void showcase_game();
    void update_possible_moves();
    bool verify_possible_move (int position);

// Utility

    // ----------------------------------------
    bool is_empty (int row, int col)
    {
        return (board[row][col] == ' ');
    }

    // ----------------------------------------
    int is_off_board ( int row, int col )
    {
        if ((row < 0) || (row > 7)) return true;
        if ((col < 0) || (col > 7)) return true;
        return false;
    }

    // ----------------------------------------
    void position_to_row_col ( int position, int *row, int *col )
    {
       *row = position / 10;
       *col = position % 10;
    }

    // ----------------------------------------
    void row_col_to_position ( int row, int col, int *position )
    {
        *position = (row * 10) + col;
    }

// Implementation

// ----------------------------------------
void bishop ( int r1 , int c1, int player)
{
    int a , b;
    gn_possible_moves = 0;

    if (player == PLAYER_BLACK)
    {
        a = 1 , b = 1;
        while ((board[r1-a][c1+b] == ' ') || cell_has_white_piece(r1-a, c1+b))
        {
            if (((r1-a) == -1) || ((c1+b) == 8) )
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1+b;
            if (cell_has_white_piece(r1-a, c1+b))
                break;
            //wprintf(L"%d%d , " , r1-a , c1+b );
            a++;
            b++;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1-b] == ' ') || cell_has_white_piece(r1+a, c1-b))
        {
            if (((r1+a) == 8) || ((c1-b) == -1))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1-b;
            if (cell_has_white_piece(r1+a, c1-b))
                break ;
            //wprintf(L"%d%d , " , r1+a , c1-b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1+b] == ' ') || cell_has_white_piece(r1+a, c1+b))
        {
            if (((r1+a) == 8) || ((c1+b) == 8))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1+b;
            if (cell_has_white_piece(r1+a, c1+b))
                break ;
            //wprintf(L"%d%d , " , r1+a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 ;
        b = 1 ;
        while ((board[r1-a][c1-b] == ' ') || cell_has_white_piece(r1-a, c1-b))
        {
            if (((r1-a) == -1) || ((c1-b) == -1))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1-b;
            if (cell_has_white_piece(r1-a, c1-b))
                break ;
            //wprintf(L"%d%d , " , r1-a , c1-b ) ;
            a++ ;
            b++ ;
        }
    }
    else
    {
        a = 1 , b = 1 ;
        while ((board[r1-a][c1+b] == ' ') || cell_has_black_piece(r1-a, c1+b))
        {
            if (((r1-a) == -1) || ((c1+b) == 8))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1+b;
            if (cell_has_black_piece(r1-a, c1+b))
                break ;
            //wprintf(L"%d%d , " , r1-a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1-b] == ' ') || cell_has_black_piece(r1+a, c1-b))
        {
            if (((r1+a) == 8) || ((c1-b) == -1))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1-b;
            if (cell_has_black_piece(r1+a, c1-b))
                break ;
            //wprintf(L"%d%d , " , r1+a , c1-b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while( board[r1+a][c1+b] == ' ' || cell_has_black_piece(r1+a, c1+b))
        {
            if (((r1+a) == 8) || ((c1+b) == 8))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1+b;
            if (cell_has_black_piece(r1+a, c1+b))
                break ;
            //wprintf(L"%d%d , " , r1+a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 ;
        b = 1 ;
        while ((board[r1-a][c1-b] == ' ') || cell_has_black_piece(r1-a, c1-b))
        {
            if (((r1-a) == -1) || ((c1-b) == -1))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1-b;
            if (cell_has_black_piece(r1-a, c1-b))
                break ;
            //wprintf(L"%d%d , " , r1-a , c1-b ) ;
            a++ ;
            b++ ;
        }
    }

    wprintf( L"Possible moves %lc: \n", display_convert(board[r1][c1]) );
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf( L" %d :", ga_possible_moves[i] );
        }
    }
    else
    {
        wprintf( L"No possible moves -> " );
    }

}

#ifdef _WIN32
// ----------------------------------------
void cls (HANDLE hConsole)
{
    COORD coordScreen = { 0, 0 };    // home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with blanks.
    if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
                                    (TCHAR)' ',      // Character to write to the buffer
                                    dwConSize,       // Number of cells to write
                                    coordScreen,     // Coordinates of first cell
                                    &cCharsWritten)) // Receive number of characters written
    {
        return;
    }

    // Get the current text attribute.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Set the buffer's attributes accordingly.
    if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute
                                    coordScreen,      // Coordinates of first cell
                                    &cCharsWritten))  // Receive number of characters written
    {
        return;
    }

    // Put the cursor at its home coordinates.
    SetConsoleCursorPosition(hConsole, coordScreen);
}
#endif

// ----------------------------------------
void change ( int r1 , int c1 , int r2 , int c2 , int player )
{
    char temp ;
    temp = board[r1][c1] ;

    if (player == PLAYER_BLACK)
    {
        if (cell_has_white_piece(r2, c2)) // black captures white
        {
            insert_eliminated_list(board[r2][c2], &eliminated_pieces_white);
            board[r1][c1] = ' ';
            board[r2][c2] = temp;
        }
        else
        {
            board[r1][c1] = board[r2][c2] ;
            board[r2][c2] = temp ;
        }
    }
    else
    {
        if (cell_has_black_piece(r2, c2)) // white captures black
        {
            insert_eliminated_list(board[r2][c2], &eliminated_pieces_black);
            board[r1][c1] = ' ';
            board[r2][c2] = temp;
        }
        else
        {
            board[r1][c1] = board[r2][c2] ;
            board[r2][c2] = temp ;
        }
    }
}

// Check if there is a white piece at row,col
// ----------------------------------------
bool cell_has_white_piece (int row , int col )
{
    switch( board[row][col] )
    {
        case '?': { int OUT_OF_ARRAY_BOUNDS = 0; assert( OUT_OF_ARRAY_BOUNDS ); exit(0); }
        case 'p':              // intentional fall-through
        case 'r':              // intentional fall-through
        case 'h':              // intentional fall-through
        case 'c':              // intentional fall-through
        case 'k':              // intentional fall-through
        case 'q': return true; // intentional fall-through
        default : return false;
    }
}

// Test if there is a black piece at row,col
// ----------------------------------------
bool cell_has_black_piece ( int row , int col )
{
    switch( board[row][col] )
    {
        case '?': { int OUT_OF_ARRAY_BOUNDS = 0; assert( OUT_OF_ARRAY_BOUNDS ); exit(0); }
        case 'P':              // intentional fall-through
        case 'R':              // intentional fall-through
        case 'H':              // intentional fall-through
        case 'C':              // intentional fall-through
        case 'K':              // intentional fall-through
        case 'Q': return true; // intentional fall-through
        default : return false;
    }
}

bool is_cell_occupied (int row, int col, int player)
{
    if (player == PLAYER_WHITE)
        return cell_has_white_piece( row, col );
    else
        return cell_has_black_piece( row, col );
}

// ----------------------------------------
void clear_screen ()
{
#if _WIN32

    // This is a security nightmare. DON'T DO THIS!!!
    //     system( "cls" );
    // See:. :-/
    // * https://learn.microsoft.com/en-us/windows/console/clearing-the-screen. :-/
    //. :-/
    // Not everyone has a smart terminal with ANSI support. :-/
    //     HANDLE hStdOut  = GetStdHandle(STD_OUTPUT_HANDLE);
    //     DWORD  written  = 0;
    //     PCWSTR sequence = L"\x1b[2J"; // ANSI codes: ESC [ 2J
    //     WriteConsoleW(hStdOut, sequence, (DWORD)wcslen(sequence), &written, NULL);
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    cls(hStdout);
#else
    // TODO: Fixme *nix
    // This is a security nightmare. DON'T DO THIS!!!
    //     system( "clear" );
#endif // _WIN32
}

// Generating piece node
// ----------------------------------------
eliminated_piece* create_piece (char value)
{
    eliminated_piece *piece;
    if ((piece = (eliminated_piece *)malloc(sizeof(eliminated_piece))) != NULL)
    {
        piece->piece_type = value;
        piece->next = NULL;
    }
    return piece;
}

// ----------------------------------------
void delay (int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    clock_t start_time = clock();
    clock_t end_time   = start_time + milli_seconds;

    // looping till required time is not achieved
    while (clock() < end_time)
        ; // intentional busy-waiting
}

// ----------------------------------------
void display_board ()
{
    int x , y;

    display_eliminated_list(eliminated_pieces_white);

    wprintf(L" ") ;
    for (x = 0 ; x < 8; x++)
    {
        wprintf(L"   %d" , x );
    }
    wprintf(L"\n" ) ;

    const wchar_t *separator = L"  ----------------------------------\n";

    for (y = 0; y < 8; y++)
    {
        wprintf( separator );
        wprintf(L"%d " , y ) ;

        for( x = 0 ; x < 8 ; x++ )
        {
            wprintf(L"||%lc " , display_convert(board[y][x]) ) ;
        }
        wprintf(L"|| \n" ) ;
    }
    wprintf( separator );

    display_eliminated_list(eliminated_pieces_black);
    wprintf(L"\n");
}

// ----------------------------------------
int display_convert (char symbol)
{
    static const wchar_t turn[2] =
    {
#ifdef _WIN32
        0x26AB, 0x26AA
#else
        0x26AA, 0x26AB
#endif
    };

    switch (symbol)
    {
        case 'p': return 0x265F; // White pawn
        case 'r': return 0x265C; //       rook
        case 'h': return 0x265E; //       knight
        case 'c': return 0x265D; //       bishop
        case 'k': return 0x265A; //       king
        case 'q': return 0x265B; //       queen
        case 'P': return 0x2659; // Black pawn
        case 'R': return 0x2656; //       rook
        case 'H': return 0x2658; //       knight
        case 'C': return 0x2657; //       bishop
        case 'K': return 0x2654; //       king
        case 'Q': return 0x2655; //       queen
        case 'x': return 0x2716; // Possible move
        case '!': return 0x26A0; // /!\ Warning Sign
        case 'W': return turn[0]; // White's Turn
        case 'B': return turn[1]; // Black's Turn
        default : return 0x0020; // Empty
    }
}

// ----------------------------------------
void display_eliminated_list (eliminated_pieces_list l)
{
    eliminated_piece* temp = l.start;
    int converted_value;

    while (temp != NULL)
    {
        converted_value = display_convert(temp->piece_type);
        wprintf( L"%lc ", (wchar_t)converted_value );
        temp = temp->next;
    }
    wprintf( L"\n" );
}

void display_possible_board()
{
    int x , y;

    display_eliminated_list(eliminated_pieces_white);

    wprintf(L" ") ;
    for (x = 0 ; x < 8; x++)
    {
        wprintf(L"   %d" , x );
    }
    wprintf(L"\n" ) ;

    const wchar_t *separator = L"  ----------------------------------\n";

    for (y = 0; y < 8; y++)
    {
        wprintf( separator );
        wprintf(L"%d " , y ) ;

        for( x = 0 ; x < 8 ; x++ )
        {
            wprintf(L"||%lc " , display_convert( possible_board[y][x]) );
        }
        wprintf(L"|| \n" ) ;
    }
    wprintf( separator );

    display_eliminated_list(eliminated_pieces_black);
    wprintf(L"\n");
}

#ifdef _WIN32
    #define getch _getch
#else
// Reads a single character, including arrow keys
// ----------------------------------------
char getch ()
{
    struct termios oldt, newt;
    char ch, seq[3];

    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read the first character
    read(STDIN_FILENO, &ch, 1);

    if (ch == '\033') // Escape sequence
    {
        // Read the next two characters
        if (read(STDIN_FILENO, &seq[0], 1) == 0 || read(STDIN_FILENO, &seq[1], 1) == 0)
        {
            ch = '\033'; // Treat as ESC if sequence is incomplete
        }
        else
            if (seq[0] == '[')
            {
                // Interpret arrow keys
                switch (seq[1])
                {
                    case 'A': ch =     -1; break; // Up arrow
                    case 'B': ch =     -2; break; // Down arrow
                    case 'C': ch =     -3; break; // Right arrow
                    case 'D': ch =     -4; break; // Left arrow
                    default : ch = '\033'; break; // Unknown sequence
                }
            }
            else
            {
                ch = '\033'; // Unknown sequence
            }
    }

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
#endif // !_WIN32

// ----------------------------------------
void insert_eliminated_list (char value, eliminated_pieces_list *l)
{
    eliminated_piece *temp;
    eliminated_piece *piece;
    piece = create_piece(value);

    // Empty list
    if (l->start == NULL)
    {
        l->start    = piece;
        piece->next = NULL;
    }
    else // list not Empty
    {
        temp = l->start;
        while (temp->next!=NULL)
        {
            temp = temp->next;
        }
        temp->next=piece;
        piece->next=NULL;
    }
}

// ----------------------------------------
void intro ()
{
    wprintf(
L"\n"
"      __\n"
"     /  \\\n"
"     \\__/                             _                      \n"
"    /____\\             __          __| |__   ___  ___ ___    \n"
"     |__|            / __| ___   / __| '_ \\ / _ \\/ __/ __|  \n"
"    (====)          | (__ |___| | (__| | | |  __/\\__ \\__ \\ \n"
"    }===={           \\___|       \\___|_| |_|\\___||___/___/ - Kahouli Wajd. 死\n"
"  (________) \n"
    );
}

// ----------------------------------------
void king ( int r1 , int c1, int player )
{
    gn_possible_moves  = 0;

    if (player == PLAYER_BLACK)
    {
        if ((board[r1][c1+1] == ' ') || cell_has_white_piece(r1, c1+1))
        {
            if ((r1 >= 0) && (r1 <= 7) && (c1+1 >= 0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = r1*10+c1+1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }

        if ((board[r1][c1-1] == ' ') || cell_has_white_piece(r1, c1-1))
        {
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
            if ((r1 >= 0) && (r1 <= 7) && (c1-1 >= 0) && (c1-1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = r1*10+c1-1;
            }
        }

        if ((board[r1-1][c1] == ' ') || cell_has_white_piece(r1-1, c1))
        {
            if ((r1-1 >= 0) && (r1-1 <= 7) && (c1 >= 0) && (c1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }

        if ((board[r1-1][c1+1] == ' ') || cell_has_white_piece(r1-1, c1+1))
        {
            if ((r1-1 >= 0) && (r1-1 <= 7) && (c1+1 >= 0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1+1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }
        if ((board[r1-1][c1-1] == ' ') || cell_has_white_piece(r1-1, c1-1))
        {
            if ((r1-1 >= 0) && (r1-1 <= 7) && (c1-1 >=0) && (c1-1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1-1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }

        if ((board[r1+1][c1] == ' ') || cell_has_white_piece(r1+1, c1))
        {
            if ((r1+1 >= 0) && (r1+1 <= 7) && (c1 >=0) && (c1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }

        if ((board[r1+1][c1+1] == ' ') || cell_has_white_piece(r1+1, c1+1))
        {
            if ((r1+1 >= 0) && (r1+1 <= 7) && (c1+1 >=0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1+1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }

        if ((board[r1+1][c1-1] == ' ') || cell_has_white_piece(r1+1, c1-1))
        {
            if ((r1+1 >= 0) && (r1+1 <= 7) && (c1-1 >=0) && (c1-1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1-1;
            }
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
        }
    }
    else
    {
        if ((r1 >= 0) && (r1 <= 7) && (c1+1 >= 0) && (c1+1 <= 7))
        {
            if ((board[r1][c1+1] == ' ') || cell_has_black_piece(r1, c1+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = r1*10+c1+1;
            }
        }
        if ((r1 >= 0) && (r1 <= 7) && (c1-1 >= 0) && (c1-1 <= 7))
        {
            //wprintf(L"%d%d , " , r1 , c1+1 ) ;
            if ((board[r1][c1-1] == ' ') || cell_has_black_piece(r1, c1-1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = r1*10+c1-1;
            }
        }
        if ((r1-1 >= 0) && (r1-1 <= 7) && (c1 >= 0) && (c1 <= 7))
        {
            if ((board[r1-1][c1] == ' ') || cell_has_black_piece(r1-1, c1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1;
            }
        }
        if ((r1-1 >= 0) && (r1-1 <= 7) && (c1+1 >= 0) && (c1+1 <= 7))
        {
            if ((board[r1-1][c1+1] == ' ') || cell_has_black_piece(r1-1, c1+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1+1;
            }
        }
        if ((r1-1 >= 0) && (r1-1 <= 7) && (c1-1 >= 0) && (c1-1 <= 7))
        {
            if ((board[r1-1][c1-1] == ' ') || cell_has_black_piece(r1-1, c1-1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1-1;
            }
        }
        if ((r1+1 >= 0) && (r1+1 <= 7) && (c1 >=0) && (c1 <= 7))
        {
            if ((board[r1+1][c1] == ' ') || cell_has_black_piece(r1+1, c1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1;
            }
        }
        if ((r1+1 >= 0) && (r1+1 <= 7) && (c1+1 >= 0) && (c1+1 <= 7))
        {
            if ((board[r1+1][c1+1] == ' ') || cell_has_black_piece(r1+1, c1+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1+1;
            }
        }
        if ((r1+1 >= 0) && (r1+1 <= 7) && (c1-1 >= 0) && (c1-1 <= 7))
        {
            if ((board[r1+1][c1-1] == ' ') || cell_has_black_piece(r1+1, c1-1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1-1;
            }
        }
    }

    wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}

// ----------------------------------------
void knight ( int r1 , int c1, int player )
{
    gn_possible_moves = 0;

    if (player == PLAYER_BLACK)
    {
        if ((board[r1+2][c1+1] == ' ') || cell_has_white_piece(r1+2, c1+1))
        {
            if ((r1+2 <= 7) && (r1+2 >= 0) && (c1+1 >= 0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+2)*10+c1+1;
            }

            //wprintf(L"%d%d, " , r1+2 ,c1+1) ;
        }

        if ((board[r1+2][c1-1] == ' ') || cell_has_white_piece(r1+2, c1-1))
        {
            if (r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <= 7)
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+2)*10+c1-1;
            }
        }

        if ((board[r1+1][c1+2] == ' ') || cell_has_white_piece(r1+1, c1+2))
        {
            if ((r1+1 <= 7) && (r1+1 >= 0) && (c1+2 >= 0) && (c1+2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1+2;
            }
        }
        if ((board[r1-1][c1+2] == ' ') || cell_has_white_piece(r1-1, c1+2))
        {
            if ((r1-1 <= 7) && (r1-1 >= 0) && (c1+2 >= 0) && (c1+2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1+2;
            }
        }

        if ((board[r1-2][c1-1] == ' ') || cell_has_white_piece(r1-2, c1-1))
        {
            if ((r1-2 <= 7) && (r1-2 >= 0) && (c1-1 >= 0) && (c1-1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-2)*10+c1-1;
            }
        }

        if ((board[r1-2][c1+1] == ' ') || cell_has_white_piece(r1-2, c1+1))
        {
            if ((r1-2 <= 7) && (r1-2 >= 0) && (c1+1 >= 0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-2)*10+c1+1;
            }
        }

        if ((board[r1+1][c1-2] == ' ') || cell_has_white_piece(r1+1, c1-2))
        {
            if ((r1+1 <= 7) && (r1+1 >= 0) && (c1-2 >= 0) && (c1-2 <=7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1-2;
            }
        }

        if ((board[r1-1][c1-2] == ' ') || cell_has_white_piece(r1-1, c1-2))
        {
            if ((r1-1 <= 7) && (r1-1 >= 0) && (c1-2 >= 0) && (c1-2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1-2;
            }
        }
    }
    else
    {
        if ((board[r1+2][c1+1] == ' ') || cell_has_black_piece(r1+2, c1+1))
        {
            if ((r1+2 <= 7) && (r1+2 >= 0) && (c1+1 >= 0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+2)*10+c1+1;
            }

            //wprintf(L"%d%d, " , r1+2 ,c1+1);
        }

        if ((board[r1+2][c1-1] == ' ') || cell_has_black_piece(r1+2, c1-1))
        {
            if ((r1+2 <= 7) && (r1+2 >= 0) && (c1-1 >= 0) && (c1-1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+2)*10+c1-1;
            }
        }

        if ((board[r1+1][c1+2] == ' ') || cell_has_black_piece(r1+1, c1+2))
        {
            if ((r1+1 <= 7) && (r1+1 >= 0) && (c1+2 >= 0) && (c1+2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1+2;
            }
        }

        if ((board[r1-1][c1+2] == ' ') || cell_has_black_piece(r1-1, c1+2))
        {
            if ((r1-1 <= 7) && (r1-1 >= 0) && (c1+2 >= 0) && (c1+2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1+2;
            }
        }

        if ((board[r1-2][c1-1] == ' ') || cell_has_black_piece(r1-2, c1-1))
        {
            if ((r1-2 <= 7) && (r1-2 >= 0) && (c1-1 >= 0) && (c1-1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-2)*10+c1-1;
            }
        }

        if ((board[r1-2][c1+1] == ' ') || cell_has_black_piece(r1-2, c1+1))
        {
            if ((r1-2 <= 7) && (r1-2 >= 0) && (c1+1 >= 0) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-2)*10+c1+1;
            }
        }

        if ((board[r1+1][c1-2] == ' ') || cell_has_black_piece(r1+1, c1-2))
        {
            if ((r1+1 <= 7) && (r1+1 >= 0) && (c1-2 >= 0) && (c1-2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1-2;
            }
        }

        if ((board[r1-1][c1-2] == ' ') || cell_has_black_piece(r1-1, c1-2))
        {
            if ((r1-1 <= 7) && (r1-1 >= 0) && (c1-2 >= 0) && (c1-2 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1-2;
            }
        }
    }

    wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}

// ----------------------------------------
void pawn ( int r1 , int c1 )
{
    gn_possible_moves = 0;

    if (r1 == 1) // starting black row
    {
        if (board[r1+1][c1] == ' ')
        {
            //wprintf(L"%d%d , " , r1+1 , c1 );
            if ((r1+1 <= 7) && (c1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1;
            }
        }

        if (board[r1+2][c1] == ' ')
        {
            //wprintf(L"%d%d , " , r1+2 , c1 );
            if ((r1+2 <= 7) && (c1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+2)*10+c1;
            }
        }

        if (cell_has_white_piece(r1+1 , c1+1)) // en passant right
        {
             //wprintf(L"%d%d* , " , r1+1 , c1+1 );
             if ((r1+1 <= 7) && (c1+1 <= 7))
             {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1+1;
             }
        }

        if (cell_has_white_piece(r1+1 , c1-1)) // en passant left
        {
            //wprintf(L"%d%d* , " , r1+1 , c1-1 );
            if ((r1+1 <= 7) && (c1-1 <= 7) && (c1-1 >=0))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1-1;
            }
        }
    }
    else
    {
        if (board[r1+1][c1] == ' ')
        {
            //wprintf(L"%d%d , " , r1+1 , c1 ) ;
            if ((r1+1 <= 7) && (c1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1;
            }
        }

        if (cell_has_white_piece(r1+1 , c1+1))
        {
            //wprintf(L"%d%d* , " , r1+1 , c1+1 ) ;
            if ((r1+1 <= 7) && (c1+1 <= 7))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1+1;
            }
        }

        if (cell_has_white_piece(r1+1 , c1-1))
        {
            //wprintf(L"%d%d* , " , r1+1 , c1-1 ) ;
            if ((r1+1 <= 7) && (c1-1 <= 7) && (c1-1 >=0))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1+1)*10+c1-1;
            }
        }
    }

    wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}

// ----------------------------------------
void pawnb ( int r1 , int c1 )
{
    gn_possible_moves = 0;

    if (r1 == 6) // starting white row
    {
        if (board[r1-1][c1] == ' ')
        {
            //wprintf(L"%d%d , " , r1-2 , c1 ) ;
            if( r1-1 >= 0 && c1 >= 0 )
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1;
            }
        }

        if (board[r1-2][c1] == ' ')
        {
            //wprintf(L"%d%d , " , r1-2 , c1 ) ;
            if( r1-2 >= 0 && c1 >= 0 )
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-2)*10+c1;
            }
        }

        if (cell_has_black_piece(r1-1 , c1+1)) // en passant right
        {
             //wprintf(L"%d%d* , " , r1+1 , c1+1 );
             if( r1-1 >= 0 && c1+1 >= 0 )
             {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1+1;
             }
        }

        if (cell_has_black_piece(r1-1 , c1-1)) // en passant left
        {
            //wprintf(L"%d%d* , " , r1+1 , c1-1 );
            if( r1-1 >= 0 && c1-1 <= 7 && c1-1 >=0) // check for outside board
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1-1;
            }
        }
    }
    else
    {
        if (board[r1-1][c1] == ' ')
        {
            //wprintf(L"%d%d , " , r1-1 , c1 ) ;
            if( r1-1 >= 0 && c1 >= 0 )
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1;
            }
        }

        if (cell_has_black_piece(r1-1 , c1+1))
        {
             //wprintf(L"%d%d* , " , r1+1 , c1+1 );
             if( r1-1 >= 0 && c1+1 >= 0 )
             {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1+1;
             }
        }

        if (cell_has_black_piece(r1-1 , c1-1))
        {
            //wprintf(L"%d%d* , " , r1+1 , c1-1 );
            if ((r1-1 >= 0) && (c1-1 <= 7) && (c1-1 >=0)) // check for outside board
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1-1)*10+c1-1;
            }
        }
    }

    wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}

// ----------------------------------------
void player_white ()
{
    int player = PLAYER_WHITE;
    int p1 , p2 , c1 , r1 , c2 , r2, input_control;

    wprintf( L"%lc White to move ...\n", display_convert( 'W' ) ) ;

again2:
    do
    {
        fflush(stdin);
        wprintf(L"Enter position of piece to move [row col]: " );
        input_control = scanf( "%d" , &p1 );

        position_to_row_col( p1, &r1, &c1 );
        if (is_off_board(r1, c1))
        {
            if (is_off_board(r1, 0))
                wprintf( L"%lc Invalid row. Must be 0..7\n", display_convert( '!' ) );
            else
                wprintf( L"%lc Invalid column. Must be 0..7\n", display_convert( '!' ) );
        }
    } while ( input_control==0 || p1 < 10 || p1 > 77 || p1%10 > 7 );


    switch( board[r1][c1] )
    {
        case 'p': pawnb ( r1 , c1         ); break ;
        case 'r': rook  ( r1 , c1, player ); break ;
        case 'h': knight( r1 , c1, player ); break ;
        case 'c': bishop( r1 , c1, player ); break ;
        case 'k': king  ( r1 , c1, player ); break ;
        case 'q': queen ( r1 , c1, player ); break ;
        default:
            wprintf( L"%lc Invalid Position! ", display_convert( '!' ) );
            goto again2 ;
    }

    if (gn_possible_moves)
    {
        clear_screen();
        update_possible_moves();
        wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }

        do
        {
            wprintf(L"\nEnter new position of piece [row column]: " );
            scanf( "%d" , &p2 ) ;
        } while (verify_possible_move(p2));
    }
    else
    {
        goto again2;
    }

    c2 = p2 % 10 ;
    r2 = p2 / 10  ;

    previous_position = r1*10+c1;
    new_position = r2*10+c2;

    change(r1,c1,r2,c2, player);
}

// ----------------------------------------
void player_black ()
{
    int player = PLAYER_BLACK;
    int p1 , p2 , c1 , r1 , c2 , r2, input_control;

    wprintf( L"%lc Black to move ...", display_convert( 'B' ) );

again1:
    do
    {
        fflush(stdin);
        wprintf(L"\nEnter position of piece to move [row column]: " );
        input_control = scanf( "%d" , &p1 );

        position_to_row_col( p1, &r1, &c1 );
        if (is_off_board(r1, c1))
        {
            if (is_off_board(r1, 0))
                wprintf( L"%lc Invalid row. Must be 0..7\n", display_convert( '!' ) );
            else
                wprintf( L"%lc Invalid column. Must be 0..7\n", display_convert( '!') );
        }
    } while ((input_control == 0) || (p1!=0 && p1>7&&p1 < 10) || p1 > 77 || p1%10 > 7 );

    switch( board[r1][c1] ) // Select only player's pieces.
    {
        case 'P': pawn  ( r1 , c1         ); break;
        case 'R': rook  ( r1 , c1, player ); break;
        case 'H': knight( r1 , c1, player ); break;
        case 'C': bishop( r1 , c1, player ); break;
        case 'K': king  ( r1 , c1, player ); break;
        case 'Q': queen ( r1 , c1, player ); break;
        default:
            wprintf( L"%lc Invalid Position! ", display_convert( '!' ) );
            goto again1 ;
    }

    if (gn_possible_moves)
    {
        clear_screen();
        update_possible_moves();
        wprintf( L"Possible moves %lc: \n", display_convert(board[r1][c1]) );
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }

        do
        {
            wprintf( L"\nEnter new position of piece [row column]: " );
            scanf( "%d" , &p2 ) ;
        } while (verify_possible_move(p2));
    }
    else
    {
        goto again1;
    }

    c2 = p2 % 10 ;
    r2 = p2 / 10  ;
    previous_position = r1*10+c1;
    new_position = r2*10+c2;

    change(r1,c1,r2,c2, player);
}

// ----------------------------------------
void queen ( int r1 , int c1, int player )
{
    int a, b;
    int n = c1;

    gn_possible_moves = 0;

    if (player == PLAYER_BLACK)
    {
        a = 1 , b = 1 ;
        while ((board[r1-a][c1+b] == ' ') || cell_has_white_piece(r1-a, c1+b))
        {
            if ((r1-a) == -1 || ((c1+b) == 8))
                break ;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1+b;
            if (cell_has_white_piece(r1-a, c1+b))
                break;
            //wprintf(L"%d%d , " , r1-a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1-b] == ' ') || cell_has_white_piece(r1+a, c1-b))
        {
            if ((r1+a) == 8 || (c1-b) == -1)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1-b;
            if (cell_has_white_piece(r1+a, c1-b))
                break;
            //wprintf(L"%d%d , " , r1+a , c1-b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1+b] == ' ') || cell_has_white_piece(r1+a, c1+b))
        {
            if ((r1+a) == 8 || (c1+b) == 8)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1+b;
            if (cell_has_white_piece(r1+a, c1+b))
                break;
            //wprintf(L"%d%d , " , r1+a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 ;
        b = 1 ;
        while ((board[r1-a][c1-b] == ' ') || cell_has_white_piece(r1-a, c1-b))
        {
            if ((r1-a) == -1 || (c1-b) == -1)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1-b;
            if (cell_has_white_piece(r1-a, c1-b))
                break;
            //wprintf(L"%d%d , " , r1-a , c1-b ) ;
            a++ ;
            b++ ;
        }

        if (n != 0)
        {
            while ((board[r1][n-1] == ' ') || cell_has_white_piece(r1, n-1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n-1;
                if (cell_has_white_piece(r1, n-1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = c1;
        if (n != 7)
        {
            while ((board[r1][n+1] == ' ') || cell_has_white_piece(r1, n+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n+1;
                if (cell_has_white_piece(r1, n+1))
                    break;
                //wprintf(L"%d%d , " , r1 , n+1 ) ;
                n++;
            }
        }

        n = r1 ;
        if (n != 0)
        {
            while ((board[n-1][c1] == ' ') || cell_has_white_piece(n-1, c1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n-1)*10+c1;
                if (cell_has_white_piece(n-1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = r1;
        if (n != 7)
        {
            while ((board[n+1][c1] == ' ') || cell_has_white_piece(n+1, c1))
            {
                if (n == 7)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n+1)*10+c1;
                if (cell_has_white_piece(n+1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n++ ;
            }
        }
    }
    else
    {
        a = 1 , b = 1 ;
        while ((board[r1-a][c1+b] == ' ') || cell_has_black_piece(r1-a, c1+b))
        {
            if ((r1-a) == -1 || (c1+b) == 8)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1+b;
            if (cell_has_black_piece(r1-a, c1+b))
                break;
            //wprintf(L"%d%d , " , r1-a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1-b] == ' ') || cell_has_black_piece(r1+a, c1-b))
        {
            if ((r1+a) == 8 || (c1-b) == -1)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1-b;
            if (cell_has_black_piece(r1+a, c1-b))
                break;
            //wprintf(L"%d%d , " , r1+a , c1-b ) ;
            a++ ;
            b++ ;
        }

        a = 1 , b = 1 ;
        while ((board[r1+a][c1+b] == ' ') || cell_has_black_piece(r1+a, c1+b))
        {
            if ((r1+a) == 8 || (c1+b) == 8)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1+a)*10+c1+b;
            if (cell_has_black_piece(r1+a, c1+b))
                break;
            //wprintf(L"%d%d , " , r1+a , c1+b ) ;
            a++ ;
            b++ ;
        }

        a = 1 ;
        b = 1 ;
        while ((board[r1-a][c1-b] == ' ') || cell_has_black_piece(r1-a, c1-b))
        {
            if ((r1-a) == -1 || (c1-b) == -1)
                break;
            ga_possible_moves[ gn_possible_moves++ ] = (r1-a)*10+c1-b;
            if (cell_has_black_piece(r1-a, c1-b))
                break;
            //wprintf(L"%d%d , " , r1-a , c1-b ) ;
            a++ ;
            b++ ;
        }

        if (n != 0)
        {
            while ((board[r1][n-1] == ' ') || cell_has_black_piece(r1, n-1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n-1;
                if (cell_has_black_piece(r1, n-1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = c1;
        if (n != 7)
        {
            while ((board[r1][n+1] == ' ') || cell_has_black_piece(r1, n+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n+1;
                if (cell_has_black_piece(r1, n+1))
                    break;
                //wprintf(L"%d%d , " , r1 , n+1 ) ;
                n++;
            }
        }

        n = r1;
        if (n != 0)
        {
            while ((board[n-1][c1] == ' ') || cell_has_black_piece(n-1, c1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n-1)*10+c1;
                if (cell_has_black_piece(n-1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = r1;
        if (n != 7)
        {
            while ((board[n+1][c1] == ' ') || cell_has_black_piece(n+1, c1))
            {
                if( n == 7 )
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n+1)*10+c1;
                if (cell_has_black_piece(n+1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n++ ;
            }
        }
    }

    wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}

// ----------------------------------------
void rook ( int r1 , int c1, int player)
{
    gn_possible_moves = 0;

    int n = c1;
    if (player == PLAYER_BLACK)
    {
        if (n != 0)
        {
            while ((board[r1][n-1] == ' ') || cell_has_white_piece(r1, n-1))
            {
                if (n == 0)
                     break;
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n-1;
                if (cell_has_white_piece(r1, n-1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = c1;
        if (n != 7)
        {
            while ((board[r1][n+1] == ' ')|| cell_has_white_piece(r1, n+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n+1;
                if (cell_has_white_piece(r1, n+1))
                    break;
                //wprintf(L"%d%d , " , r1 , n+1 ) ;
                n++;
            }
        }

        n = r1;
        if (n != 0)
        {
            while ((board[n-1][c1] == ' ') || cell_has_white_piece(n-1, c1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n-1)*10+c1;
                if (cell_has_white_piece(n-1, c1))
                     break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = r1;
        if (n != 7)
        {
            while ((board[n+1][c1] == ' ') || cell_has_white_piece(n+1, c1))
            {
                if (n == 7)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n+1)*10+c1;
                if (cell_has_white_piece(n+1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n++ ;
            }
        }
    }
    else
    {
        if (n != 0)
        {
            while ((board[r1][n-1] == ' ') || cell_has_black_piece(r1, n-1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n-1;
                if (cell_has_black_piece(r1, n-1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = c1;
        if (n != 7)
        {
            while ((board[r1][n+1] == ' ') || cell_has_black_piece(r1, n+1))
            {
                ga_possible_moves[ gn_possible_moves++ ] = (r1)*10+n+1;
                if (cell_has_black_piece(r1, n+1))
                    break;
                //wprintf(L"%d%d , " , r1 , n+1 ) ;
                n++;
            }
        }

        n = r1;
        if (n != 0)
        {
            while ((board[n-1][c1] == ' ') || cell_has_black_piece(n-1, c1))
            {
                if (n == 0)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n-1)*10+c1;
                if (cell_has_black_piece(n-1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n-- ;
            }
        }

        n = r1;
        if (n != 7)
        {
            while ((board[n+1][c1] == ' ') || cell_has_black_piece(n+1, c1))
            {
                if (n == 7)
                    break;
                ga_possible_moves[ gn_possible_moves++ ] = (n+1)*10+c1;
                if (cell_has_black_piece(n+1, c1))
                    break;
                //wprintf(L"%d%d , " , r1 , n-1 ) ;
                n++ ;
            }
        }
    }

    wprintf(L"Possible moves %lc: \n", display_convert(board[r1][c1]));
    if (gn_possible_moves)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            wprintf(L" %d :", ga_possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}

// ----------------------------------------
void showcase_board (char old_row, char old_col, char new_row, char new_col, int player)
{
    int c1 , r1 , c2 , r2;

    r1 = (int)(old_row - '0');
    c1 = (int)(old_col - '0');
    r2 = (int)(new_row - '0');
    c2 = (int)(new_col - '0');

    if ((r1 == 0) && (c1 == 0) && (r2 == 0) && (c2 == 0) && (player == PLAYER_WHITE)) // castle
    {
        change(7,4,7,6, PLAYER_WHITE); // king
        change(7,7,7,5, PLAYER_WHITE); // rook
    }
    else
    if ((r1 == 0) && (c1 == 0) && (r2 == 0) && (c2 == 0) && (player == PLAYER_BLACK)) // castle
    {
        change(0,4,0,2, PLAYER_BLACK); // king
        change(0,0,0,3, PLAYER_BLACK); // rook
    }
    else
    {
        change(r1,c1,r2,c2, player);
        display_board();
    }
}

/*
   File Format for chess_game_notes.txt

      #Comment that will be displayed
      ?##.##-##.##

    That is,

      <any> <WhiteOldRow><WhiteOldCol> '.' <WhiteNewRow><WhiteNewCol> <any> <BlackOldRow><BlackOldCol> '.' <BlackNewRow><BlackNewCol>

   1. Where # is 0..7
   2. Where ## is RowCol. 64 is Row 6, Col 4.
   3. Format is OldRC.NewRC
   4. There must be a pair of white / black moves on a single line.

   i.e.

      ?64.44-14.34
*/
// ----------------------------------------
void showcase_game ()
{
    const char *filename = "chess_game_notes.txt";
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
         perror("Unable to open file!\n");
         perror( filename );
         exit(1);
    }

    #define MAX_LINE 128
    char chunk[MAX_LINE];
    char white_input[2];
    char white_output[2];
    char black_input[2];
    char black_output[2];

    while(fgets(chunk, sizeof(chunk)-1, fp) != NULL)
    {
        chunk[MAX_LINE-1] = 0;
        if (chunk[0] == '#') // Add support for line comments
        {
           for( int i = 1; i < MAX_LINE; i++ )
           {
               if( chunk[i] )
                   wprintf( L"%c", (wchar_t) chunk[i] );
               else
                   break;
           }
           fflush(stdout);
           continue;
        }

        white_input[0] = chunk[1];
        white_input[1] = chunk[2];
        white_output[0] = chunk[4];
        white_output[1] = chunk[5];
        wprintf(L"%c%c.%c%c\n", white_input[0], white_input[1],white_output[0], white_output[1]);

        showcase_board(white_input[0], white_input[1], white_output[0], white_output[1], PLAYER_BLACK);
        delay(2);

        black_input[0] = chunk[7];
        black_input[1] = chunk[8];
        black_output[0] = chunk[10];
        black_output[1] = chunk[11];
        wprintf(L"%c%c.%c%c\n", black_input[0], black_input[1], black_output[0], black_output[1]);
        showcase_board(black_input[0], black_input[1], black_output[0], black_output[1], PLAYER_WHITE);
        delay(2);

        // marker string used to show where the content of the chunk array has ended
    }

    fclose(fp);
}

// ----------------------------------------
void update_possible_moves ()
{
    int x,y;

    for (int i=0; i < gn_possible_moves; i++)
    {
        position_to_row_col( ga_possible_moves[i], &y, &x );
        if (is_empty( y, x ))
            possible_board[y][x] = 'x';
    }
    display_possible_board();
}

// ----------------------------------------
bool verify_possible_move (int position)
{
    for (int i = 0; i < gn_possible_moves; i++)
    {
        if (position == ga_possible_moves[i])
            return false;
    }
    return true;
}

// ========================================
void main (int argc, char *argv[])
{
    // Init lists
    eliminated_pieces_white.start = NULL;
    eliminated_pieces_black.start = NULL;

    char ch;
    char showcase_option;

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT); // Unicode encoding
#else
    setlocale(LC_ALL,"");
#endif
    if ((argc > 1) && (strcmp(argv[1], "-test") == 0))
    {
        const char *pieces = "prhcqk\nPRHCQK\nx";
        size_t len = strlen( pieces );
        for (size_t i = 0; i < len; i++ )
        {
            wchar_t glyph = (wchar_t) display_convert( pieces[ i ] );
            wprintf( L"%lc", glyph );
        }
        wprintf( L"\n" );
        wprintf( L"\n\x26AB White (Win32)" );
        wprintf( L"\n\x26AA Black (Win32)" );
        wprintf( L"\n\x26AA White (Unix )" );
        wprintf( L"\n\x26AB Black (Unix )" );
        return;
    }

    clear_screen();
    intro();
    wprintf(L" \nPress Any Key To Continue...\nOr press SPACE to replay chess_game_notes.txt\n" );
    showcase_option = getch();
    clear_screen();

    if (showcase_option == ' ') // Space is Showcase game
    {
        showcase_game();
    }
    else
    {
        int moves = 0;
        memcpy( board, BOARD_INIT, sizeof(BOARD_INIT) );

        do
        {
            memcpy( possible_board, board, sizeof(board) );
            clear_screen();
            display_board();

            if ((moves % 2) == PLAYER_WHITE)
            {
                player_white();
            }
            else
            {
                player_black();
            }
            moves++;

            wprintf( L"Press Any Key To Continue or ESC to quit... " );
            ch = getch();
        } while (ch != 27);
    }
}
