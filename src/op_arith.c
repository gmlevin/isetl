#include "common.h"
/* $Header: op_arith.c,v 3.2 90/04/25 16:23:36 gary Locked $ */
#include "instr.h"
#include "op.h"
#include "manager.h"
#include "micro.h"
#include "emitter.h"
/* Because these are ISETL operations, the arguments are on the ISETL
 * stack, not passed as C arguments.  Comments refer to the arguments
 * on the stack.  They are correctly accessed by the ENTER macros.
 * See op.h for an explanation of ENTER, LEAVE, R(X_Op).
 * See instr.h for further description of ISETL operations.
 */

/* (a,b):*  a+b */
void    opPlus()
{
    BIN_ENTER;

    if (Kind(R(Left_Op)) == Kind(R(Right_Op))) {
	switch (Kind(R(Left_Op))) {
	  case Integer:
	    R(ans) = New_Integer((WORKING) Int_Value(R(Left_Op))
				 + Int_Value(R(Right_Op)));
	    break;

	  case Bignum:
	    R(ans) = mpAdd(R(Left_Op), R(Right_Op));
	    break;

	  case Rational: 
	    R(ans)  = RatAdd(R(Left_Op),R(Right_Op));
	    break;

	  case Real:
	    R(ans) = New_Real(Real_Value(R(Left_Op))
			      + Real_Value(R(Right_Op)));
	    break;

	  case Set:
	    R(ans) = mcUnion(Set_Value(R(Left_Op)),
			     Set_Value(R(Right_Op)));
	    R(ans) = New_Set(R(ans));
	    break;

	  case String:
	    R(ans) = mcStr_Concat(R(Left_Op), R(Right_Op));
	    break;

	  case Tuple:
	    R(ans) = mcCat(Tuple_Value(R(Left_Op)),
			   Tuple_Value(R(Right_Op)));
	    R(ans) = New_Tuple(R(ans), Tuple_Origin(R(Left_Op)));
	    break;

	  default:
	    BAD_ARGS("+", R(Left_Op), R(Right_Op));
	}
    } else {
	/* mixed operations */
	switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
	  case Pair(Bignum,Rational):
	  case Pair(Rational,Bignum):
	  case Pair(Integer,Rational):
	  case Pair(Rational,Integer):
		R(ans)     = RatAdd(R(Left_Op), R(Right_Op));   
		break;


	  case Pair(Integer, Bignum):
	  case Pair(Bignum, Integer):{
		R(Left_Op) = mpForce(R(Left_Op));
		R(Right_Op) = mpForce(R(Right_Op));
		R(ans) = mpAdd(R(Left_Op), R(Right_Op));
	    } break;

	  case Pair(Real,Rational):
	  case Pair(Rational,Real):
	  case Pair(Integer, Real):
	  case Pair(Real, Integer):
	  case Pair(Bignum, Real):
	  case Pair(Real, Bignum):
	    R(Left_Op) = mpFloat(R(Left_Op));
	    R(Right_Op) = mpFloat(R(Right_Op));
	    R(ans) = New_Real(Real_Value(R(Left_Op))
			      + Real_Value(R(Right_Op)));
	    break;

	  default:
	    BAD_ARGS("+", R(Left_Op), R(Right_Op));
	}
    }
    BIN_LEAVE;
}



/* (a,b):*  a-b */
void    opMinus()
{
    BIN_ENTER;

    if (Kind(R(Left_Op)) == Kind(R(Right_Op))) {
	switch (Kind(R(Left_Op))) {
	  case Integer:
	    R(ans) = New_Integer((WORKING) Int_Value(R(Left_Op))
				 - Int_Value(R(Right_Op)));
	    break;

	  case Bignum:
	    R(ans) = mpSub(R(Left_Op), R(Right_Op));
	    break;

	  case Rational:
	    R(ans) = RatSub(R(Left_Op), R(Right_Op));
	    break;

	  case Real:
	    R(ans) = New_Real(Real_Value(R(Left_Op))
			      - Real_Value(R(Right_Op)));
	    break;

	  case Set:
	    R(ans) = mcDiff(Set_Value(R(Left_Op)),
			    Set_Value(R(Right_Op)));
	    R(ans) = New_Set(R(ans));
	    break;
	  default:
	    BAD_ARGS("-", R(Left_Op), R(Right_Op));
	}
    } else {
	/* mixed operations */
	switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
	  case Pair(Integer, Bignum):
	  case Pair(Bignum, Integer):{
		R(Left_Op) = mpForce(R(Left_Op));
		R(Right_Op) = mpForce(R(Right_Op));
		R(ans) = mpSub(R(Left_Op), R(Right_Op));
	    } break;

	  case Pair(Bignum,Rational):
	  case Pair(Rational,Bignum):
	  case Pair(Integer,Rational):
	  case Pair(Rational,Integer):
		R(ans)     = RatSub(R(Left_Op), R(Right_Op));   
		break;


	  case Pair(Real,Rational):
	  case Pair(Rational,Real):
	  case Pair(Integer, Real):
	  case Pair(Real, Integer):
	  case Pair(Bignum, Real):
	  case Pair(Real, Bignum):
	    R(Left_Op) = mpFloat(R(Left_Op));
	    R(Right_Op) = mpFloat(R(Right_Op));
	    R(ans) = New_Real(Real_Value(R(Left_Op))
			      - Real_Value(R(Right_Op)));
	    break;

	  default:
	    BAD_ARGS("-", R(Left_Op), R(Right_Op));
	}
    }
    BIN_LEAVE;
}


