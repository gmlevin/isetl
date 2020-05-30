#include "common.h"
/* $Header: mp.c,v 3.1 90/04/25 16:23:29 gary Locked $ */
#include "micro.h"
#include "manager.h"

/* See mp.h for definitions of Bignum */

/* Convert Integer to Bignum */
IPtr     New_Big(x)
    WORKING x;
{
    ENTER;
    int     i;
    Reg     (new);

    R(new) = New_MP(3);

    Sign(R(new)) = (x < 0 ? (x = -x, -1) : 1);

    for (i = 0; x > 0; i++) {
	Digits(R(new))[i] = Low(x);
	x = High(x);
    }
    mpNorm(R(new));
    LEAVE   R(new);
}


/* (x, val):Bool *val:= Integer value of x if it fits
 * Returns true if succeeds, false ow.
 */
Bool    mpToInt(x, val)
    IPtr     x;
    WORKING *val;
{
    switch (Kind(x)) {
      case Bignum:
	if (Length(x) > 2)
	    return false;
	*val = 0;
	if (Length(x) >= 1)
	    *val += Digits(x)[0];
	if (Length(x) >= 2)
	    *val += Scale_Up(Digits(x)[1]);
	*val *= Sign(x);
	return true;

      case Integer:
	*val = Int_Value(x);
	return true;

      default:
	return false;
    }
}

/*  New_MP(size)	returns MP record */
IPtr     New_MP(size)
    int     size;
{
    ENTER;
    int     i;
    Reg     (new);

    if (size <= 0)
	size = 1;
    R(new) = New_Block(Bignum, size);
    for (i = 0; i < size; i++)
	Digits(R(new))[i] = 0;
    Length(R(new)) = size;
    Sign(R(new)) = 0;
    LEAVE   R(new);
}



/*
 * Returns normalized n (integer if possible).
 * Side-effect of adjusting Length(n), stripping leading zeros
 */
IPtr     mpNorm(n)
    IPtr     n;
{
    ENTER;
    Reg     (new);

    if (Kind(n) == Integer) {
	LEAVE   n;
    }
    assert(Kind(n) == Bignum);
    while (Length(n) != 0 && Digits(n)[Length(n) - 1] == 0)
	Length(n)--;

    if (Length(n) <= 1)
	R(new) = New_Integer((WORKING) (Sign(n) * Digits(n)[0]));
    else
	R(new) = n;
    LEAVE   R(new);
}



/* Unsigned Addition or Subtraction (specified by do_add) 
 * Assumes non-negative result
 */
IPtr     mpUAddSub(a, b, do_add)
    IPtr     a,
            b;
    Bool    do_add;
{
    ENTER;
    Reg     (new);
    int     i;
    int     carry = 0;

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    R(new) = New_MP(max(Length(a), Length(b)) + 1);
    for (i = 0; i < Length(R(new)); i++) {
	WORKING digit;

	if (i < Length(a))
	    digit = Digits(a)[i];
	else
	    digit = 0;
	if (i < Length(b))
	    digit = digit + (do_add ? Digits(b)[i] : -Digits(b)[i]);
	digit += carry;

	if (do_add ? (digit >= BASE) : (digit < 0)) {
	    digit = digit + (do_add ? -BASE : BASE);
	    carry = (do_add ? 1 : -1);
	} else {
	    carry = 0;
	}
	Digits(R(new))[i] = digit;
    }
    Sign(R(new)) = 1;
    LEAVE   R(new);
}

/* mpCmp(a,b)		returns >0 if a>b,   =0 if a=b,   <0 if a<b */
int     mpCmp(a, b)
    IPtr     a,
            b;
{
    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    if (Sign(a) != Sign(b))
	return Sign(a);
    return mpUCmp(a, b) * Sign(a);
}

/* Unsigned comparison (same results as mpComp) */
int     mpUCmp(a, b)
    IPtr     a,
            b;
{
    int     i;
    DIGIT   temp;

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    temp = Length(a) - Length(b);

    if (temp != 0)
	return temp;

    for (i = Length(a) - 1; i >= 0; i--) {
	temp = Digits(a)[i] - Digits(b)[i];
	if (temp != 0)
	    return temp;
    }
    return 0;
}


