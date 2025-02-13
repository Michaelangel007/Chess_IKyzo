/*
You can download a .ttf Unicode chess font from:

DejaVuSansMono.ttf
* https://dejavu-fonts.github.io/

chess_merida_unicode.ttf
* https://github.com/xeyownt/chess_merida_unicode

You may want to use the ConEmu terminal on Windows
* https://conemu.github.io/

Also see:
* https://stackoverflow.com/questions/27483800/displaying-unicode-chess-pieces-in-windows-console
* https://superuser.com/questions/390933/how-to-add-a-font-to-the-cmd-window-choices-in-windows-7-64-bit/956818#956818
*/

#define VERIFY_SAFE_BOARD_ARRAY 0
#define INPUT_ALGEBRAIC         1

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
    enum players_t
    {
        PLAYER_WHITE,
        PLAYER_BLACK,
        NUM_PLAYERS
    };

    enum pieces_e
    {
        PIECE_NONE,
        PIECE_PAWN,
        PIECE_ROOK,
        PIECE_BISHOP,
        PIECE_KNIGHT,
        PIECE_QUEEN,
        PIECE_KING,
        NUM_PIECES
    };

// Globals

#if VERIFY_SAFE_BOARD_ARRAY
    #define BOARD_DIMENSION 10
#else
    #define BOARD_DIMENSION 8
#endif
    char ga_board_position[BOARD_DIMENSION][BOARD_DIMENSION];
    char ga_board_possible[BOARD_DIMENSION][BOARD_DIMENSION];

    const char ga_pieces[ NUM_PLAYERS ][ NUM_PIECES ] =
    {
        { ' ', 'p', 'r', 'h', 'c', 'q', 'k' }, // white
        { ' ', 'P', 'R', 'H', 'C', 'Q', 'K' }  // black
    };

    const char *ga_piece_names[ NUM_PIECES ] =
    {
        "piece" , // PIECE_NONE
        "pawn"  , // PIECE_PAWN
        "rook"  , // PIECE_ROOK
        "knight", // PIECE_KNIGHT
        "bishop", // PIECE_BISHOP
        "queen" , // PIECE_QUEEN
        "king"    // PIECE_KING
    };

    // castling support
    bool ga_king_moved  [ NUM_PLAYERS ];
    bool ga_rook_k_moved[ NUM_PLAYERS ];
    bool ga_rook_q_moved[ NUM_PLAYERS ];

    #define MAX_POSSIBLE_MOVES 100
    // This is a packed row,col -- see row_col_to_position() and position_to_row_col()
    int ga_possible_moves[ MAX_POSSIBLE_MOVES ];
    int gn_possible_moves;

    #define MAX_PIECES 32
    char ga_eliminated_pieces[ NUM_PLAYERS ][ MAX_PIECES ];
    int  gn_eliminated_pieces[ NUM_PLAYERS ];

