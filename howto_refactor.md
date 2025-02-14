# HOWTO: Refactor - A Tutorial on Semantic Compression

This example will work through refactoring naive code to beautiful code.

Let's look at how a knight moves, given some starting row and column position, r1 and c1 respectively:

We can label the destination locations by using a clock.

```
    . 10  .  1  .    -2
    9  .  .  .  3    -1
    .  .  N  .  .     0
    8  .  .  .  4    +1
    .  7  .  5  .    +2

   -2 -1  0 +1 +2
```

1. Original code

**One of the secrets to being a greate programmer is** -- get a (slow) reference version working first, THEN optimize it (fast version).

Why?

1. It doesn't matter how fast you get the wrong answer!

2. How do you KNOW your optimized version is _actually_ faster?  Without a reference version to benchmark you won't really know.

The original C code for generating a list of potential moves that a knight can move to is this hideous monstrosity:

```c
void knight( int r1 , int c1, int player_id )
{
    possible_moves_index=0;

    if(player_id==1)
    {
        if( board[r1+2][c1+1] == ' ' || check(r1+2, c1+1)==1 )
        {
            if(r1+2 <= 7 && r1+2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1+1;
                possible_moves_index++;
            }
            
            //wprintf(L"%d%d, " , r1+2 ,c1+1) ;
        }
         
        if( board[r1+2][c1-1] == ' ' || check(r1+2, c1-1)==1) 
        { 
            if(r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1+1][c1+2] == ' ' || check(r1+1, c1+2)==1) 
        {  
            if(r1+1 <= 7 && r1+1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1+2;
                possible_moves_index++;
            }
        }
        if( board[r1-1][c1+2] == ' ' || check(r1-1, c1+2)==1) 
        {  
            if(r1-1 <= 7 && r1-1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1-1] == ' ' || check(r1-2, c1-1)==1 )
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1+1] == ' ' || check(r1-2, c1+1)==1) 
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1+1;
                possible_moves_index++;
            }
        }
        

        if( board[r1+1][c1-2] == ' ' || check(r1+1, c1-2)==1) 
        {
            if(r1+1 <= 7 && r1+1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1-2;
                possible_moves_index++;
            }
        }
    

        if( board[r1-1][c1-2] == ' ' || check(r1-1, c1-2)==1 )
        {
            if(r1-1 <= 7 && r1-1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1-2;
                possible_moves_index++;
            }
                
        }
    }
    else if(player_id==2)
    {
        if( board[r1+2][c1+1] == ' ' || check2(r1+2, c1+1)==1 )
        {
            if(r1+2 <= 7 && r1+2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1+1;
                possible_moves_index++;
            }
            
            //wprintf(L"%d%d, " , r1+2 ,c1+1) ;
        }
         
        if( board[r1+2][c1-1] == ' ' || check2(r1+2, c1-1)==1) 
        { 
            if(r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1+1][c1+2] == ' ' || check2(r1+1, c1+2)==1) 
        {  
            if(r1+1 <= 7 && r1+1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1+2;
                possible_moves_index++;
            }
        }
        if( board[r1-1][c1+2] == ' ' || check2(r1-1, c1+2)==1) 
        {  
            if(r1-1 <= 7 && r1-1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1-1] == ' ' || check2(r1-2, c1-1)==1 )
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1+1] == ' ' || check2(r1-2, c1+1)==1) 
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1+1;
                possible_moves_index++;
            }
        }
        

        if( board[r1+1][c1-2] == ' ' || check2(r1+1, c1-2)==1) 
        {
            if(r1+1 <= 7 && r1+1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1-2;
                possible_moves_index++;
            }
        }
    

        if( board[r1-1][c1-2] == ' ' || check2(r1-1, c1-2)==1 )
        {
            if(r1-1 <= 7 && r1-1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1-2;
                possible_moves_index++;
            }
                
        }
    }
    else{
        wprintf(L"Error : Knight");
    }
    

    //display_possible_moves(board_possible_moves);
    wprintf(L"Possible moves %c : \n", display_convert(board[r1][c1]));
    if(possible_moves_index >= 1)
    {
        for (int i = 0; i < possible_moves_index; i++)
        {
            wprintf(L" %d :", possible_moves[i]);
        }
    }
    else
    {
        wprintf(L"No possible moves -> ");
    }
}
```

I mean as _reference_ version it works. Or does it?

Unfortunately there are lots of problems and bugs with this code.