void    opNegate()
{
    UNARY_ENTER;
    switch (Kind(R(Only_Op))) {
      case Integer:
	R(ans) = New_Block(Integer, 0);
	Int_Value(R(ans)) = -Int_Value(R(Only_Op));
	break;

      case Bignum:
	R(ans) = New_MP(Length(R(Only_Op)));
	mpcpy(R(ans), R(Only_Op));
	Sign(R(ans)) = -Sign(R(ans));
	break;

      case Rational:
	R(ans) = New_Block(Rational,0);
	Num(R(ans)) = New_MP(Length(Num(R(Only_Op))));
	mpcpy(Num(R(ans)), Num(R(Only_Op)));
	Sign(Num(R(ans))) = -Sign(Num(R(ans)));
	Den(R(ans)) = Den(R(Only_Op));
	break;

      case Real:
	R(ans) = New_Real(-Real_Value(R(Only_Op)));
	break;

      default:
	BAD_ARGS("-", NULL, R(Only_Op));
    }
    UNARY_LEAVE;
}



/* (a,b):*  a*b */
void    opTimes()
{
    BIN_ENTER;

    if (Kind(R(Left_Op)) == Kind(R(Right_Op))) {
	switch (Kind(R(Left_Op))) {
	  case Integer:{
		WORKING result;

		result = ((WORKING) Int_Value(R(Left_Op)))
		    * Int_Value(R(Right_Op));
		R(ans) = New_Integer((WORKING)result);
	    } break;

	  case Bignum:
	    R(ans) = mpTimes(R(Left_Op), R(Right_Op));
	    break;

	  case Rational:
	    R(ans) = RatTimes(R(Left_Op), R(Right_Op));
	    break;

	  case Real:
	    R(ans) = New_Real(Real_Value(R(Left_Op))
			      * Real_Value(R(Right_Op)));
	    break;

	  case Set:
	    R(ans) = mcInter(Set_Value(R(Left_Op)),
			     Set_Value(R(Right_Op)));
	    R(ans) = New_Set(R(ans));
	    break;
	  default:
	    BAD_ARGS("*", R(Left_Op), R(Right_Op));
	}
    } else {
	/* mixed operations */
	switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
	  case Pair(Integer, Bignum):
	  case Pair(Bignum, Integer):{
		R(Left_Op) = mpForce(R(Left_Op));
		R(Right_Op) = mpForce(R(Right_Op));
		R(ans) = mpTimes(R(Left_Op), R(Right_Op));
	    } break;

	  case Pair(Bignum,Rational):
	  case Pair(Rational,Bignum):
	  case Pair(Integer,Rational):
	  case Pair(Rational,Integer):
	    R(ans) = RatTimes(R(Left_Op), R(Right_Op));
	    break;


	  case Pair(Real,Rational):
	  case Pair(Rational,Real):
	  case Pair(Integer, Real):
	  case Pair(Real, Integer):
	  case Pair(Bignum, Real):
	  case Pair(Real, Bignum):
	    R(Left_Op) = mpFloat(R(Left_Op));
	    R(Right_Op) = mpFloat(R(Right_Op));
	    R(ans) = New_Real(Real_Value(R(Left_Op))
			      * Real_Value(R(Right_Op)));
	    break;

	  case Pair(Integer, String):
	    R(ans) = mcStr_Rep(Int_Value(R(Left_Op)),
			       R(Right_Op));
	    break;

	  case Pair(String, Integer):
	    R(ans) = mcStr_Rep(Int_Value(R(Right_Op)),
			       R(Left_Op));
	    break;

	  case Pair(Integer, Tuple):
	    R(ans) = mcRep(Int_Value(R(Left_Op)),
			   Tuple_Value(R(Right_Op)));
	    R(ans) = New_Tuple(R(ans), Tuple_Origin(R(Right_Op)));
	    break;

	  case Pair(Tuple, Integer):
	    R(ans) = mcRep(Int_Value(R(Right_Op)),
			   Tuple_Value(R(Left_Op)));
	    R(ans) = New_Tuple(R(ans), Tuple_Origin(R(Left_Op)));
	    break;

	  default:
	    BAD_ARGS("*", R(Left_Op), R(Right_Op));
	}
    }

    BIN_LEAVE;
}



