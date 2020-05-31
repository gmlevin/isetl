#include "common.h"
#include "sort.h"
#include "micro.h"
#include "manager.h"
#define unit  IPtr

static void heapify(PTR ADDR X INT X INT);
static Bool  lt(PTR X PTR);
static Bool lex(PTR X PTR);
static void sort(PTR ADDR X INT);

static Bool lt(a,b) IPtr a,b; {
    if( is_num(a) && is_num(b) ) return mcLt(a,b);
    if( is_num(a) ) return true;
    if( is_num(b) ) return false;
    if( Kind(a)!=Kind(b) ) return Kind(a)<Kind(b);
    switch( Kind(a) ) {
      case Atom:    return Atom_Value(a) < Atom_Value(b);
      case Boolean: return Bool_Value(a) < Bool_Value(b);
      case SArray:  return Array_Len(a)<Array_Len(b) ||
                         (Array_Len(a)==Array_Len(b) && lex(a,b));
      case TArray:  return lex(a,b);
      case Closure: return Cl_Cnt(a) < Cl_Cnt(b);
      case Predef:  return strcmp(Pd_name(a),Pd_name(b))<0;
      case String:  return strcmp(Str_Value(a),Str_Value(b))<0;
      default: assert(0);
    }
}

static Bool lex(a,b) IPtr a, b; {
    int i, len;
    len = min(Array_Len(a),Array_Len(b));
    for( i=0; i<len; i++ ) {
	if( lt(Array_Data(a)[i],Array_Data(b)[i]) ) return true;
	if( lt(Array_Data(b)[i],Array_Data(a)[i]) ) return false;
    }
    return Array_Len(a) < Array_Len(b);
}

static void heapify(base,first,stop)
  unit *base;
  int first, stop; {
      int child, max_idx;
      unit *max_at, *child_at, *first_at;
      first_at = &base[first];
      for(;;) {
	  child = first*2;
	  child_at = &base[child];
	  max_at = first_at;

	  if( child < stop && lt(*max_at, *child_at) ) {
	      max_at = child_at;
	      max_idx = child;
	  }
	  child++;
	  child_at++;
	  if( child < stop && lt(*max_at, *child_at) ) {
	      max_at = child_at;
	      max_idx = child;
	  }

	  if( max_at == first_at ) return;
	  {unit t; t = *first_at; *first_at = *max_at; *max_at = t;}
	  first_at = max_at;
	  first    = max_idx;
      }
  }

static void sort(base, count)
  unit *base;
  int count;{
      int i;
      for( i=count/2; i >= 0; i-- ) {
	  heapify(base,i,count);
      }
      for( i=count-1; i > 0; i-- ) {
	  {unit t; t = base[0]; base[0] = base[i]; base[i] = t;}
	  heapify(base,0,i);
      }
  }


IPtr PrintCopy(x) IPtr x; {
    ENTER;
    Reg(ans);
    Reg(terms);
    Reg(t);
    int i;
    Bool was_set;

    was_set = Kind(x)==Set;
    switch( Kind(x) ) {
      case Set:
	R(ans) = New_Block(SArray, (int)mcLen(x));
	x = Set_Value(x);
	break;
      case Tuple:
	R(ans) = New_Block(TArray, (int)mcLen(x));
	x = Tuple_Value(x);
	break;
      default:
	LEAVE x;
    }
    
    R(terms) = mcStart(x);
    i = 0;
    while( mcNext(&R(t), &R(terms), true) ) {
	Array_Data(R(ans))[i] = PrintCopy(R(t));
	i++;
    }

    if( was_set ) sort(Array_Data(R(ans)), Array_Len(R(ans)));

    LEAVE R(ans);
}