* It references memory Out-of-Bounds,
* It uses a magic-number hard-coded for the white and black player,
* It duplicates code,
* It generates possible moves and displays possible moves,
* LOTS of redundant calculations,
* It uses `check()` and `check2()` which aren't descriptive of what they actually DO.
* Inconsistent formatting.

The functions:

* `check()` really should be called `cell_has_white()`
* `check2()` reallly should be called `cell_has_black()`

We also don't need to manually check if they return true.

Let's clean this up and show how to write beautiful code.

2. Cleanup hard-coded players and remove output.

Code should either do a) calculations, or b) display.  There _are_ times when you want both but this isn't the time.

```c
void knight_v2( int r1 , int c1, int player )
{
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        if( board[r1+2][c1+1] == ' ' || cell_has_white(r1+2, c1+1) )
        {
            if(r1+2 <= 7 && r1+2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1+1;
                possible_moves_index++;
            }
        }

        if( board[r1+2][c1-1] == ' ' || cell_has_white(r1+2, c1-1) ) 
        { 
            if(r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1+1][c1+2] == ' ' || cell_has_white(r1+1, c1+2) ) 
        {  
            if(r1+1 <= 7 && r1+1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( board[r1-1][c1+2] == ' ' || cell_has_white(r1-1, c1+2) ) 
        {  
            if(r1-1 <= 7 && r1-1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1-1] == ' ' || cell_has_white(r1-2, c1-1) )
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1+1] == ' ' || cell_has_white(r1-2, c1+1) ) 
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1+1;
                possible_moves_index++;
            }
        }

        if( board[r1+1][c1-2] == ' ' || cell_has_white(r1+1, c1-2) ) 
        {
            if(r1+1 <= 7 && r1+1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1-2;
                possible_moves_index++;
            }
        }

        if( board[r1-1][c1-2] == ' ' || cell_has_white(r1-1, c1-2) )
        {
            if(r1-1 <= 7 && r1-1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1-2;
                possible_moves_index++;
            }
        }
    }
    else
    if (player == PLAYE_WHITE)
    {
        if( board[r1+2][c1+1] == ' ' || cell_has_black(r1+2, c1+1) )
        {
            if(r1+2 <= 7 && r1+2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1+1;
                possible_moves_index++;
            }
        }
         
        if( board[r1+2][c1-1] == ' ' || cell_has_black(r1+2, c1-1) ) 
        { 
            if(r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1+1][c1+2] == ' ' || cell_has_black(r1+1, c1+2) ) 
        {  
            if(r1+1 <= 7 && r1+1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( board[r1-1][c1+2] == ' ' || cell_has_black(r1-1, c1+2) ) 
        {  
            if(r1-1 <= 7 && r1-1 >= 0 && c1+2 >= 0 && c1+2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1-1] == ' ' || cell_has_black(r1-2, c1-1) )
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1-1 >= 0 && c1-1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( board[r1-2][c1+1] == ' ' || cell_has_black(r1-2, c1+1) ) 
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1+1;
                possible_moves_index++;
            }
        }

        if( board[r1+1][c1-2] == ' ' || cell_has_black(r1+1, c1-2) ) 
        {
            if(r1+1 <= 7 && r1+1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1-2;
                possible_moves_index++;
            }
        }

        if( board[r1-1][c1-2] == ' ' || cell_has_black(r1-1, c1-2) )
        {
            if(r1-1 <= 7 && r1-1 >= 0 && c1-2 >= 0 && c1-2 <=7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1-2;
                possible_moves_index++;
            }
        }
    }
}
```

3. All the checking for an empty location can be simplied if we provide a `is_cell_empty(row,cow)` function.

```c
    // ----------------------------------------
    bool is_cell_empty (int row, int col)
    {
        return (board[row][col] == ' ');
    }
```

Making another pass:

```c
void knight_v3( int r1 , int c1, int player )
{
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        if( is_cell_empty(r1+2,c1+1) || cell_has_white(r1+2, c1+1) )
        {
            if(r1+2 <= 7 && r1+2 >= 0 && c1+1 >= 0 && c1+1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1+1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1+2,c1-1) || cell_has_white(r1+2, c1-1) ) 
        { 
            if(r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1+1,c1+2) || cell_has_white(r1+1, c1+2) ) 
        {  
            if(r1+1 <= 7 && r1+1 >= 0 && c1+2 >= 0 && c1+2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-1,c1+2) || cell_has_white(r1-1, c1+2) ) 
        {  
            if(r1-1 <= 7 && r1-1 >= 0 && c1+2 >= 0 && c1+2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-2,c1-1) || cell_has_white(r1-2, c1-1) )
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1-1 >= 0 && c1-1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-2,c1+1) || cell_has_white(r1-2, c1+1) ) 
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1+1 >= 0 && c1+1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1+1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1+1,c1-2) || cell_has_white(r1+1, c1-2) ) 
        {
            if(r1+1 <= 7 && r1+1 >= 0 && c1-2 >= 0 && c1-2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1-2;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-1,c1-2) || cell_has_white(r1-1, c1-2) )
        {
            if(r1-1 <= 7 && r1-1 >= 0 && c1-2 >= 0 && c1-2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1-2;
                possible_moves_index++;
            }
        }
    }
    else
    if (player == PLAYE_WHITE)
    {
        if( is_cell_empty(r1+2,c1+1) || cell_has_black(r1+2, c1+1) )
        {
            if(r1+2 <= 7 && r1+2 >= 0 && c1+1 >= 0 && c1+1 <=7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1+1;
                possible_moves_index++;
            }
        }
         
        if( is_cell_empty(r1+2,c1-1) || cell_has_black(r1+2, c1-1) ) 
        { 
            if(r1+2 <= 7 && r1+2 >= 0 && c1-1 >= 0 && c1-1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1+1,c1+2) || cell_has_black(r1+1, c1+2) ) 
        {  
            if(r1+1 <= 7 && r1+1 >= 0 && c1+2 >= 0 && c1+2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-1,c1+2) || cell_has_black(r1-1, c1+2) ) 
        {  
            if(r1-1 <= 7 && r1-1 >= 0 && c1+2 >= 0 && c1+2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1+2;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-2,c1-1) || cell_has_black(r1-2, c1-1) )
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1-1 >= 0 && c1-1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1-1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-2,c1+1) || cell_has_black(r1-2, c1+1) ) 
        {
            if(r1-2 <= 7 && r1-2 >= 0 && c1+1 >= 0 && c1+1 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-2)*10+c1+1;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1+1,c1-2) || cell_has_black(r1+1, c1-2) ) 
        {
            if(r1+1 <= 7 && r1+1 >= 0 && c1-2 >= 0 && c1-2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1+1)*10+c1-2;
                possible_moves_index++;
            }
        }

        if( is_cell_empty(r1-1,c1-2) || cell_has_black(r1-1, c1-2) )
        {
            if(r1-1 <= 7 && r1-1 >= 0 && c1-2 >= 0 && c1-2 <= 7)
            {
                possible_moves[possible_moves_index] = (r1-1)*10+c1-2;
                possible_moves_index++;
            }
        }
    }
}
```

4. It may not be obvious as it looks like we aren't making much progress but we are -- we are slowly removing "cruft" that is letting us see the bigger picutre.

The next thing to do simply factor out all the redundant row and column calculations:

```c
void knight_v4( int r1 , int c1, int player )
{
    int r, c;
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        r = r1+2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) )
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        { 
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {  
            if(r <= 7 && r >= 0 && c >= 0 && c2 <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {  
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) )
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) )
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }
    }
    else
    if (player == PLAYE_WHITE)
    {
        r = r1+2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) )
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        { 
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {  
            if(r <= 7 && r >= 0 && c >= 0 && c2 <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {  
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) )
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) )
        {
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }
    }
}
```

5. With less semantics to parse we can see there is a common pattern of checking if r,c are in bounds.

We'll use a helper function since compilers these days are smart enough to inline trivial functions like this.

```c
    // ----------------------------------------
    int is_off_board ( int row, int col )
    {
        if ((row < 0) || (row > 7)) return true;
        if ((col < 0) || (col > 7)) return true;
        return false;
    }
```

Using that:

```c
void knight_v5( int r1 , int c1, int player )
{
    int r, c;
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        r = r1+2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) )
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        { 
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {  
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {  
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) )
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) ) 
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_white(r,c) )
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }
    }
    else
    if (player == PLAYE_WHITE)
    {
        r = r1+2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) )
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+2; c = c1-1;
            if (!is_off_board(r,c))
        { 
            if(r <= 7 && r >= 0 && c >= 0 && c <= 7)
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {  
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1+2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {  
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1-1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) )
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-2; c = c1+1;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1+1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) ) 
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }

        r = r1-1; c = c1-2;
        if( is_cell_empty(r,c) || cell_has_black(r,c) )
        {
            if (!is_off_board(r,c))
            {
                possible_moves[possible_moves_index] = r*10+c;
                possible_moves_index++;
            }
        }
    }
}
```