/*
 * Defined only for non-zero divisors, result is always non-negative.
 * See opDiv for accommodations so that (a div b)*b + (a mod b) = a
 * and (a div -b) = -(a div b)
 */
void    opMod()
{
    C_Int   a,
            b,
	    r;

    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Integer, Integer):
	a = Int_Value(R(Left_Op));
	b = Int_Value(R(Right_Op));

	if (b < 0) {
	    b = -b;
	} else if (b == 0) {
	    BAD_ARGS("mod", R(Left_Op), R(Right_Op));
	}
	r = a % b;
	if (r < 0)
	    r = r + b;
	R(ans) = New_Integer((WORKING) r);
	break;

      case Pair(Bignum, Bignum):{
	    Reg     (a2);

	    if (Length(R(Right_Op)) == 0) {
		BAD_ARGS("mod", R(Left_Op), R(Right_Op));
	    }
	    mpDivMod(R(Left_Op), R(Right_Op), &R(a2), &R(ans));
	    /* handles negative b correctly */
	} break;

      case Pair(Integer, Bignum):
      case Pair(Bignum, Integer):{
	    Reg     (a2);

	    R(Left_Op) = mpForce(R(Left_Op));
	    R(Right_Op) = mpForce(R(Right_Op));
	    if (Length(R(Right_Op)) == 0) {
		BAD_ARGS("mod", R(Left_Op), R(Right_Op));
	    }
	    mpDivMod(R(Left_Op), R(Right_Op), &R(a2), &R(ans));
	} break;
      default:
	BAD_ARGS("mod", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}



void    opDiv()
{
    C_Int   a,
            b,
            q,
            s;

    BIN_ENTER;

    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Integer, Integer):
	a = Int_Value(R(Left_Op));
	b = Int_Value(R(Right_Op));

	if (b == 0) {
	    RT_ERROR("Divide by zero");
	} else if (b > 0) {
	    s = 1;
	} else {
	    s = -1;
	    b = -b;
	}

	q = a / b;
	/* if a mod b < 0 add |b| to make it positive,	 */
	/* therefore subtract 1 from quotient	 */
	if (a % b < 0)
	    q = q - 1;

	q = q * s;

	R(ans) = New_Integer((WORKING) q);
	break;

      case Pair(Bignum, Bignum):{
	    Reg     (a2);

	    if (Length(R(Right_Op)) == 0) {
		RT_ERROR("Divide by zero");
	    }
	    mpDivMod(R(Left_Op), R(Right_Op), &R(ans), &R(a2));
	} break;

      case Pair(Integer, Bignum):
      case Pair(Bignum, Integer):{
	    Reg     (a2);

	    R(Left_Op) = mpForce(R(Left_Op));
	    R(Right_Op) = mpForce(R(Right_Op));
	    if (Length(R(Right_Op)) == 0) {
		RT_ERROR("Divide by zero");
	    }
	    mpDivMod(R(Left_Op), R(Right_Op), &R(ans), &R(a2));
	} break;

      default:
	BAD_ARGS("div", R(Left_Op), R(Right_Op));
    }

    BIN_LEAVE;
}


