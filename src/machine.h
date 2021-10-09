#ifdef THINK_C
#define Mac
#endif
/* Defined systems

 Vax		Vax under BSD Unix
 Ultrix		On Vax
 Sun
 GCC
 Gould
 Mac		Using THINK C
 TURBOC		MSDOS
 ATT
 SYS5
 VMS
*/


/* $HEADER: machine.h,v 2.1 89/09/08 11:51:58 gary Locked $ */
/*
  The following defines can be used to control the form of ISETL
  generated.  Some machines are predefined below, using these defines.
  If your machine is not among them, use this chart to help choose
  the features that fit.

  Either define the machine type using a global define, or define it at
  the top of this file.

	Define		If
	======		==
	Assert		<assert.h> exists
	Double		use "double" instead of "float"
	Exec_Path       argv[0] contains path to executable.
	Get_Env		getenv("HOME") will return path to home directory.
	Index		the "standard" function "index" is not defined
	InitCast	casts in initialization are legal
        LShift		Logical Shift -- sign does not propogate
                        define if your machine does a logical shift right
	Prototype	Define if your compile understands ANSI prototypes
	Rand		"rand" exists and returns 0..2**15-1
	Random		"random" exists and returns 0..2**31-1
	BigRand		rand exists and returns 0..2**31-1
	Raw		Need to set stdin to raw mode.
	  CRmode	Define CRmode if your system uses crmode()
                        rather than cbreak() which is more standard.
	Screen		Use full screen editor in place of ile.
			Currently only implemented for Turbo-C and Mac.
	Signals		<signals.h> is present
	StringH		<string.h> is present (SYSTEM5)
	StringsH	<strings.h> is present (BSD)
        Volatile        compiler has the keyword "volatile"
	Void		compiler does not know about "void",
			or doesn't allow "void (*p)()"
	GOOD_EXIT	0  -- Redefine if exit code should be different
	BAD_EXIT	1  -- Redefine if exit code should be different

	TORTURE         GC before EVERY New_Block  (debugging only)

	The system assumes that all basic types have sizes that are
	powers of 2.  If this is not true, you need to define X_Mult
	here to be the first power of 2 greater than the size.	See
	common.h for the X_Mult definitions, and Mac below for an
	example.

	Gen_Mult should be the smallest power of two that is a good
	boundary for all data types in use.  It will default to
	Real_Mult on the assumption that Reals are the hardest data to
	align.  The smaller Gen_Mult is, the less wasted space per
	node.  This assumes that all structures are permitted to begin
	on a Gen_Mult boundary.
*/

#ifdef NewSun
#define Get_Env
#define Assert
#define Double
#define Random
#define Signals
#define StringsH
#define InitCast
#define SYS5
#endif


#ifdef Ubuntu
#define Get_Env
#define Assert
#define Double
#undef Rand
#define Random
#define Signals
#define StringsH
#define InitCast
#define SYS5
#define Raw
#endif


#ifdef Vax
#define Get_Env
#define Assert
#define Double
#define Random
#define Signals
#define StringsH
#define Void
#define InitCast
#define Raw
#endif

#ifdef Ultrix
#define CRmode
#define Get_Env
#define Assert
#define Double
#define Random
#define Signals
#define StringsH
#define Void
#define InitCast
#define Raw
#endif


#ifdef Sun
#define Double
#define Get_Env
#define Assert
#define Random
#define Signals
#define StringsH
#define Void
#define InitCast
#define Raw
#endif


#ifdef GCC
#define Get_Env
#define Assert
#define Double
#define Random
#define Signals
#define StringsH
#define Prototype
#define InitCast
#define Raw
#endif


#ifdef Gould
#define Get_Env
#define Assert
#define Random
#define Signals
#define StringsH
#define Void
#define InitCast
#define Double
#define Raw
#endif


#ifdef Mac
#define GRAPHICS
#define Screen
#define Prototype
#define StringH
#define Double
#define Real_Mult 16
/* Reals are 10 bytes in LightSpeed-C */
#define InitCast
#define Index
#define Rand
#define Signals
#define Gen_Mult 2
#ifndef Screen
#   define Raw
#endif
/* Undefine Signals in LSC 2 */
#endif


#ifdef TURBOC
#define Volatile
#define GRAPHICS
#define Screen
#define Exec_Path
#define Assert
#define Double
#define Rand
#define StringH
#define Index
#define Signals
#define Prototype
#define Gen_Mult      2
#endif


#ifdef ATT
#define LShift
#define SYS5
#endif


#ifdef SYS5
#define Assert
#define Rand
#define StringH
#define Index
#define Signals
#define Raw
#endif


#ifdef VMS
#define Get_Env
#define BigRand
#define Index
#define Signals
#define Void
#define GOOD_EXIT 1
#define BAD_EXIT  0
#endif

#ifndef GOOD_EXIT
#define GOOD_EXIT 0
#endif

#ifndef BAD_EXIT
#define BAD_EXIT 1
#endif

#ifdef Volatile
#define VOLATILE volatile
#else
#define VOLATILE
#endif