6. This code actually as bug where it checks an out-of-bounds array THEN checks if the coordinates are in bounds.  Reversing the test will fix this buggy code.

This simplies to:

```c
void knight_v6( int r1 , int c1, int player )
{
    int r, c;
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        r = r1+2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1+2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1+1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1+1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }
    }
    else
    if (player == PLAYE_WHITE)
    {
        r = r1+2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1+2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1+1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1+1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }

        r = r1-1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
        {
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;
        }
    }
}
```

7. This function is manually encoding a position as a 2-digit number: R*10 + C.  Again we will use a helper function:

```c
    // ----------------------------------------
    int row_col_to_position ( int row, int col )
    {
        int position = (row * 10) + col;
        return position;
    }
```

Also, we can use the post increment operator to combine keeping tracking of the total number of potential movies with this one line:

```
            possible_moves[possible_moves_index] = r*10+c;
            possible_moves_index++;

becomes

            possible_moves[possible_moves_index] = row_col_to_position(r,c);
            possible_moves_index++;

becomes
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);
```

Our new knight function is:

```c
void knight_v7( int r1 , int c1, int player )
{
    int r, c;
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        r = r1+2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_white(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);
    }
    else
    if (player == PLAYE_WHITE)
    {
        r = r1+2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_black(r,c)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);
    }
}
```

8. We still have a lot of duplicate code.  If only we could get rid of that `cell_has_white(r,c)` and `cell_has_black(r,c)` calls.

Actually we can. Instead of using 1 and 2 for black and white players respectively we can use 0 and 1 for white and black respectively.

We then can programmatically calculate the opponent via this simple calculation:

```c
    int opponent = 1 - player;
```

Again we have another utility/wrapper function:

```
    // ----------------------------------------
    bool cell_has_player ( int row, int col, int player )
    {
        if (player == PLAYER_WHITE)
            return cell_has_white( row, col );
        else
            return cell_has_black( row, col );
    }
```

This leaves:

```c
void knight_v8( int r1 , int c1, int player )
{
    int r, c;
    int opponent = 1 - player;
    possible_moves_index = 0;

    if (player == PLAYER_BLACK)
    {
        r = r1+2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);
    }
    else
    if (player == PLAYE_WHITE)
    {
        r = r1+2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1+2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1-1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-2; c = c1+1;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1+1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);

        r = r1-1; c = c1-2;
        if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
            possible_moves[possible_moves_index++] = row_col_to_position(r,c);
    }
}
```

9. With the code being identical for black and white players we can simply remove the whole "then" clause:

```c
void knight_v9( int r1 , int c1, int player )
{
    int r, c;
    int opponent = 1 - player;
    possible_moves_index = 0;

    r = r1+2; c = c1+1;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1+2; c = c1-1;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1+1; c = c1+2;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1-1; c = c1+2;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1-2; c = c1-1;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1-2; c = c1+1;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1+1; c = c1-2;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);

    r = r1-1; c = c1-2;
    if ( !is_off_board(r,c) && (is_cell_empty(r,c) || cell_has_player(r,c,opponent)) )
        possible_moves[possible_moves_index++] = row_col_to_position(r,c);
}
```

10. Each of the 8 `if`-then clauses are identical. We can write another utility/wrapper to factor out all the common code:

```
    // ----------------------------------------
    void add_possible_move ( int row, int col, int opponent )
    {
        if ( !is_off_board(row,col) && (is_cell_empty(row,col) || cell_has_player(row,col,opponent)) )
           possible_moves[possible_moves_index++] = row_col_to_position(row,col);
    }
```

We end up with:

```c
void knight_v10( int r1 , int c1, int player )
{
    int r, c;
    int opponent = 1 - player;
    possible_moves_index = 0;

    r = r1+2; c = c1+1; add_possible_move(r, c, opponent);
    r = r1+2; c = c1-1; add_possible_move(r, c, opponent);
    r = r1+1; c = c1+2; add_possible_move(r, c, opponent);
    r = r1-1; c = c1+2; add_possible_move(r, c, opponent);
    r = r1-2; c = c1-1; add_possible_move(r, c, opponent);
    r = r1-2; c = c1+1; add_possible_move(r, c, opponent);
    r = r1+1; c = c1-2; add_possible_move(r, c, opponent);
    r = r1-1; c = c1-2; add_possible_move(r, c, opponent);
}
```