/* (a,b):*  a/b */
void    opSlash()
{
    C_Real  a,
            b;

    BIN_ENTER;

    if (Kind(R(Left_Op)) == Kind(R(Right_Op))) {
	switch (Kind(R(Left_Op))) {
	  case Rational:
	    R(ans) = RatDiv(R(Left_Op), R(Right_Op));
	    BIN_LEAVE;

	  case Integer:
	  case Bignum:
	    if( do_rational ) {
	        if( Kind(R(Right_Op))==Integer 
	            && Int_Value(R(Right_Op))==0 ) {
	            	RT_ERROR("Divide by zero");
                }
	         
		R(ans) = New_Rational(R(Left_Op), R(Right_Op));
		BIN_LEAVE;
	    } else {
		/* fall through */
	    }
	  case Real:
	    R(Left_Op) = mpFloat(R(Left_Op));
	    R(Right_Op) = mpFloat(R(Right_Op));
	    break;

	  default:
	    BAD_ARGS("/", R(Left_Op), R(Right_Op));
	}
    } else {
	/* mixed operations */
	switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {

	  case Pair(Bignum,Rational):
	  case Pair(Rational,Bignum):
	  case Pair(Integer,Rational):
	  case Pair(Rational,Integer):
	    if( Kind(R(Right_Op))==Integer 
	        && Int_Value(R(Right_Op))==0 ) {
	      RT_ERROR("Divide by zero");
            }
	         
	    R(ans) = RatDiv(R(Left_Op), R(Right_Op));
	    BIN_LEAVE;
	    break;

	  case Pair(Integer, Bignum):
	  case Pair(Bignum, Integer): 
	    if( do_rational ) {
		R(ans) = New_Rational(R(Left_Op), R(Right_Op));
		BIN_LEAVE;
	    } else {
		/* fall through */
	    }

	  case Pair(Real,Rational):
	  case Pair(Rational,Real):
	  case Pair(Integer, Real):
	  case Pair(Real, Integer):
	  case Pair(Bignum, Real):
	  case Pair(Real, Bignum):
	    R(Left_Op) = mpFloat(R(Left_Op));
	    R(Right_Op) = mpFloat(R(Right_Op));
	    break;

	  default:
	    BAD_ARGS("/", R(Left_Op), R(Right_Op));
	}
    }
    a = Real_Value(R(Left_Op));
    b = Real_Value(R(Right_Op));

    if (b == 0.0) {
	RT_ERROR("Divide by zero");
    }
    R(ans) = New_Real(a / b);

    BIN_LEAVE;
}


/* (B,Ex): B ** Ex */
void    opExp()
{
    BIN_ENTER;
    switch (Pair(Kind(R(Left_Op)), Kind(R(Right_Op)))) {
      case Pair(Rational,Integer): {
	  WORKING Ex = Int_Value(R(Right_Op));
	  Bool reciprocal = false;

	  R(ans) = BigOne;
	  if( Ex < 0 ) {
	      reciprocal = true;
	      Ex = - Ex;
	  }
	  while( Ex != 0 ) {
	      while( (Ex & 1) == 0 ) {
		  R(Left_Op) = RatTimes(R(Left_Op),R(Left_Op));
		  Ex = Ex / 2;
	      }
	      /* odd */
		  R(ans) = RatTimes(R(ans), R(Left_Op));
		  Ex = Ex - 1;
	  }
	  if( reciprocal && R(ans) != BigOne ) {
	      R(ans) = New_Rational( Den(R(ans)), Num(R(ans)) );
	  }
      } break;

      case Pair(Rational,Bignum):
	  RT_ERROR("rat ** bignum not yet available");
	  break;
	  

      case Pair(Bignum, Bignum):
      case Pair(Bignum, Integer):
      case Pair(Integer, Bignum):
      case Pair(Integer, Integer):{
	    Reg     (x);
	    Reg     (y);
	    Reg     (t);
	    Reg     (r);
	    int     s = 0;
	    Bool    reciprocal;

	    R(x) = mpForce(R(Left_Op));
	    R(y) = mpForce(R(Right_Op));


	    if (  (Sign(R(y)) < 0 && Length(R(x)) == 0)
	       || (Length(R(x)) == 0 && Length(R(y)) == 0)) {
		R(ans) = OM;
	    } else {
		reciprocal = Sign(R(y)) < 0;
		if( reciprocal ) Sign(R(y)) = 1;

		/* Left**Right == ans*(x**y) */
		R(ans) = BigOne;
		while (true) {
		    mpUDivMod(R(y), BigTwo, &R(t), &R(r));
		    mpNorm(R(r));
		    mpNorm(R(t));
		    if (s == 0) {
			if (Length(R(r)) == 0)
			    s = 1;
			else
			    s = Sign(R(x));
		    }
		    R(y) = R(t);
		    if (Length(R(r)) != 0) {
			R(ans) = mpUTimes(R(ans), R(x));
			mpNorm(R(ans));
		    }
		    if (Length(R(y)) == 0)
			break;

		    R(x) = mpUTimes(R(x), R(x));
		    mpNorm(R(x));
		}
		if (s == 0)
		    s = 1;
		Sign(R(ans)) = s;
		if( reciprocal ) {
		    if( do_rational ) {
			mpNorm(R(ans));
			R(ans) = New_Rational(BigOne, R(ans));
		    } else {
			R(ans) = New_Real( 1 / Real_Value(mpFloat(R(ans))) );
		    }
		} else {
		    R(ans) = mpNorm(R(ans));
		}
	    }
	} break;

      case Pair(Bignum,Rational):
      case Pair(Integer,Rational):
      case Pair(Real,Rational):
	  R(Right_Op) = mpFloat(R(Right_Op));
	  /*  fall through */

      case Pair(Rational,Real):

      case Pair(Integer, Real):
      case Pair(Real, Integer):
      case Pair(Bignum, Real):
      case Pair(Real, Bignum):
      case Pair(Real, Real):{
	    C_Real  B,
	            Ex;

	    R(Left_Op) = mpFloat(R(Left_Op));
	    B = Real_Value(R(Left_Op));

	    if ((B < 0 && Kind(R(Right_Op)) == Real)) {
		R(ans) = OM;
	    } else {
		R(Right_Op) = mpFloat(R(Right_Op));
		Ex = Real_Value(R(Right_Op));
		if (B == 0 && Ex <= 0) {
		    R(ans) = OM;
		} else {
		    R(ans) = New_Real(pow(B, Ex));
		}
	    }
	} break;

      default:
	BAD_ARGS("**", R(Left_Op), R(Right_Op));
    }
    BIN_LEAVE;
}


