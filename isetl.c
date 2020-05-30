#include "common.h"

#ifdef Mac
#include <console.h>
#endif

void    main(argc, argv)
    int     argc;
    char   *argv[];

{

#ifdef Mac
#ifdef Screen
    startup();
#else
    argc = ccommand(&argv);
#endif
#endif
    _main(argc, argv);

    do_exit(GOOD_EXIT);
}
