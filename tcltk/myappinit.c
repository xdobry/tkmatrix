/* 
 * tkAppInit.c --
 *
 *	Provides a default version of the Tcl_AppInit procedure for
 *	use in wish and similar Tk-based applications.
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tkAppInit.c 1.22 96/05/29 09:47:08
 */

#include "tk.h"

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

extern int matherr();
int *tclDummyMathPtr = (int *) matherr;

#ifdef TK_TEST
EXTERN int		Tktest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TK_TEST */

/*  my Tk_Main prozedur */
static Tcl_Interp *interp;
static Tcl_DString command;
static Tcl_DString line;
static int tty;
void
Tk_myMain(argc, argv, appInitProc)
    int argc;				/* Number of arguments. */
    char **argv;			/* Array of argument strings. */
    Tcl_AppInitProc *appInitProc;	/* Application-specific initialization
					 * procedure to call after most
					 * initialization but before starting
					 * to execute commands. */
{
    char *args, *fileName;
    char buf[20];
    int code;
    size_t length;
    char *executable,*progpath;
    int pargc;
    char **pargv;
    static char *firstfile="tkmatrix.tcl";
    Tcl_DString dstring;
    Tcl_Channel inChannel, outChannel;

    Tcl_FindExecutable(argv[0]);
    /*
    executable=Tcl_GetNameOfExecutable();
    */
    interp = Tcl_CreateInterp();


    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".
     */

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", argv[0], TCL_GLOBAL_ONLY);

    /*
     * Set the "tcl_interactive" variable.
     */

    /*^
     * For now, under Windows, we assume we are not running as a console mode
     * app, so we need to use the GUI console.  In order to enable this, we
     * always claim to be running on a tty.  This probably isn't the right
     * way to do it.
     */

    tty = 0;

    /* not interactive */
    Tcl_SetVar(interp, "tcl_interactive","0", TCL_GLOBAL_ONLY);

    /*
     * Invoke application-specific initialization.
     */

    if ((*appInitProc)(interp) != TCL_OK) {
	TkpDisplayWarning(interp->result, "Application initialization failed");
    }

    /*
     * Invoke the script specified on the command line, if any.
     */
     /*
     Tcl_SplitPath(executable,&pargc,&pargv);
     pargv[pargc-1]=executable;
     Tcl_DStringInit(&dstring);
     Tcl_JoinPath(pargv,pargc,&dstring);
     */
     code = Tcl_EvalFile(interp, firstfile);
    /* Tcl_DStringFree(&dstring);
    Tcl_Free(pargv);*/
    if (code != TCL_OK) {
      /*
       * The following statement guarantees that the errorInfo
       * variable is set properly.
       */

      Tcl_AddErrorInfo(interp, "");
      TkpDisplayWarning(Tcl_GetVar(interp, "errorInfo",
				   TCL_GLOBAL_ONLY), "Can´t find tkmatrix.tcl!");
      Tcl_DeleteInterp(interp);
      Tcl_Exit(1);
    }
    tty = 0;

    /*
    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
    if (outChannel) {
	Tcl_Flush(outChannel);
    }
    Tcl_DStringInit(&command);
    Tcl_DStringInit(&line);
    Tcl_ResetResult(interp);
    */
    /*
     * Loop infinitely, waiting for commands to execute.  When there
     * are no windows left, Tk_MainLoop returns and we exit.
     */

    Tk_MainLoop();
    Tcl_DeleteInterp(interp);
    Tcl_Exit(0);
}
/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Results:
 *	None: Tk_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

int
main(argc, argv)
    int argc;			/* Number of command-line arguments. */
    char **argv;		/* Values of command-line arguments. */
{
    Tk_myMain(argc, argv, Tcl_AppInit);
    return 0;			/* Needed only to prevent compiler warning. */
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */
extern int Matrixtcl_Init(Tcl_Interp *interp);
extern int Float_Init(Tcl_Interp *interp);
extern int Winsupport_Init(Tcl_Interp *interp);

int
Tcl_AppInit(interp)
    Tcl_Interp *interp;		/* Interpreter for application. */
{
    Tcl_SetVar(interp, "tcl_library","tcl8.0", TCL_GLOBAL_ONLY);

    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    if (Tk_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_SafeInit);
/* Init of own matrix procedures and float widget */
    Matrixtcl_Init(interp);
    Float_Init(interp);
#ifndef __WIN32__
    /* Float_Init(interp); */
#else
	 Winsupport_Init(interp);
    /*
    Registry_Init(interp);
    */
#endif

#ifdef TK_TEST
    if (Tktest_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tktest", Tktest_Init,
            (Tcl_PackageInitProc *) NULL);
#endif /* TK_TEST */


    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */
    Tcl_SetVar(interp, "DynLinked","0", TCL_GLOBAL_ONLY);

    /* #ifdef __WIN32__
    Tcl_SetVar(interp, "floatwidget","0", TCL_GLOBAL_ONLY);
#endif */
    /* Tcl_SetVar(interp, "tcl_rcFileName", "tkmatrix.tcl", TCL_GLOBAL_ONLY); */
    return TCL_OK;
}
