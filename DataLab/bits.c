/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
    /**
     * ~ and & ops are complete, which means they can construct any logic operator
     * De Morgan's laws implys that | op can be represented by ~ and &
     * x | y = ~(~x & ~y)   priority: ~ (2) much higher & (8)
     * x xor y = ~x&y | x&~y
     */
    return ~(~(~x & y) & ~(x & ~y));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void)
{
    /**
     * the minimum 32 bit signed number is 0x80000000
     */
    return 0x80 << 24;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x)
{
    /**
     * Tmax should be 0x7fffffff
     * ^ (9) just blew & (8), both blew << (5)
     * first solution but wrong: return !(~x ^ 0x80 << 24);
     * !!4 == 1 : True
     * ~(T unsigned max[0xffffffff] + 1) == T unsigned max
     * except the above case
     * + (4)
     * thoughts:
     *      plus one and inverse is itself, except -1 case
     */
    return !(~(x + 1) ^ x) & !!(x + 1);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
    /**
     * 0x55 = 0101 0101
     * parentheses just to avoid warning
     */
    return !((((((((0x55 << 8) + 0x55) << 8) + 0x55) << 8) + 0x55) | x) + 1);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x)
{
    /**
     * 2's complement negate: inverse and puls one
     */
    return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x)
{
    /**
     * 0011 0000 -> 0011 1001
     * ./dlc do not support 0b110 notation
     * two cases : start with 110 or 111
     */
    return (!(x >> 3 ^ 0x6)) | (!(x >> 3 ^ 0x7) & !(x & 0x6));
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z)
{
    /**
     * y^z^z = y, so make a choice
     * mask is 0xffffffff
     * using valuable is good style
     */
    int mask = ~0;
    // int mask = (((((0xff << 8) + 0xff) << 8) + 0xff) << 8) + 0xff;
    int choice = (y & (!!x + mask)) + (z & (!x + mask));
    return y ^ z ^ choice;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
    /**
     * -2147483648[0x80000000] inverse and plus 1 is itself!
     * first detect the sign
     * do not consider y to be Tmin
     */
    // sign not the same
    int signx = x >> 31;
    int signy = y >> 31;
    int x_neg_y_pos = !((signx + 1) ^ signy); //positive caes use |
    int y_neg_x_pos = !((signy + 1) ^ signx); //negative case use &
    // sign bit is same
    int nx = ~x + 1;
    int xisTmin = !(nx ^ x);
    int sum = nx + y;
    /*if x and y are same sign and x is not Tmin then if sum sign = 0, it is true
    no underflow or overflow cases in this condition */
    return (x_neg_y_pos | (!(sum >> 31)) | xisTmin) & !y_neg_x_pos;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x)
{
    /**
     * if x is zero then zeroflag 32bits are all set to 0
     *      int zeroflag = ((~x + 1) ^ x) | (x >> 31);
     * the above method do not work
     * it utilizes the fact that only zero and -1 changes the sign bit when subtracted by 1
     * also notice that 0->1 and non-zero -> 0, it is negate
     * TRICK : -1 == ~0
     */
    int minus1flag = (((x + ~0) ^ x) >> 31) & 1;
    // to make conditions smaller, use &
    return (~minus1flag + 1) & (((x >> 31) + 1) & 1);
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x)
{
    /**
     * after formalizaton, the only thing to do is to count the position 
     * of the last leading zero, without any control sequence.
     * the return value recorded each time by =
     *      int sign = x >> 31;
     *      int fmt = sign ^ x;
     * 
     * so the above thoughts are completely wrong 
     * ref:http://www.hh-yzm.com/index.php/archives/6/
     * it is't a bit-wise examination
     * but a 2-divid trick
     * 
     * use 32 rounds will certainly exceeds the limit 
     * use 2-fold-divid
     */
    int sign = x >> 31;
    int fmt = sign ^ x;
    int base = 0x0;
    // check the first half (16 digits) where there is a one
    // then move the base pointer
    // each line base will gets changed by =+
    base += (!!(((~0) << 16) & fmt)) << 4;
    base += (!!(((~0) << (base + 8)) & fmt)) << 3;
    base += (!!(((~0) << (base + 4)) & fmt)) << 2;
    base += (!!(((~0) << (base + 2)) & fmt)) << 1;
    base += (!!(((~0) << (base + 1)) & fmt));
    // the current digit
    base += (!!(((~0) << (base)) & fmt));
    // divid and conquer is the most powerful tool!
    return base + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf)
{
    /**
     * take care of the denormalized case is not just add exponent by 1
     * do not debug by printf, it is pretty slow
     */
    unsigned mask = 0x7f800000;
    unsigned exponent = (uf & mask) >> 23;
    /**
     * 1 sign bit + 8 exponent bits (biased: - 0x7f[127:2^7-1] ) +23 fraction bits
     */
    if (exponent == 0xff) { // if is inf(frac = 0) or NaN(frac != 0)
        return uf;
    } else if (exponent == 0) { // denormalized: left shift 1 bit and remain sign bit
        uf = (uf << 1) | (uf & 0x80000000);
        return uf;
    } else { // exponent plus 1
        uf += 1 << 23;
        return uf;
    }
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf)
{
    /**
     * take care of the sign bit
     * int32 can represent : -2^31 to 2^31 - 1
     * shifting the exponent
     * the same as casting type from float to int
     */
    unsigned mask = 0x7f800000;
    unsigned expo = (uf & mask) >> 23;
    if (expo >= 32 + 0x7f) { //too large
        return 0x80000000u;
    } else if (expo < 0 + 0x7f) { //1.xx left shfit is always zero
        return 0x0;
    } else {
        int ret = (0x00800000 | uf) & 0x00ffffff;
        int sign = uf >> 31;
        ret = ret >> (23 - expo + 0x7f);
        if (sign != 0x0) { //negative case
            ret = ~ret + 1;
        }
        return ret;
    }
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x)
{
    /**
     * exponent: -126 to 127 (128 inf, 0 denormalize)
     * pow(2,x) can only be positive
     * 
     * simple is better than complex
     */
    int exp = 0x7f + x;
    if (exp >= 255) {
        return 0x7f800000;
    } else if (exp < 0) {
        return 0x0;
    } else {
        return exp << 23;
    }
}
