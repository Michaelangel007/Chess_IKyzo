Final Project -- version 2.0 by Kahouli Wajd 
Bugfixes, cleanup, and QoL by Michaelangel007.

# Features

* Unicode display of chess pieces.
* Algebraic notation (input and display.)
* Selected piece shows possible moves.
* WIP Castling (doesn't check if King passes through check.)
* Save game replay.
* Compiles on Windows and Linux
* MSVC 2022 Solution and Project

# Chess Fonts

You can download an Unicode chess font from:

DejaVuSansMono.ttf
* https://dejavu-fonts.github.io/

chess_merida_unicode.ttf
* https://github.com/xeyownt/chess_merida_unicode

# UNICODE Symbols

* ♔ black-king           : \x2654
* ♕ black-queen          : \x2655
* ♖ black-rook           : \x2656
* ♗ black-bishop         : \x2657
* ♘ black-knight         : \x2658
* ♙ black-pawn           : \x2659
* ♚ white-king           : \x265A
* ♛ white-queen          : \x265B
* ♜ white-rook           : \x265C
* ♝ white-bishop         : \x265D
* ♞ white-knight         : \x265E
* ♟ white-pawn           : \x265F
* ⚠ Warning sign        : \x26A0
* ⚪ medium white circle : \x26AA
* ⚫ medium black circle : \x26AB

---

# Game Manager Architecture

Game Manage();
-> display_board();
-> white_play();
-> black_play();

player()
--> Piece selection
--> Piece Positioning/Taking  


piece characteristics :
{
    color : white/black [playeri / playerii]
    type : p,r,h,c,k,q
    pinned : true/false
}

# Display Design

♟♟♟♝♞                              Array of of eliminated white pieces
    A   B   C   D   E   F   G   H      Rows and Columns using Algebraic notation
  ----------------------------------   Board Display using Unicode
8 ||♖||♘||♗||♔||♕||♗||♘||♖|| 8
  ----------------------------------
7 ||♙||♙||♙||♙||♙||♙||♙||♙|| 7
  ----------------------------------
6 ||  ||  ||  ||  ||  ||  ||  ||  || 6
  ----------------------------------
5 ||  ||  ||  ||  ||  ||  ||  ||  || 5
  ---------------------------------- 
4 ||  ||  ||  ||  ||  ||  ||  ||  || 4
  ----------------------------------
3 ||  ||  ||  ||  ||  ||  ||  ||  || 3
  ----------------------------------
2 ||♟||♟||♟||♟||♟||♟||♟||♟|| 2
  ----------------------------------
1 ||♜||♞||♝||♚||♛||♝||♞||♜|| 1
  ----------------------------------
    A   B   C   D   E   F   G   H
♙♙♙♙♙♙♖♖♕                        Array of eliminated black pieces

# Movement Calculation

* Pawn

  ----------------------------------   
0 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ----------------------------------
1 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
2 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
3 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
4 ||  ||  ||  ||✖||  ||  ||  ||  ||
  ----------------------------------
5 ||  ||  || ✖||✖||✖ ||  ||  ||  ||
  ----------------------------------
6 ||  ||  ||  ||♟||  ||  ||  ||  ||
  ----------------------------------
7 ||  ||  ||  ||  ||  ||  ||  ||  ||
  ----------------------------------
  !-> if conditions
  
* Knight

  ----------------------------------   
0 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ----------------------------------
1 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
2 ||  ||  || ✖||  ||✖||  ||  ||  || 
  ---------------------------------- 
3 ||  ||✖||  ||  ||  ||✖||  ||  || 
  ---------------------------------- 
4 ||  ||  ||  ||♞||  ||  ||  ||  ||
  ----------------------------------
5 ||  ||✖||  ||  ||  ||✖||  ||  ||
  ----------------------------------
6 ||  ||  ||✖||  ||✖||  ||  ||  ||
  ----------------------------------
7 ||  ||  ||  ||  ||  ||  ||  ||  ||
  ----------------------------------
  !-> if conditions 

* Bishop

  ----------------------------------   
0 ||  ||  ||  ||  ||  ||  ||  ||✖|| 
  ----------------------------------
1 ||✖||  ||  ||  ||  ||  ||✖||  || 
  ---------------------------------- 
2 ||  ||✖||  ||  ||  ||✖||  ||  || 
  ---------------------------------- 
3 ||  ||  ||✖||  ||✖||  ||  ||  || 
  ---------------------------------- 
4 ||  ||  ||  ||♝||  ||  ||  ||  ||
  ----------------------------------
5 ||  ||  ||✖||  ||✖||  ||  ||  ||
  ----------------------------------
6 ||  ||✖||  ||  ||  ||✖||  ||  ||
  ----------------------------------
7 ||✖||  ||  ||  ||  ||  ||✖||  ||
  ----------------------------------
  !-> While Loops in 4 directions  

* Rook

  ----------------------------------   
0 ||  ||✖||  ||  ||  ||  ||  ||  || 
  ----------------------------------
1 ||  ||✖||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
2 ||✖||♜||✖||✖|| ✖|| ✖|| ✖|| ✖|| 
  ---------------------------------- 
3 ||  ||✖||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
4 ||  ||✖||  ||  ||  ||  ||  ||  ||
  ----------------------------------
5 ||  ||✖||  ||  ||  ||  ||  ||  ||
  ----------------------------------
6 ||  ||✖||  ||  ||  ||  ||  ||  ||
  ----------------------------------
7 ||  ||✖||  ||  ||  ||  ||  ||  ||
  ----------------------------------
  !-> While loops in 4 directions

* King

  ----------------------------------   
0 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ----------------------------------
1 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
2 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
3 ||  ||  ||  ||  ||  ||  ||  ||  || 
  ---------------------------------- 
4 ||  ||  ||  ||  ||  ||  ||  ||  ||
  ----------------------------------
5 ||  ||  ||✖||✖||✖||  ||  ||  ||
  ----------------------------------
6 ||  ||  ||✖||♚||✖||  ||  ||  ||
  ----------------------------------
7 ||  ||  ||✖||✖||✖||  ||  ||  ||
  ----------------------------------
  !-> if conditions

* Queen

  ----------------------------------   
0 ||  ||  ||  ||✖||  ||  ||  ||✖|| 
  ----------------------------------
1 ||✖||  ||  ||✖||  ||  ||✖||  || 
  ---------------------------------- 
2 ||  ||✖||  ||✖||  ||✖||  ||  || 
  ---------------------------------- 
3 ||  ||  ||✖||✖||✖||  ||  ||  || 
  ---------------------------------- 
4 ||✖||✖||✖||♛||✖||✖|| ✖|| ✖||
  ----------------------------------
5 ||  ||  ||✖||✖||✖||  ||  ||  ||
  ----------------------------------
6 ||  ||✖||  ||✖||  ||✖||  ||  ||
  ----------------------------------
7 ||✖||  ||  ||✖||  ||  ||✖||  ||
  ----------------------------------
  !-> While loops in 8 directions.

# NOTES

- Incomplete components :
-x Terminal display is not at a production level.
-> Transforming the encoding to Unicode to include special symbols. [UTF-8, UTF-16 are not functional]
Setting the execution environment in visual code. [Windows consoles are not reliable] 
C/C++ Compile Run.


-x Take mechanic is absent.
-> Construct piece elemination mecanism. 

-x Check mechanic is absent.
-> Append new king situation check.

-x Board escaping pieces.
-> Append new movement restrictions.

-x Pieces are not identified to each player.
-> Construct a game manager.

-x Checkmate mechanic is absent.
-x Promote pawn mechanic is absent.

Features : 
-+ Improve main page asthetics.
-+ Compute real time score.
-+ Add match notes.
-+ Add timer.
-+ Improve input system.
-+ Show possible moves for selected piece.