/* Add or Sub based on do_add */
IPtr     mpAddSub(a, b, do_add)
    IPtr     a,
            b;
    Bool    do_add;
{
    ENTER;
    Reg     (new);

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    if ((Sign(a) == Sign(b)) == do_add) { /* adding absolute values */
	R(new) = mpUAdd(a, b);
	Sign(R(new)) = Sign(a);
    } else if (mpUCmp(a, b) >= 0) { /* subtract small from large */
	R(new) = mpUSub(a, b);
	Sign(R(new)) = Sign(a);
    } else {
	R(new) = mpUSub(b, a);
	Sign(R(new)) = (do_add ? Sign(b) : -Sign(a));
    }
    R(new) = mpNorm(R(new));
    LEAVE   R(new);
}


/* mpcpy(a,b)		assigns b to a, assumes there is room.  */
void    mpcpy(a, b)
    IPtr     a,
            b;
{
    int     i;

    assert(Kind(a) == Bignum && Kind(b) == Bignum && Length(a) >= Length(b));

    Sign(a) = Sign(b);
    Length(a) = Length(b);
    for (i = Length(a) - 1; i >= 0; i--)
	Digits(a)[i] = Digits(b)[i];
}


/*  mpTimes(a,b)		returns a*b */
IPtr     mpTimes(a, b)
    IPtr     a,
            b;
{
    ENTER;
    Reg     (new);

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    R(new) = mpUTimes(a, b);
    Sign(R(new)) = Sign(a) * Sign(b);
    R(new) = mpNorm(R(new));
    LEAVE   R(new);
}

/* Unsigned times */
IPtr     mpUTimes(a, b)
    IPtr     a,
            b;
{
    ENTER;
    Reg     (new);
    int     i;

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    R(new) = New_MP(Length(a) + Length(b) + 1);  /* approximate length of result, overestimate */
    for (i = 0; i < Length(b); i++) { /* add a*b[i] shifted to result */
	IPtr     result = R(new);
	DIGIT   m = Digits(b)[i];
	int     shift = i;
	int     j;
	DIGIT   carry = 0;
	WORKING digit;
	register DIGIT *s,
	       *r;

	s = &Digits(a)[0];
	r = &Digits(result)[shift];

	for (j = 0; j < Length(a); j++, s++, r++) {
	    digit = *s * (WORKING) m + (WORKING) carry + *r;
	    *r = Low(digit);
	    carry = High(digit);
	}

	for (; carry != 0; r++) {
	    digit = (WORKING) carry + *r;
	    *r = Low(digit);
	    carry = High(digit);
	}

    }
    LEAVE   R(new);
}

/* result := result - a*(m*BASE**shift) (unsigned) */
void    mpUTSub(result, a, m, shift)
    IPtr     result,
            a;
    DIGIT   m;
    int     shift;
{
    int     i;
    DIGIT   carry = 0;		       /* carry <= 0 */
    WORKING digit;

    if( Length(a)+shift > Length(result) )  return;  /* too big to subtract */

    for (i = 0; i < Length(a); i++) {
	digit = Digits(result)[i + shift] - Digits(a)[i] * (WORKING) m
	    + (WORKING) carry;
	Digits(result)[i + shift] = Low(digit);
	carry = High(digit);
    }

    for (; carry != 0; i++) {
	assert(i + shift < Length(result));

	digit = (WORKING) carry + Digits(result)[i + shift];
	Digits(result)[i + shift] = Low(digit);
	carry = High(digit);
    }
    mpNorm(result);
}

/*  Compare a to b times BASE**shift (unsigned) */
int     mpUCmpTimes(a, b, shift)
    IPtr     a,
            b;
    int     shift;
{
    int     i;
    DIGIT   temp;

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    temp = Length(a) - (Length(b) + shift);

    if (temp != 0)
	return temp;

    for (i = Length(a) - 1; i >= 0; i--) {
	if (i >= shift)
	    temp = Digits(a)[i] - Digits(b)[i - shift];
	else
	    temp = Digits(a)[i];
	if (temp != 0)
	    return temp;
    }
    return 0;
}


