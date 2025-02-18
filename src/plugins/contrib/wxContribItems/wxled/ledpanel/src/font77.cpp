/***************************************************************
 * Name:      font77.cpp
 * Purpose:   Code for the Font with Letters in format 7x7
 * Author:    Christian Gr�fe (info@mcs-soft.de)
 * Created:   2007-03-24
 * Copyright: Christian Gr�fe (www.mcs-soft.de)
 * License:	  wxWindows licence
 **************************************************************/
const int font77_letterWidth = 7;
const int font77_letterHeight = 7;
const char font77_LettersData[95 * 7 * 7] =
{
    //' ',		// 0 -> SPACE
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'!',		// 1 -> !
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    //'"',		// 2 -> "
    0, 0, 1, 1, 0, 1, 1,
    0, 0, 1, 1, 0, 1, 1,
    0, 0, 0, 1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'#',		// 3 -> #
    0, 1, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 1, 0,
    //' ',		// 4 -> $
    0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 0,
    //'%',		// 5 -> %
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 0, 0, 0, 1, 1, 0,
    //'&',		// 6 -> &
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 0, 0, 1, 0,
    0, 0, 1, 0, 1, 0, 0,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 0, 1,
    0, 0, 1, 0, 0, 1, 0,
    0, 0, 0, 1, 1, 0, 1,
    //'\'',		// 7 -> '
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'(',		// 8 -> (
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    //')',		// 9 -> )
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    //'*',		// 10 -> *
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 1, 0, 1,
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 1, 0, 1, 0, 1,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'+',		// 11 -> +
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //',',		// 12 -> ,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    //'-',		// 13 -> -
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'.',		// 14 -> .
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    //'/',		// 15 -> /
    0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    //'0',		// 16 -> 0
    0, 1, 1, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 0,
    //'1',		// 17 -> 1
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1,
    //'2',		// 18 -> 2
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1,
    //'3',		// 19 -> 3
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'4',		// 20 -> 4
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 0,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    //'5',		// 21 -> 5
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'6',		// 22 -> 6
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'7',		// 23 -> 7
    1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    //'8',		// 24 -> 8
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'9',		// 25 -> 9
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    //':',		// 26 -> :
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0,
    //';',		// 27 -> ;
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    //'<',		// 28 -> <
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'=',		// 29 -> =
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'>',		// 30 -> >
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'?',		// 31 -> ?
    0, 0, 1, 1, 1, 1, 0,
    0, 1, 1, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    //'@',		// 32 -> @
    0, 1, 1, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 1, 0, 1, 1,
    1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1,
    //'A',		// 33 -> A
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    //'B',		// 34 -> B
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    //'C',		// 35 -> C
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'D',		// 36 -> D
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    //'E',		// 37 -> E
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1,
    //'F',		// 38 -> F
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    //'G',		// 38 -> G
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'H',		// 40 -> H
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    //'I',		// 41 -> I
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1,
    //'J',		// 42 -> J
    0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 0,
    //'K',		// 43 -> K
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 1, 1, 0, 0,
    1, 1, 1, 1, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    //'L',		// 44 -> L
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    //'M',		// 45 -> M
    1, 0, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    //'N',		// 46 -> N
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 0, 0, 1, 1,
    1, 1, 1, 1, 0, 1, 1,
    1, 1, 0, 1, 1, 1, 1,
    1, 1, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    //'O',		// 47 -> O
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'P',		// 48 -> P
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    //'Q',		// 49 -> Q
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 1, 1, 1, 1,
    1, 1, 0, 0, 1, 1, 0,
    0, 1, 1, 1, 0, 1, 1,
    //'R',		// 50 -> R
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    //'S',		// 51 -> S
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'T',		// 52 -> T
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    //'U',		// 53 -> U
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'V',		// 54 -> V
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    //'W',		// 55 -> W
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    //'X',		// 56 -> X
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 0, 1,
    //'Y',		// 57 -> Y
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    //'Z',		// 58 -> Z
    1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1,
    //'[',		// 59 -> [
    0, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 1,
    // 60 ->
    1, 1, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 1,
    //']',		// 61 -> ]
    0, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 1, 1, 1, 1,
    //'^',		// 62 -> ^
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    //'_',		// 63 -> _
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1,
    //'`',		// 64 -> `
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    /////////////////////////////
    // a - z
    /////////////////////////////
    //'a',		// 65 -> a
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    //'b',		// 66 -> b
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    //'c',		// 67 -> c
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'d',		// 68 -> d
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    //'e',		// 69 -> e
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    //'f',		// 70 -> f
    0, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    //'g',		// 71 -> g
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'h',		// 72 -> h
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    //'i',		// 73 -> i
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    //'j',		// 74 -> j
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 1, 1, 1, 0,
    //'k',		// 75 -> k
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 1, 1, 0, 0,
    1, 1, 1, 1, 0, 0, 0,
    1, 1, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    //'l',		// 76 -> l
    0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 1, 1,
    //'m',		// 77 -> m
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 0, 1, 1, 0,
    1, 1, 0, 1, 0, 1, 1,
    1, 1, 0, 1, 0, 1, 1,
    1, 1, 0, 1, 0, 1, 1,
    1, 1, 0, 1, 0, 1, 1,
    //'n',		// 78 -> n
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 0,
    //'o',		// 79 -> o
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'p',		// 80 -> p
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 0, 0,
    //'q',		// 81 -> q
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    //'r',		// 82 -> r
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 1, 1,
    0, 0, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    //'s',		// 83 -> s
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'t',		// 84 -> t
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 1,
    //'u',		// 85 -> u
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'v',		// 86 -> v
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    //'w',		// 87 -> w
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 1, 0, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    //'x',		// 88 -> x
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1,
    //'y',		// 89 -> y
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    //'z',		// 90 -> z
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1,
    //'{',		// 91 -> {
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    //'|',		// 92 -> |
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0,
    //'}',		// 93 -> }
    0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 0,
    //'~',		// 94 -> ~
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 1, 0,
    0, 0, 1, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
