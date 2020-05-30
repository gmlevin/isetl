#ifndef max
#define max(a,b)  ((a) > (b) ? (a) : (b))
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

/* multiple precision arithmetic */

/*
 * mpAdd(a,b)		returns a+b 
 * mpSub(a,b)		returns a-b
 * mpTimes(a,b)		returns a*b
 * mpDivMod(a,b,&q,&r) assign *q and *r: a = q*b+r
 *
 * mpUAdd, mpUSub, mpUTimes, mpUDivMod are the same, but for unsigned
 * quantities.  mpUSub(a,b) assumes a>=b. The results are not normalized
 * (may be small enough for Int).
 *
 * mpTimesPlus(a,b,mult,places) adds b*mult shifted places left directly to a
 * (in place).
 *
 * mpcpy(a,b)		assigns b to a, assumes there is room.
 *
 * mpCmp(a,b)		returns >0 if a>b,   =0 if a=b,   <0 if a<b 
 *
 * New_Big(int)	returns Bignum
 * New_MP(size)	returns MP record
 * mpNorm(big)  returns big normalized or Integer
 * mpFloat(big)	returns Real equivalent
 * mpFix(C_Real)	returns Big equivalent (truncated)
 * mpForce(IPtr)		converts Int to Big
 * mpStrToInt(STR)	returns Big or Int
 *
 * mpToInt(PTR X WORKING ADDR) returns true if converted, false on error
 * assigns value to WORKING ADDR
 *
 */

/*
    Represent multiple precision numbers by
	sign	-1 for negative +1 for positive (either for zero)
	number of digits (base BASE)
	digits, index = power

    BASE must be small enough so that BASE**2 < max long int.

    The high order digit will never be zero.
    If number of digits would be 1, use Integer.
    (Except for temporary results, MP are never zero.)
*/

#define mpUAdd(a,b)	mpUAddSub(a,b,true)
#define mpUSub(a,b)	mpUAddSub(a,b,false)
IPtr     mpUAddSub(PTR X PTR X BOOL);

#define mpAdd(a,b)	mpAddSub(a,b,true)
#define mpSub(a,b)	mpAddSub(a,b,false)
IPtr     mpAddSub(PTR X PTR X BOOL);

IPtr     mpNorm(PTR);
IPtr     New_MP(INT);
IPtr     New_Big(PWORKING);
IPtr     mpFloat(PTR);
IPtr     mpFix(C_REAL);
IPtr     mpForce(PTR);
IPtr     mpStrToInt(STR);
int     mpCmp(PTR X PTR);
int     mpUCmpTimes(PTR X PTR X INT);
int     mpUCmp(PTR X PTR);
IPtr     mpTimes(PTR X PTR);
IPtr     mpUTimes(PTR X PTR);
void    mpTimesPlus(PTR X PTR X PDIGIT X INT);
void    mpUTSub(PTR X PTR X PDIGIT X INT);
void    mpcpy(PTR X PTR);
void    mpDivMod(PTR X PTR X PTR ADDR X PTR ADDR);
void    mpUDivMod(PTR X PTR X PTR ADDR X PTR ADDR);

IPtr     mpCnt(PTR X PTR X PTR);
Bool    mpToInt(PTR X PWORKING ADDR);
void    mpPrint(PTR X C_FILE X BOOL X INT X INT);