/* mpUDivMod(a,b,&q,&r) assign *q and *r: a = q*b+r (absolute values) */
void    mpUDivMod(a, b, q, r)
    IPtr     a,
            b,
           *q,
           *r;
{
    int     place;
    WORKING trialr;	/* approximation of first digits of r */
    double  trialb;	/* approximation of first digits of b */
    DIGIT   m;		/* quess at quotient digit (low) */

    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    place = Length(a) - Length(b);

    *r = New_MP(Length(a));
    mpcpy(*r, a);
    Sign(*r) = 1;
    *q = New_MP(max(place,0) + 1);
    Sign(*q) = 1;


    /* choose trialb to approximate the leading digits of b */
    trialb = Digits(b)[Length(b) - 1];
    if (Length(b) > 1) {
	trialb = trialb + (Digits(b)[Length(b) - 2] + 1) / (double) BASE;
    }
    trialr = 0;

    for (; place >= 0; place--) {
	trialr = Scale_Up(trialr) + Digits(*r)[Length(b) + place - 1];

	m = trialr / trialb;
	mpUTSub(*r, b, m, place);

	while (mpUCmpTimes(*r, b, place) >= 0) {
	    mpUTSub(*r, b, (DIGIT) 1, place);
	    m++;
	}
	Digits(*q)[place] = m;

	trialr = Digits(*r)[Length(b) + place - 1];
    }
}

/* works correctly for negative and positive divisors */
void    mpDivMod(a, b, q, r)
    IPtr     a,
            b,
           *q,
           *r;
{
    assert(Kind(a) == Bignum && Kind(b) == Bignum);
    mpUDivMod(a, b, q, r);

    mpNorm(*q);
    mpNorm(*r);
    assert(Sign(*r) == 1 && Sign(*q) == 1);

    /* |a| = q*|b| + r, 0<=r<|b| */
    if (Sign(a) < 0) {
	Sign(*q) = -1;
	/* a = q*|b| - r = (q-1)*|b| + |b| - r, 0<=r<|b| */
	if (Length(*r) > 0) {
	    *q = mpSub(*q, BigOne);
	    *q = mpForce(*q);
	    *r = mpUSub(b, *r);
	}
    }

    /* a = q*|b| + r, 0<=r<|b| */
    if (Sign(b) < 0) {
	Sign(*q) = -Sign(*q);
    }
    *q = mpNorm(*q);
    *r = mpNorm(*r);
    /* a = q*b + r, 0<=r<|b| */
}

IPtr     mpFloat(x)
    IPtr     x;
{
    C_Real  f;
    int     i;

    switch (Kind(x)) {
      case Bignum:
	f = 0;
	for (i = Length(x) - 1; i >= 0; i--) {
	    f = f * BASE + Digits(x)[i];
	}
	return New_Real(Sign(x) * f);
      case Real:
	return x;
      case Integer:
	return New_Real((C_Real) Int_Value(x));
      case Rational:
	return New_Real(Real_Value(mpFloat(Num(x)))
			/ Real_Value(mpFloat(Den(x))));
      default:
	assert(0);
    }
}

IPtr     mpFix(x)
    C_Real  x;
{
    ENTER;
    int     places;
    C_Real  q,
            f;
    DIGIT   r;
    Reg     (result);

    f = x;
    if (f < 0)
	f = -f;
    for (places = 0; f >= 1; places++) {
	f = f / BASE;
    }
    R(result) = New_MP(places + 1);

    f = x;
    if (f < 0) {
	f = -f;
	Sign(R(result)) = -1;
    } else {
	Sign(R(result)) = 1;
    }
    for (places = 0; f >= 1; places++) {
	q = floor(f / BASE);
	r = f - q * BASE;
	if (r < 0)
	    r = f - q * (BASE - 1);    /* handles roundoff errors */
	Digits(R(result))[places] = r;
	f = q;
    }

    R(result) = mpNorm(R(result));
    LEAVE   R(result);
}

#define BASEDIGITS	3
#define cTOd(p)		((p) - '0')

