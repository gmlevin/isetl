/* $Header: predef.h,v 3.2 90/04/25 16:25:08 gary Locked $ */
extern void Init_Pd();		       /* Must be called before any
				        * predefined functions are invoked */

#ifdef Screen
#  ifdef TURBOC
   extern IPtr pdVideo();
#  endif
#endif

#ifdef GRAPHICS
extern IPtr pdClearScreen(), pdRectangle(), pdWhere(), pdXtoYratio();
extern IPtr pdRes();
extern IPtr pdTitleW();
extern IPtr pdGraphics();
extern IPtr pdScale();
extern IPtr pdMove();
extern IPtr pdDraw();
extern IPtr pdTextout();
extern IPtr pdGetCoord();
extern IPtr pdErasable();
extern IPtr pdThickLine();
extern IPtr pdAddObj();
extern IPtr pdDelObj();
extern IPtr pdNewObj();
extern IPtr pdCharMult();
extern IPtr pdSaveGraph();
extern IPtr pdLoadGraph();
extern IPtr pdPoint();
extern IPtr pdDelay();
#endif

extern IPtr pdAsString();
extern IPtr pdSys();
extern IPtr pdLo(), pdHi(), pdOrigin();
extern IPtr pdPrec();
extern IPtr pdAbs();
extern IPtr pdAtom();
extern IPtr pdBool();
extern IPtr pdCeil();
extern IPtr pdChar();
extern IPtr pdDenominator();
extern IPtr pdDomain();
extern IPtr pdEven();
extern IPtr pdFix();
extern IPtr pdFloat();
extern IPtr pdFloor();
extern IPtr pdInt();
extern IPtr pdFile();
extern IPtr pdFont();
extern IPtr pdFunc();
extern IPtr pdMap();
extern IPtr pdNumber();
extern IPtr pdMax();
extern IPtr pdMaxLine();
extern IPtr pdMin();
extern IPtr pdNpow();
extern IPtr pdNumerator();
extern IPtr pdOdd();
extern IPtr pdOrd();
extern IPtr pdPow();
extern IPtr pdRandom();
extern IPtr pdRange();
extern IPtr pdOm();
extern IPtr pdDefined();
extern IPtr pdReal();
extern IPtr pdRational();
extern IPtr pdSet();
extern IPtr pdSign();
extern IPtr pdStr();
extern IPtr pdTuple();
extern IPtr pdOpenr();
extern IPtr pdOpenw();
extern IPtr pdOpena();
extern IPtr pdClose();
extern IPtr pdEof();
extern IPtr pdExp();
extern IPtr pdLn();
extern IPtr pdSqrt();
extern IPtr pdSeed();		       /* vogl 2/23/87 */
extern IPtr pdLog();		       /* ahd 2/10/87 */
extern IPtr pdSin();		       /* vogl 2/25/87 */
extern IPtr pdCos();		       /* vogl 2/25/87 */
extern IPtr pdTan();
extern IPtr pdASin();
extern IPtr pdACos();
extern IPtr pdATan();
extern IPtr pdSec();	
extern IPtr pdCsc();	
extern IPtr pdCot();
extern IPtr pdASec();
extern IPtr pdACsc();
extern IPtr pdACot();

extern IPtr pdSinh();		       /* Kletnieks 02/28/87 */
extern IPtr pdCosh();		       /* Kletnieks 02/28/87 */
extern IPtr pdTanh();		       /* Kletnieks 02/28/87 */
extern IPtr pdAsinh();		       /* Kletnieks 02/28/87 */
extern IPtr pdAcosh();		       /* Kletnieks 02/28/87 */
extern IPtr pdAtanh();		       /* Kletnieks 02/28/87 */

#define Pd_Arg(id)	\
	id = Info(args); args = Next(args)

#define Pd(id)		IPtr id(args) IPtr args;

#define Pd0(id)		IPtr id()
#define Pd1(id)		IPtr id(x) IPtr x;
#define Pd2(id)		IPtr id(a,b) IPtr a,b;
