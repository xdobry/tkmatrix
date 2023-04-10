/*
// winsupport.c
//
// Author: Artur Trzewik
// Date: 02.2000
//
// winsupport implement interfaces to
// MSWindows Help (winhelp string) and Registry DB (winregistry load|save key array)
//
*/
#define DBUG 1
#undef DBUG
#ifdef DBUG
 #include <stdio.h>
#endif

#include <tk.h>
#include <windows.h>

#define WINSUPPORT_VERSION "0.1"

#define help_command_count 19
static char *help_command[help_command_count]=
             {"HELP_COMMAND","HELP_CONTENTS","HELP_CONTEXT","HELP_CONTEXTPOPUP",
	      "HELP_FORCEFILE","HELP_HELPONHELP","HELP_INDEX",
	      "HELP_KEY","HELP_PARTIALKEY","HELP_QUIT",
	      "HELP_SETCONTENTS","HELP_SETINDEX","HELP_CONTEXTMENU",
	      "HELP_FINDER"};
static UINT help_command_index[help_command_count]=
             {(0x102L),(0x3L),(0x1L),(0x8L),
	      (0x9L),(0x4L),(0x3L),
	      (0x101L),(0x105L),(0x2L),
	      (0x5L),(0x5L),(0xa),
	      (0xb)};

static int Tk_WinhelpCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[]; 
{
    Tk_Window tkwin = (Tk_Window) clientData;
    int t,hcommand,index,length;
    char puf[50];
    if (objc < 2 || objc>4) {
      Tcl_WrongNumArgs(interp, 1, objv, "Helppath ?HELPCOMMAND INDEX?");
        return TCL_ERROR;
    }
    if (objc==2) {
      WinHelp(Tk_GetHWND(Tk_WindowId(tkwin)),Tcl_GetStringFromObj(objv[1],&length),HELP_FINDER,0L);
    }  else {
      if (Tcl_GetIndexFromObj(interp, objv[2], help_command, "HelpCommand", 0, &t)
	  != TCL_OK) {
        return TCL_ERROR;
      }
      hcommand=help_command_index[t];
      if (hcommand==0x102 || hcommand==0x101 || hcommand==0x105) {
	WinHelp(Tk_GetHWND(Tk_WindowId(tkwin)),Tcl_GetStringFromObj(objv[1],&length),
		hcommand,Tcl_GetStringFromObj(objv[3],&length));
      } else {
		if (Tcl_GetIntFromObj(interp,objv[3],&index)==TCL_ERROR) {
	  	return TCL_ERROR;
	}
	WinHelp(Tk_GetHWND(Tk_WindowId(tkwin)),Tcl_GetStringFromObj(objv[1],&length),hcommand,index);
      }

    }
    return TCL_OK;
}
/*
static int Tk_WinregistryCmd(clientData, interp, argc, argv)
     ClientData clientData;

     Tcl_Interp *interp;
     int argc;
     Tchar **argv;
{
  Tk_Window tkwin = (Tk_Window) clientData;
}
*/
int Winsupport_Init(Tcl_Interp *interp) {

    Tcl_CreateObjCommand (interp, "winhelp",
                       Tk_WinhelpCmd,
                       (ClientData) Tk_MainWindow (interp),
                       (Tcl_CmdDeleteProc*) NULL);
/*    Tcl_CreateCommand (interp, "winregistry",
                       Tk_WinregistryCmd,
                       (ClientData) Tk_MainWindow (interp),
                       (Tcl_CmdDeleteProc*) NULL);
*/

    /* winsupport as available package */
    Tcl_PkgProvide (interp, "Winsupport", WINSUPPORT_VERSION);
    return TCL_OK;
}