#if VERIFY_SAFE_BOARD_ARRAY
    const char BOARD_INIT[BOARD_DIMENSION][BOARD_DIMENSION] = // array + padding for array OOB detection
    {
        { '?', '?', '?', '?', '?', '?', '?', '?', '?', '?' },
        { '?', 'R', 'H', 'C', 'Q', 'K', 'C', 'H', 'R', '?' },
        { '?', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', '?' },
        { '?', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { '?', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { '?', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { '?', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '?' },
        { '?', 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p', '?' },
        { '?', 'r', 'h', 'c', 'q', 'k', 'c', 'h', 'r', '?' },
        { '?', '?', '?', '?', '?', '?', '?', '?', '?', '?' }
    };
#else
    const char BOARD_INIT[8][8] = // array 8x8 representing the board.
    {                                               //row algebraic
        { 'R', 'H', 'C', 'Q', 'K', 'C', 'H', 'R' }, // 0  8
        { 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' }, // 1  7
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }, // 2  6
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }, // 3  5
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }, // 4  4
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }, // 5  3
        { 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' }, // 6  2
        { 'r', 'h', 'c', 'q', 'k', 'c', 'h', 'r' }  // 7  1
    };  // 0    1    2    3    4    5    6    7 col
        // A    B    C    D    E    F    G    H algebraic
#endif


#if INPUT_ALGEBRAIC
    const char ga_board_rows[] = "87654321";
    const char ga_board_cols[] = "ABCDEFGH";
#else
   const char ga_board_rows[] = "01234567";
   const char ga_board_cols[] = "01234567";
#endif

    const char *ga_player_colors[NUM_PLAYERS] =
    {
        "WHITE",
        "BLACK"
    };

// Movement Axes
    enum axis_e
    {
        AXIS_E , // +x
        AXIS_NE, // +x -y
        AXIS_N , //    -y
        AXIS_NW, // -x -y
        AXIS_W , // -x
        AXIS_SW, // -x +y
        AXIS_S , //    +y
        AXIS_SE, // +x +y
        NUM_AXIS
    };

    //                                        E  NE   N  NW   W  SW   S  SE
    const int axis_delta_col[ NUM_AXIS ] = { +1, +1,  0, -1, -1, -1,  0, +1 };
    const int axis_delta_row[ NUM_AXIS ] = {  0, -1, -1, -1,  0, +1, +1, +1 };

// Prototypes
    void add_possible_array( int positions, const int *rows, const int *cols, int row, int col, int player );
    void add_possible_axis( int row, int col, int player, int axis );
    void add_possible_move( int row, int col, int opponent );
    int  cell_get_piece( int row, int col );
    bool cell_has_black_piece( int row, int col );
    bool cell_has_player(int row, int col, int player);
    bool cell_has_white_piece( int row, int col );
    void clear_screen();
    void delay(int);
    void display_board( char board[BOARD_DIMENSION][BOARD_DIMENSION] );
    void display_board_header();
    int  get_glyph( char ); // convert char to Unicode
    void display_eliminated( int player );
    void display_position_board();
    void display_possible_board();
    void intro();
    bool is_empty( int row, int col );
    int  is_off_board( int row, int col );
    void move_piece( int, int, int, int, int );
    void moves_bishop( int , int, int player );
    void moves_king( int , int , int player );
    void moves_knight(int , int, int player );
    void moves_pawn( int r1, int c1, int player );
    void moves_queen( int , int , int player );
    void moves_rook(int , int, int player );
    int  player_input_row_col( int player, int from_piece );
    void player_turn( int player );
    void player_turn_algebraic ( int player );
    void player_turn_integer( int player );
    void position_to_row_col( int position, int *row, int *col );
    int row_col_to_position( int row, int col );
    void showcase_board(char, char, char, char, int);
    void showcase_game();
    void update_possible_moves( int row, int col );

// Utility

    // ----------------------------------------
    void add_possible_array ( int positions, const int *rows, const int *cols, int row, int col, int player )
    {
        const int opponent = 1 - player;
        for( int position = 0; position < positions; position++ )
        {
            int r = row + rows[ position ];
            int c = col + cols[ position ];
            add_possible_move( r, c, opponent );
        }
    }

    // ----------------------------------------
    void add_possible_axis ( int row, int col, int player, int axis )
    {
        const int opponent = 1 - player;
        const int dx = axis_delta_col[ axis ];
        const int dy = axis_delta_row[ axis ];

        int r = row;
        int c = col;
        for( int position = 0; position < 7; position++ )
        {
            r += dy;
            c += dx;
            if (is_off_board(r,c))
                return;
            else
            if (cell_has_player(r, c, opponent ))
            {
                ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(r,c);
                return;
            }
            else
            if (!is_empty(r,c))
                return; // cell must be our own color
            else
                ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(r,c);
        }
    }

    // ----------------------------------------
    void add_possible_move ( int row, int col, int opponent )
    {
        if ( !is_off_board(row,col) && (is_empty(row,col) || cell_has_player(row,col,opponent)) )
           ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(row,col);
    }

    // ----------------------------------------
    int cell_get_piece ( int row, int col )
    {
#if VERIFY_SAFE_BOARD_ARRAY
        row++;
        col++;
#endif
        return ga_board_position[row][col];
    }

    // ----------------------------------------
    void cell_set_piece ( int row, int col, int piece )
    {
#if VERIFY_SAFE_BOARD_ARRAY
        row++;
        col++;
#endif
        ga_board_position[row][col] = piece;
    }

    // ----------------------------------------
    int get_glyph (char symbol)
    {
        static const wchar_t turn[2] =
        { // white, black
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
            case PLAYER_WHITE: return turn[0]; // White's Turn
            case PLAYER_BLACK: return turn[1]; // Black's Turn
            default : return 0x0020; // Empty
        }
    }


    // ----------------------------------------
    bool is_empty (int row, int col)
    {
#if VERIFY_SAFE_BOARD_ARRAY
        return (ga_board_position[row+1][col+1] == ' ');
#else
        return (ga_board_position[row][col] == ' ');
#endif
    }

    // ----------------------------------------
    int is_off_board ( int row, int col )
    {
        if ((row < 0) || (row > 7)) return true;
        if ((col < 0) || (col > 7)) return true;
        return false;
    }

    // ----------------------------------------
    bool is_move_possible (int position)
    {
        for (int i = 0; i < gn_possible_moves; i++)
        {
            if (position == ga_possible_moves[i])
                return true;
        }
        return false;
    }

    // ----------------------------------------
    void position_to_labels ( int position, char *row, char *col )
    {
        int x, y;
        position_to_row_col( position, &y, &x );
        *row = ga_board_rows[ y ];
        *col = ga_board_cols[ x ];
    }

    // ----------------------------------------
    void position_to_row_col ( int position, int *row, int *col )
    {
       *row = (position >> 4) & 0xF;
       *col = (position >> 0) & 0xF;
    }

    // ----------------------------------------
    int row_col_to_position ( int row, int col )
    {
        int position = (row << 4) | col;
        return position;
    }

    // Convert piece symbol (representation) to enum (piece_e)
    // ----------------------------------------
    int type_to_piece( char type )
    {
        for( int color = PLAYER_WHITE; color < NUM_PLAYERS; color++ )
            for( int piece = 0; piece < NUM_PIECES; piece++ )
                if (ga_pieces[ color ][ piece ] == type)
                        return piece;
        return PIECE_NONE;
    }

// Implementation

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

// Test if there is a black piece at row,col
// NOTE: This is horribly inefficient; ga_board_position should be storing
// PIECE_* not ASCII characters then a simple bit comparison can be used.
// ----------------------------------------
bool cell_has_black_piece ( int row , int col )
{
    int piece = cell_get_piece(row,col);
    switch( piece )
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

// ----------------------------------------
bool cell_has_player ( int row, int col, int player )
{
    if (player == PLAYER_WHITE)
        return cell_has_white_piece( row, col );
    else
        return cell_has_black_piece( row, col );
}

// Check if there is a white piece at row,col
// ----------------------------------------
bool cell_has_white_piece ( int row , int col )
{
    int piece = cell_get_piece(row,col);
    switch( piece )
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
void display_board_header ()
{
    wprintf(L" ") ;
    for (int col = 0; col < 8; col++)
    {
        wprintf( L"   %lc" , ga_board_cols[ col ] );
    }
    wprintf(L"\n" );
}

// ----------------------------------------
void display_board (char board[BOARD_DIMENSION][BOARD_DIMENSION])
{
    int x , y;

    display_eliminated( PLAYER_WHITE );
    display_board_header();

    const wchar_t *separator = L"  ----------------------------------\n";

    for (y = 0; y < 8; y++)
    {
        wprintf( separator );
        wprintf(L"%lc " , ga_board_rows[ y ] );

        for( x = 0 ; x < 8 ; x++ )
        {
#if VERIFY_SAFE_BOARD_ARRAY
            wprintf(L"||%lc " , get_glyph( board[y+1][x+1] ));
#else
            wprintf(L"||%lc " , get_glyph( board[y][x] ));
#endif
        }
        wprintf(L"|| %lc\n", ga_board_rows[ y ] );
    }
    wprintf( separator );
    display_board_header();
    display_eliminated( PLAYER_BLACK );
    wprintf(L"\n");
}

// ----------------------------------------
void display_position_board ()
{
    display_board( ga_board_position );
}

// ----------------------------------------
void display_eliminated( int player )
{
    char *eliminated_pieces = &ga_eliminated_pieces[ player ][ 0 ];
    int   eliminated_count  =  gn_eliminated_pieces[ player ]     ;

    for (int i = 0; i < eliminated_count; i++ )
    {
        wprintf( L"%lc ", get_glyph( eliminated_pieces[ i ] ) );
    }
    wprintf( L"\n" );
}

// ----------------------------------------
void display_possible_board()
{
    display_board( ga_board_possible );
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
"    }===={           \\___|       \\___|_| |_|\\___||___/___/ \n"
"  (________)       Kahouli Wajd. 死, bugfixes by Michaelangel007\n"
    );
}

// r1,c1 from
// r2,c2 to
// ----------------------------------------
void move_piece ( int r1, int c1, int r2, int c2, int player )
{
    int  opponent   = 1 - player;
    int  eliminated = gn_eliminated_pieces[ opponent ];

    char attacker   = cell_get_piece(r1,c1);
    char defender   = cell_get_piece(r2,c2);

    // castling flags -- rook
    if (attacker == ga_pieces[ player ][ PIECE_ROOK ])
    {
        if (player == PLAYER_WHITE)
        {
            if (!ga_rook_k_moved[ player ] && (r1 == 7) && (c1 == 7)) ga_rook_k_moved[ player ] = true;
            if (!ga_rook_q_moved[ player ] && (r1 == 7) && (c1 == 0)) ga_rook_q_moved[ player ] = true;
        }
        else
        {
            if (!ga_rook_k_moved[ player ] && (r1 == 0) && (c1 == 7)) ga_rook_k_moved[ player ] = true;
            if (!ga_rook_q_moved[ player ] && (r1 == 0) && (c1 == 0)) ga_rook_q_moved[ player ] = true;
        }
    }

    // check for castling -- king "attacks" rook
    // https://en.wikipedia.org/wiki/Castling
    if (((attacker == ga_pieces[ player ][ PIECE_KING ]) && (defender == ga_pieces[ player ][ PIECE_ROOK ])))
    {
        int castle_row = -1; // +0 = black, +7 = white
        int king_side  =  0; // +1 = king,  -1 = queen

        if ((r1 == 0) && (c1 == 4) && (r2 == 0) && (c2 == 7)) { castle_row =  0; king_side =  1; } // black king side
        if ((r1 == 0) && (c1 == 4) && (r2 == 0) && (c2 == 0)) { castle_row =  0; king_side = -1; } // black queen side
        if ((r1 == 7) && (c1 == 4) && (r2 == 7) && (c2 == 7)) { castle_row =  7; king_side =  1; } // white king side
        if ((r1 == 7) && (c1 == 4) && (r2 == 7) && (c2 == 0)) { castle_row =  7; king_side = -1; } // white queen side

        if (castle_row >= 0)
        {
            if (king_side > 0)
            {
                // We don't need to check if king passes through check, that should already have been done in moves_king or moves_rook
                cell_set_piece( castle_row, 4, ga_pieces[ player ][ PIECE_NONE ] ); // E1 empty
                cell_set_piece( castle_row, 5, ga_pieces[ player ][ PIECE_ROOK ] ); // F1 rook
                cell_set_piece( castle_row, 6, ga_pieces[ player ][ PIECE_KING ] ); // G1 king (E1 + 2)
                cell_set_piece( castle_row, 7, ga_pieces[ player ][ PIECE_NONE ] ); // H1 empty
                ga_rook_k_moved[ player ] = true;
                return;
            }
            else
            {
                // We don't need to check if king passes through check, that should already have been done in moves_king or moves_rook
                cell_set_piece(castle_row, 4, ga_pieces[ player ][ PIECE_NONE ] ); // E1 empty
                cell_set_piece(castle_row, 3, ga_pieces[ player ][ PIECE_ROOK ] ); // D1 rook
                cell_set_piece(castle_row, 2, ga_pieces[ player ][ PIECE_KING ] ); // C1 king (E1 - 2)
                cell_set_piece(castle_row, 0, ga_pieces[ player ][ PIECE_NONE ] ); // A1 empty
                ga_rook_q_moved[ player ] = true;
                return;
            }
        }
    }

    if (player == PLAYER_BLACK)
    {
        if (cell_has_white_piece(r2, c2)) // black captures white
        {
            ga_eliminated_pieces[ opponent ][ eliminated ] = defender;
            gn_eliminated_pieces[ opponent ]++;
        }
        cell_set_piece( r1, c1, ga_pieces[ player ][ PIECE_NONE ] );
        cell_set_piece( r2, c2, attacker );
    }
    else
    {
        if (cell_has_black_piece(r2, c2)) // white captures black
        {
            ga_eliminated_pieces[ opponent ][ eliminated ] = defender;
            gn_eliminated_pieces[ opponent ]++;
        }
        cell_set_piece( r1, c1, ga_pieces[ player ][ PIECE_NONE ] );
        cell_set_piece( r2, c2, attacker );
    }
}

// ----------------------------------------
void moves_bishop ( int r1 , int c1, int player)
{
    gn_possible_moves = 0;

    add_possible_axis( r1, c1, player, AXIS_NE );
    add_possible_axis( r1, c1, player, AXIS_NW );
    add_possible_axis( r1, c1, player, AXIS_SW );
    add_possible_axis( r1, c1, player, AXIS_SE );
}

// ----------------------------------------
void moves_king ( int r1 , int c1, int player )
{
    int r;
    gn_possible_moves  = 0;

    // TODO: This doesn't check if the king moves into check, need to add a flag.
    add_possible_array( NUM_AXIS, axis_delta_row, axis_delta_col, r1, c1, player );

         r    = (player == PLAYER_WHITE) ? 7 : 0;
    char rook = ga_pieces[ player ][ PIECE_ROOK ];
    if ((r1 == r) && (c1 == 4))
    {
        // can castle king side?
        if (is_empty( r, 5 )
        &&  is_empty( r, 6 )
        && (cell_get_piece(r,7) == rook) && !ga_king_moved[player] && !ga_rook_k_moved[player])
        {
            // TODO: Check if king passes through check
            ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position( r, 7 );
        }

        // castle queen side
        if (is_empty( r, 3 )
        &&  is_empty( r, 2 )
        &&  is_empty( r, 1 )
        && (cell_get_piece(r,0) == rook) && !ga_king_moved[player] && !ga_rook_q_moved[player])
        {
            // TODO: Check if king passes through check
            ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position( r, 0 );
        }
    }
}

//  . 10  .  1  .    -2
//  9  .  .  .  3    -1
//  .  .  N  .  .     0
//  8  .  .  .  4    +1
//  .  7  .  5  .    +2
//
// -2 -1  0 +1 +2
// ----------------------------------------
void moves_knight ( int r1 , int c1, int player )
{
    gn_possible_moves = 0;

    const int knight_col[8] = { -2, -2, -1, -1, +1, +1, +2, +2 };
    const int knight_row[8] = { -1, +1, -2, +2, -2, +2, +1, -1 };
    add_possible_array( sizeof(knight_row)/sizeof(int), knight_row, knight_col, r1, c1, player);
}

// ----------------------------------------
void moves_pawn ( int r1, int c1, int player )
{
    const int opponent  = 1 - player;
    const int start_row = (player == PLAYER_WHITE) ?  6 :  1;
    const int dy        = (player == PLAYER_WHITE) ? -1 : +1; // forward direction for player

    int r,c;
    gn_possible_moves = 0;

    if (r1 == start_row)
    {
        r = r1+2*dy; c = c1;
        if (!is_off_board(r,c) && is_empty(r,c) && is_empty(r1+dy,c))
            ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(r,c);
    }

    r = r1+dy; c = c1;
    if (!is_off_board(r,c) && is_empty(r,c))
        ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(r,c);

    r = r1+dy; c = c1+1; // attack right
    if (!is_off_board(r,c) && cell_has_player(r,c,opponent))
        ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(r,c);

    r = r1+dy; c = c1-1; // attack left
    if (!is_off_board(r,c) && cell_has_player(r,c,opponent))
        ga_possible_moves[ gn_possible_moves++ ] = row_col_to_position(r,c);
}

// ----------------------------------------
void moves_queen ( int r1 , int c1, int player )
{
    gn_possible_moves = 0;

    for (int axis = 0; axis < NUM_AXIS; axis++ )
        add_possible_axis( r1, c1, player, axis );
}

// ----------------------------------------
void moves_rook ( int r1 , int c1, int player)
{
    gn_possible_moves = 0;

    add_possible_axis( r1, c1, player, AXIS_E );
    add_possible_axis( r1, c1, player, AXIS_N );
    add_possible_axis( r1, c1, player, AXIS_W );
    add_possible_axis( r1, c1, player, AXIS_S );
}

// ----------------------------------------
int player_input_row_col ( int player, int from_piece )
{
    int ch, row, col, position;

    do
    {
        position = -1;
        fflush(stdin);
        do
        {
            if (from_piece == PIECE_NONE)
                wprintf( L"Enter position of %hs piece to move [col row]: ", ga_player_colors[ player ] );
            else
                wprintf( L"\nEnter new position of %hs %hs [col row]: ", ga_player_colors[ player ], ga_piece_names[ from_piece ] );
            fflush(stdout);

            col = -1;
            ch = getch();
            if (ch == 3) // Ctrl-C
                exit( 0 );
            if (ch == 27)
            {
                wprintf( L"\n" );
                return -1;
            }
            wprintf( L"%lc", ch );

            if ((ch >= 'a') && (ch <= 'h')) col = (ch - 'a');
            if ((ch >= 'A') && (ch <= 'H')) col = (ch - 'A');
            if (col < 0)
                wprintf( L"\n%lc Invalid column. Must be A..H\n", get_glyph( '!' ) );
        } while (col < 0);

        do
        {
            row = -1;
            ch = getch();
            if (ch == 3) // Ctrl-C
                exit( 0 );
            if (ch == 27)
            {
                wprintf( L"\n" );
                return -1;
            }
            wprintf( L"%lc\n", ch );

            if ((ch >= '1') && (ch <= '8')) row = (ch - '1');
            if (row < 0)
                wprintf( L"%lc Invalid row. Must be 1..8\n", get_glyph( '!' ) );
        } while (row < 0);
        row = (7 - row); // flip y

        position = row_col_to_position( row, col );
    } while (position < 0);

    return position;
}

// ----------------------------------------
void player_turn (int player)
{
#if INPUT_ALGEBRAIC
    player_turn_algebraic( player );
#else
    player_turn_integer( player );
#endif
}

// ----------------------------------------
void player_turn_algebraic ( int player )
{
    int pos1, pos2, row1, col1, row2, col2;
    int attacker;
    bool valid_from = true;
    bool valid_move = true;

    do
    {
        do
        {
            valid_from = true;
            memcpy( ga_board_possible, ga_board_position, sizeof(ga_board_position) );

            wprintf( L"%lc %hs to move. ", get_glyph( player ), ga_player_colors[ player ] );
            pos1 = player_input_row_col( player, PIECE_NONE );
            if (pos1 >= 0)
            {
                position_to_row_col( pos1, &row1, &col1 );
                attacker = cell_get_piece( row1, col1 );

                if (player == PLAYER_WHITE)
                {
                    switch( attacker )
                    {
                        case 'p': moves_pawn  ( row1 , col1, player ); break;
                        case 'r': moves_rook  ( row1 , col1, player ); break;
                        case 'h': moves_knight( row1 , col1, player ); break;
                        case 'c': moves_bishop( row1 , col1, player ); break;
                        case 'k': moves_king  ( row1 , col1, player ); break;
                        case 'q': moves_queen ( row1 , col1, player ); break;
                        default:
                            wprintf( L"%lc Invalid Position!\n", get_glyph( '!' ) );
                            valid_from = false;
                    }
                }
                else
                {
                    switch( attacker ) // Select only player's pieces.
                    {
                        case 'P': moves_pawn  ( row1 , col1, player ); break;
                        case 'R': moves_rook  ( row1 , col1, player ); break;
                        case 'H': moves_knight( row1 , col1, player ); break;
                        case 'C': moves_bishop( row1 , col1, player ); break;
                        case 'K': moves_king  ( row1 , col1, player ); break;
                        case 'Q': moves_queen ( row1 , col1, player ); break;
                        default:
                            wprintf( L"%lc Invalid Position!\n", get_glyph( '!' ) );
                            valid_from = false;
                    }
                }
            }
            else
                valid_from = false;
        } while (!valid_from);

        valid_move = false;
        int piece = type_to_piece( attacker );

        if (gn_possible_moves)
        {
            clear_screen();
            update_possible_moves( row1, col1 );

            do
            {
                pos2 = player_input_row_col( player, piece );
                if (pos2 < 0) // allow user to re-start move
                    break;
            } while (!is_move_possible(pos2));

            if (pos2 >= 0)
                valid_move = true;
        }
    } while (!valid_move);

    position_to_row_col( pos2, &row2, &col2 );
    move_piece(row1, col1, row2, col2, player);
}

// ----------------------------------------
void player_turn_integer ( int player )
{
    int opponent = 1 - player;
    int p1 , p2 , c1 , r1 , c2 , r2, input_control;
    int position;
    char attacker;
    int  piece;

    wprintf( L"%lc %hs to move ...", get_glyph( player ), ga_player_colors[ player ] );

again1:
    do
    {
        memcpy( ga_board_possible, ga_board_position, sizeof(ga_board_position) );
        fflush(stdin);
        wprintf( L"\nEnter position of piece to move [row column]: " );
        input_control = scanf( "%d" , &position );

        // Convert legacy position to native position
        c1 = position % 10;
        r1 = position / 10;
        p1 = row_col_to_position(r1,c1);
        if (is_off_board(r1, c1))
        {
            if (is_off_board(r1, 0))
                wprintf( L"%lc Invalid row. Must be 0..7\n", get_glyph( '!' ) );
            else
                wprintf( L"%lc Invalid column. Must be 0..7\n", get_glyph( '!') );
        }
    } while ((input_control == 0) && is_off_board(r1,c1));

    attacker = cell_get_piece( r1, c1 );
    if (player == PLAYER_BLACK)
    {
        switch( attacker ) // Select only player's pieces.
        {
            case 'P': moves_pawn  ( r1 , c1, player ); break;
            case 'R': moves_rook  ( r1 , c1, player ); break;
            case 'H': moves_knight( r1 , c1, player ); break;
            case 'C': moves_bishop( r1 , c1, player ); break;
            case 'K': moves_king  ( r1 , c1, player ); break;
            case 'Q': moves_queen ( r1 , c1, player ); break;
            default:
                wprintf( L"%lc Invalid Position! ", get_glyph( '!' ) );
                goto again1 ;
        }
    }
    else
    {
        switch( attacker ) // Select only player's pieces.
        {
            case 'p': moves_pawn  ( r1 , c1, player ); break;
            case 'r': moves_rook  ( r1 , c1, player ); break;
            case 'h': moves_knight( r1 , c1, player ); break;
            case 'c': moves_bishop( r1 , c1, player ); break;
            case 'k': moves_king  ( r1 , c1, player ); break;
            case 'q': moves_queen ( r1 , c1, player ); break;
            default:
                wprintf( L"%lc Invalid Position! ", get_glyph( '!' ) );
                goto again1 ;
        }
    }

    piece = type_to_piece( attacker );
    if (gn_possible_moves)
    {
        clear_screen();
        update_possible_moves( r1, c1 );

        do
        {
            wprintf( L"\nEnter new position of %hs [row column]: ", ga_piece_names[ piece ] );
            scanf( "%d" , &position );

            // Convert legacy position to native position
            c2 = position % 10;
            r2 = position / 10;
            p2 = row_col_to_position(r2,c2);
        } while (!is_move_possible(p2));
    }
    else
    {
        goto again1;
    }

    position_to_row_col( p2, &r2, &c2 );
    move_piece(r1,c1,r2,c2, player);
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
        move_piece(7,4,7,6, PLAYER_WHITE); // king
        move_piece(7,7,7,5, PLAYER_WHITE); // rook
    }
    else
    if ((r1 == 0) && (c1 == 0) && (r2 == 0) && (c2 == 0) && (player == PLAYER_BLACK)) // castle
    {
        move_piece(0,4,0,2, PLAYER_BLACK); // king
        move_piece(0,0,0,3, PLAYER_BLACK); // rook
    }
    else
    {
        move_piece(r1,c1,r2,c2, player);
        display_position_board();
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
void update_possible_moves ( int row, int col )
{
    int x,y;

    for (int i=0; i < gn_possible_moves; i++)
    {
        position_to_row_col( ga_possible_moves[i], &y, &x );
        if (is_empty( y, x ))
#if VERIFY_SAFE_BOARD_ARRAY
            ga_board_possible[y+1][x+1] = 'x';
#else
            ga_board_possible[y][x] = 'x';
#endif
    }
    display_board( ga_board_possible );

    wprintf( L"Possible moves %lc: \n", get_glyph( cell_get_piece(row,col) ));
    if (gn_possible_moves)
    {
        char row, col;

        wprintf( L"  " );
        for (int i = 0; i < gn_possible_moves; i++)
        {
            position_to_labels( ga_possible_moves[i], &row, &col );
#if INPUT_ALGEBRAIC
            wprintf( L"%lc%lc, ", col, row );
#else
            wprintf( L"%lc%lc :", row, col );
#endif
        }
        wprintf( L"\n" );
    }
    else
    {
        wprintf( L"No possible moves -> " );
    }
}

// ========================================
void main (int argc, char *argv[])
{
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
            wchar_t glyph = (wchar_t) get_glyph( pieces[ i ] );
            wprintf( L"%lc", glyph );
        }
        wprintf( L"\n" );
        wprintf( L"\n\x26AB White (Win32)" );
        wprintf( L"\n\x26AA Black (Win32)" );
        wprintf( L"\n\x26AA White (Unix )" );
        wprintf( L"\n\x26AB Black (Unix )" );
        wprintf( L"\n%lc this platform", get_glyph( PLAYER_WHITE ) );
        wprintf( L"\n%lc this platform", get_glyph( PLAYER_BLACK ) );
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
        memcpy( ga_board_position, BOARD_INIT, sizeof(BOARD_INIT) );

        for (int player = PLAYER_WHITE; player < NUM_PLAYERS; player++)
        {
            gn_eliminated_pieces[ player ] = 0;
            ga_king_moved       [ player ] = false;
            ga_rook_k_moved     [ player ] = false;
            ga_rook_q_moved     [ player ] = false;
        }

        do
        {
            clear_screen();
            display_position_board();
            player_turn( moves & 1 );
            moves++;
        } while (true);
    }
}