/* Only handles unsigned quantities */
IPtr     mpStrToInt(s)
    char   *s;
{
    ENTER;
    Reg     (new);
    Reg     (small);

    R(small) = New_MP(1);	       /* will be reused for each digit */
    Sign(R(small)) = 1;
    R(new) = New_MP(1);		       /* default value is zero */

    while (*s) {
	Digits(R(small))[0] = cTOd(*s);
	R(new) = mpUTimes(R(new), BigTen);
	R(new) = mpUAdd(R(new), R(small));
	s++;
    }

    R(new) = mpNorm(R(new));

    LEAVE   R(new);
}

IPtr     mpForce(x)
    IPtr     x;
{
    switch (Kind(x)) {
      case Bignum:
	return x;
      case Integer:
	return New_Big((WORKING) Int_Value(x));
      default:
	assert(0);
    }
}

/*
 * (i,j,step): return list i..j by step
 */
IPtr     mpCnt(i, j, step)
    IPtr     i,
            j,
            step;
{
    ENTER;
    Reg     (ans);
    Reg     (start);
    Reg     (stop);

    R(start) = i;
    R(stop) = j;

    R(ans) = Nil;

    if (Length(step) != 0) {
	while (Sign(step) * mpCmp(R(start), R(stop)) <= 0) {
	    R(start) = mpNorm(R(start));
	    R(ans) = mcCons(R(start), R(ans));

	    R(start) = mpForce(R(start));
	    R(start) = mpAdd(R(start), step);
	    R(start) = mpForce(R(start));
	}
    }
    R(ans) = mcRev(R(ans));
    LEAVE   R(ans);
}

#define DIGITBLOCK 10000

#define mpOutput(str,f)	 if(free)show(str,f);else mcfprintf(f,"%s",str,0)

/* free	 ==> use show */
/* !free ==> pad to width, break after prec digits */
void    mpPrint(x, f, free, width, prec)
    IPtr     x;
    C_File  f;
    Bool    free;
    int     width,
            prec;
{
    ENTER;
    Reg     (s);
    Reg     (r);
    int     digits = 2 + Length(x) * (BASE_BITS * 0.302);

    /* log[10](2) = 0.3010... */
    int     len;
    char   *p;
    char    c[2];

    c[1] = 0;

    R(s) = New_Block(String, digits);
    p = Str_Value(R(s));



    if (Length(x) == 0) {
	*p = '0';
	p++;
    } else {
	R(r) = New_MP(Length(x));
	mpcpy(R(r), x);
	Sign(R(r)) = 1;

	while (Length(R(r)) > 0) {
	    DIGIT   rem = 0;
	    int     i;

	    assert(digits > 0);
	    /* R(r) = R(r) div DIGITBLOCK; rem = R(r) mod DIGITBLOCK */
	    for (i = Length(R(r)) - 1; i >= 0; i--) {
		WORKING d = Digits(R(r))[i] + Scale_Up(rem);

		Digits(R(r))[i] = d / DIGITBLOCK;
		rem = d % DIGITBLOCK;
	    }

	    mpNorm(R(r));
	    /* convert DIGITBLOCK to chars */
	    *p = (rem % 10) + '0';
	    rem = rem / 10;
	    p++;
	    *p = (rem % 10) + '0';
	    rem = rem / 10;
	    p++;
	    *p = (rem % 10) + '0';
	    rem = rem / 10;
	    p++;
	    *p = (rem % 10) + '0';
	    rem = rem / 10;
	    p++;
	    digits = digits - 4;
	}

	do
	    p--;
	while (*p == '0');
    }
    p++;

    if (Sign(x) < 0) {
	*p = '-';
	p++;
    }
    /* print digits (and sign) */
    len = (p - Str_Value(R(s)));
    if (!free && width >= len) {
	int     i;

	for (i = 0; i < width - len; i++) {
	    mcfputc(' ', f);
	}
    }
    len = 0;
    do {
	p--;
	c[0] = *p;
	mpOutput(c, f);
	len++;
	if (!free && len + 1 == prec) {
	    mpOutput("\\\n", f);
	    len = 0;
	}
    } while (p != Str_Value(R(s)));
    LEAVE;
}