11. All those row and calculations are begging to be simplied. We can store each of the knights destination in a table of relative offsets. This table is "transposed" from the vertical code into a horizontal one.

```
    int knight_row[8] = { +2, +2, +1, -1, -2, -2, +1, -1 };
    int knight_col[8] = { +1, -1, +2, +2, -1, +1, -2, -2 };
```
 
We can re-arrange the table so the pair <row,col> is sorted -- first by row, then col -- for bottom, left, top, right order:

```
    //  clock positions =  8   9   7  10   5   1   4   3
    int knight_row[8] = { -2, -2, -1, -1, +1, +1, +2, +2 };
    int knight_col[8] = { -1, +1, -2, +2, -2, +2, +1, -1 };
```

We end up with:

```c
void knight_v11( int r1 , int c1, int player )
{
    int r, c;
    int opponent = 1 - player;
    possible_moves_index = 0;

    int knight_row[8] = { -2, -2, -1, -1, +1, +1, +2, +2 };
    int knight_col[8] = { -1, +1, -2, +2, -2, +2, +1, -1 };

    for( i = 0; i < 8; i++ )
    {
        r = r1 + knight_row[i];
        c = c1 + knight_col[i];
        add_possible_move( r, c, opponent );
    }
}
```

12. With a **data-driven approach** we can abstract this even more by adding our final utility/wrapper:

```c
    // ----------------------------------------
    void add_possible_array ( int positions, int *rows, int *cols, int row, int col, int player )
    {
        int opponent = 1 - player;
        for( int position = 0; position < positions; position++ )
        {
            int r = row + rows[ position ];
            int c = col + cols[ position ];
            add_possible_move( r, c, opponent );
        }
    }
```

Our final knight function becomes this beauty!

```
void knight_v12( int r1 , int c1, int player )
{
    possible_moves_index = 0;

    int knight_row[8] = { -2, -2, -1, -1, +1, +1, +2, +2 };
    int knight_col[8] = { -1, +1, -2, +2, -2, +2, +1, -1 };
    add_possible_array( sizeof(knight_row)/sizeof(int), knight_row, knight_col, r1, c1, player );
}
```

The cool thing about this pattern is that we can use this pattern for the rook, bishop, and queen just by changing the table!

Let's do that for the rook.

First, we need to model movement along an axis -- both principal ones and diagonals:

```c
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
```

Next, we'll need a delta row and delta column that we use to update our cell location as "walk" along this direction.

```c
    //                                        E  NE   N  NW   W  SW   S  SE
    const int axis_delta_col[ NUM_AXIS ] = { +1, +1,  0, -1, -1, -1,  0, +1 };
    const int axis_delta_row[ NUM_AXIS ] = {  0, -1, -1, -1,  0, +1, +1, +1 };
```

The algorithm need for "tracing" how far a "ranged" piece can move is pretty straight forward:

1. We only need to check 7 cells along a direction
2. For each cell we update our position.
3. If we are off the board we stop.
4. If we hit an opponent piece we add that to the candidate move and stop.
5. If we hit our own piece we stop.
6. We must have an empty cell so we append it to the possible moves list.

Putting that into code:

```c
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
```

Our rook code becomes this:

```c
void moves_rook ( int r1 , int c1, int player)
{
    gn_possible_moves = 0;

    add_possible_axis( r1, c1, player, AXIS_E );
    add_possible_axis( r1, c1, player, AXIS_N );
    add_possible_axis( r1, c1, player, AXIS_W );
    add_possible_axis( r1, c1, player, AXIS_S );
}
```

Our bishop code becomes this:

```c
void moves_bishop ( int r1 , int c1, int player)
{
    gn_possible_moves = 0;

    add_possible_axis( r1, c1, player, AXIS_NE );
    add_possible_axis( r1, c1, player, AXIS_NW );
    add_possible_axis( r1, c1, player, AXIS_SW );
    add_possible_axis( r1, c1, player, AXIS_SE );
}
```

And our queen code becomes this beauty:

```c
void moves_queen ( int r1 , int c1, int player )
{
    gn_possible_moves = 0;

    for (int axis = 0; axis < NUM_AXIS; axis++ )
        add_possible_axis( r1, c1, player, axis );
}
```

Not too shabby!

You may also enjoy reading Casey's article on [Semantic Compression](https://caseymuratori.com/blog_0015)

m007.