IPtr RatAdd(x,y) IPtr x,y; {
    ENTER;
    Reg(ans);
    Reg(top1); Reg(top2); Reg(bot);
    Reg(Left_Op); Reg(Right_Op);
    R(Left_Op) = RatForce(x);
    R(Right_Op)= RatForce(y);
    R(top1) = mpTimes(Num(R(Left_Op)), Den(R(Right_Op)));
        R(top1)  = mpForce(R(top1));
    R(top2) = mpTimes(Num(R(Right_Op)), Den(R(Left_Op)));
        R(top2)  = mpForce(R(top2));
    R(top1) = mpAdd(R(top1), R(top2));
    
    R(bot)  = mpTimes(Den(R(Left_Op)), Den(R(Right_Op)));
    
    R(ans) =  New_Rational(R(top1), R(bot));
    LEAVE R(ans);
}

IPtr RatSub(x,y) IPtr x,y; {
    ENTER;
    Reg(ans);
    Reg(top1); Reg(top2); Reg(bot);
    Reg(Left_Op); Reg(Right_Op);
    R(Left_Op) = RatForce(x);
    R(Right_Op)= RatForce(y);
    R(top1) = mpTimes(Num(R(Left_Op)), Den(R(Right_Op)));
        R(top1)  = mpForce(R(top1));
    R(top2) = mpTimes(Num(R(Right_Op)), Den(R(Left_Op)));
        R(top2)  = mpForce(R(top2));
    R(top1) = mpSub(R(top1), R(top2));
    R(bot)  = mpTimes(Den(R(Left_Op)), Den(R(Right_Op)));
    
    R(ans) =  New_Rational(R(top1), R(bot));
    LEAVE R(ans);
}

IPtr RatTimes(x,y) IPtr x,y; {
    ENTER;
    Reg(ans);
    Reg(top1); Reg(bot);
    Reg(Left_Op); Reg(Right_Op);
    R(Left_Op) = RatForce(x);
    R(Right_Op)= RatForce(y);
    R(top1) = mpTimes(Num(R(Left_Op)), Num(R(Right_Op)));
    R(bot)  = mpTimes(Den(R(Left_Op)), Den(R(Right_Op)));
    
    R(ans) =  New_Rational(R(top1), R(bot));
    LEAVE R(ans);
}


IPtr RatDiv(x,y) IPtr x,y; {
    ENTER;
    Reg(ans);
    Reg(top1); Reg(bot); 
    Reg(Left_Op); Reg(Right_Op);

    R(Left_Op) = RatForce(x);
    R(Right_Op)= RatForce(y);
    R(top1) = mpTimes(Num(R(Left_Op)), Den(R(Right_Op)));
    R(bot) = mpTimes(Num(R(Right_Op)), Den(R(Left_Op)));
    
    R(ans) = New_Rational(R(top1), R(bot));
    LEAVE R(ans);
}

IPtr RatForce(x) IPtr x; {
    ENTER;
    Reg(ans);
    switch(Kind(x)) {
      case Rational: R(ans) = x; break;
      case Integer: case Bignum:
	R(ans)  = New_Block(Rational,0);
	Num(R(ans)) = mpForce(x);
	Den(R(ans)) = BigOne;
	break;
      default:
	assert(0);
    }
    LEAVE R(ans);
}

