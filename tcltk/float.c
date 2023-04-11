/*
// float.c
//
// Author: Artur Trzewik
// Date: 04.1999
//
// a tk widget for displaying float number
// based on tkButton.c from tk8.03 source
// for use as library
*/
#define DBUG 1
#undef DBUG
#ifdef DBUG
 #include <stdio.h>
#endif

#include <tk.h>

#define FLOAT_RAUMA     2
#define FLOAT_RAUMB     3

#define BLACK           "Black"
#define WHITE           "White"
#define NORMAL_BG       "#d9d9d9"
#define DEF_FLOAT_FONT                 "Helvetica -12 bold"
#define DEF_FLOAT_FONTBIG              "Helvetica -16 bold"
#define DEF_FLOAT_RELIEF               "solid"
#define DEF_FLOAT_WIDTH                "0"
#define DEF_FLOAT_HEIGHT                "0" 
#define DEF_FLOAT_PADX                 "3m"
#define DEF_FLOAT_PADY                 "1m"
#define DEF_FLOAT_BG_COLOR             NORMAL_BG
#define DEF_FLOAT_BG_MONO              WHITE
#define DEF_FLOAT_FG                   BLACK
#define DEF_FLOAT_TEXT                 ""
#define DEF_FLOAT_ANCHOR               "w"  
#define DEF_FLOAT_BORDER_WIDTH         "1"

#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct {
    Tk_Window tkwin;            /* Window that embodies the button.  NULL
                                 * means that the window has been destroyed. */
    Display *display;           /* Display containing widget.  Needed to
                                 * free up resources after tkwin is gone. */
    Tcl_Interp *interp;         /* Interpreter associated with button. */

    Tcl_Command widgetCmd; 
    /*
     * Information about what's in the button.
     */
    char *text,*gzahl,*zaehler,*nenner;   /* Text to display in button (malloc'ed)
                                           * or NULL. */
    int floattype,w_gzahl,w_zaehler,w_nenner;
        /* beim w_gzahl handelt es sich um Breite im pixel */
    int len_gzahl,len_zaehler,len_nenner;
    int font_height,fontbig_height;
       /* floattype 0- bad text 1- one number 2- a float 3- float with number
        *  w_* width of strings in pixel
        */

    /*
     * Information used when displaying widget:
     */

    Tk_3DBorder normalBorder;   /* Structure used to draw 3-D
                                 * border and background when window
                                 * isn't active.  NULL means no such
                                 * border exists. */
    int borderWidth;            /* Width of border. */
    int relief;                 /* 3-d effect: TK_RELIEF_RAISED, etc. */

    int inset;                  /* Total width of all borders, including
                                 * traversal highlight and 3-D border.
                                 * Indicates how much interior stuff must
                                 * be offset from outside edges to leave
                                 * room for borders. */
    Tk_Font tkfont;             /* Information about text font, or NULL. */
    Tk_Font tkfontbig;          /* Information about text font big, or NULL. */

    XColor *normalFg;           /* Foreground color in normal mode. */
    GC normalTextGC;            /* GC for drawing text in normal mode.  Also
                                 * used to copy from off-screen pixmap onto
                                 * screen. */
    GC bigTextGC;               /* GC for Float Big Number */

    char *widthString;          /* Value of -width option.  Malloc'ed. */
    char *heightString;         /* Value of -height option.  Malloc'ed. */
    int padX, padY;             /* Extra space around text (pixels to leave
                                 * on each side).  Ignored for bitmaps and
                                 * images. */
    Tk_Anchor anchor;           /* Where text/bitmap should be displayed
                                 * inside button region. */
    int textWidth;              /* Width needed to display text as requested,
                                 * in pixels. */
    int textHeight;             /* Height needed to display text as requested,
                                 * in pixels. */
    int updatePending;

} TkFloat;
 
Tk_ConfigSpec tkFloatConfigSpecs[] = {
    {TK_CONFIG_ANCHOR, "-anchor", "anchor", "Anchor",
	DEF_FLOAT_ANCHOR, Tk_Offset(TkFloat, anchor), 0, 
        (Tk_CustomOption *) NULL },
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_FLOAT_BG_COLOR, Tk_Offset(TkFloat, normalBorder),
	TK_CONFIG_COLOR_ONLY, (Tk_CustomOption *) NULL },
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_FLOAT_BG_MONO, Tk_Offset(TkFloat, normalBorder),
        TK_CONFIG_MONO_ONLY, (Tk_CustomOption *) NULL },
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0, (Tk_CustomOption *) NULL},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0, (Tk_CustomOption *) NULL},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_FLOAT_BORDER_WIDTH, Tk_Offset(TkFloat, borderWidth),0, 
        (Tk_CustomOption *) NULL},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0, (Tk_CustomOption *) NULL},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_FLOAT_FONT, Tk_Offset(TkFloat, tkfont),
	0, (Tk_CustomOption *) NULL},
    {TK_CONFIG_FONT, "-fontbig", "fontbig", "Fontbig",
	DEF_FLOAT_FONTBIG, Tk_Offset(TkFloat, tkfontbig),
	0, (Tk_CustomOption *) NULL},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
       DEF_FLOAT_FG, Tk_Offset(TkFloat, normalFg), 0, 
       (Tk_CustomOption *) NULL },
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_FLOAT_PADX, Tk_Offset(TkFloat, padX), 0,
        (Tk_CustomOption *) NULL},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_FLOAT_PADY, Tk_Offset(TkFloat, padY), 0,
        (Tk_CustomOption *) NULL},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_FLOAT_RELIEF, Tk_Offset(TkFloat, relief), 0,
        (Tk_CustomOption *) NULL},
    {TK_CONFIG_STRING, "-text", "text", "Text",
	DEF_FLOAT_TEXT, Tk_Offset(TkFloat, text), 0, (Tk_CustomOption *) NULL},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0, (Tk_CustomOption *) NULL}
};
/* Function Declaration */
static void FloatEventProc(ClientData clientData,XEvent *eventPtr);
static int FloatConfigure(Tcl_Interp *interp,TkFloat *butPtr,int argc,char **argv,int flags);
static void FloatDisplay(ClientData clientData);
static int FloatWidgetCmd(ClientData clientData,Tcl_Interp *interp,int argc,char **argv);
static void DestroyFloat(char *clientData);
static void FloatCmdDeletedProc(ClientData clientData);
static void TkComputeAnchor(Tk_Anchor anchor,Tk_Window tkwin,int padX,int padY,int innerWidth,int innerHeight,int *xPtr,int *yPtr);
/*
 *--------------------------------------------------------------
 *
 * FloatCreate --
 *
 *	This procedure does all the real work of implementing the
 *	"float" Tcl
 *	commands.
 *
 *--------------------------------------------------------------
 */

static int Tk_FloatCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    TkFloat *butPtr;
    Tk_Window tkwin = (Tk_Window) clientData;
    Tk_Window new;
#ifdef DBUG
    fprintf(stderr,"entering Tk_FloatCmd\n");
#endif
    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Create the new window.
     */

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    Tk_SetClass(new, "Float");
    butPtr = (TkFloat *) ckalloc (sizeof (TkFloat));

    /*
     * Initialize the data structure for the button.
     */

    butPtr->tkwin = new;
    butPtr->display = Tk_Display(new);
    butPtr->widgetCmd = Tcl_CreateCommand(interp, Tk_PathName(butPtr->tkwin),
	    FloatWidgetCmd, (ClientData) butPtr, FloatCmdDeletedProc);
    butPtr->interp = interp;
    butPtr->text = NULL;
    butPtr->gzahl = NULL;
    butPtr->zaehler = NULL;
    butPtr->nenner = NULL;
    butPtr->normalBorder = NULL;
    butPtr->borderWidth = 0;
    butPtr->relief = TK_RELIEF_FLAT;
    butPtr->tkfont = NULL;
    butPtr->tkfontbig = NULL;
    butPtr->normalFg = NULL;
    butPtr->normalTextGC = None;
    butPtr->bigTextGC = None;
    butPtr->widthString = NULL;
    butPtr->heightString = NULL;
    butPtr->padX = 0;
    butPtr->padY = 0;
    butPtr->anchor = TK_ANCHOR_CENTER;
    butPtr->updatePending = 0;
#ifdef DBUG
    fprintf(stderr,"Tk_FloatCmd 2\n");
#endif

    Tk_CreateEventHandler(butPtr->tkwin,
	    ExposureMask|StructureNotifyMask,
	    FloatEventProc, (ClientData) butPtr);
#ifdef DBUG
    fprintf(stderr,"Tk_FloatCmd 3\n");
#endif

    if (FloatConfigure(interp, butPtr, argc - 2, argv + 2,0) != TCL_OK) {
	Tk_DestroyWindow(butPtr->tkwin);
	return TCL_ERROR;
    }
#ifdef DBUG
    fprintf(stderr,"Tk_FloatCmd 4\n");
#endif

    Tcl_SetObjResult(interp,Tcl_NewStringObj(Tk_PathName(butPtr->tkwin),-1));
    return TCL_OK;

}
/*
 *--------------------------------------------------------------
 *
 * FloatEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on float widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When the window gets deleted, internal structures get
 *	cleaned up.  When it gets exposed, it is redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
FloatEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    TkFloat *butPtr = (TkFloat *) clientData;
    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	goto redraw;
    } else if (eventPtr->type == ConfigureNotify) {
	/*
	 * Must redraw after size changes, since layout could have changed
	 * and borders will need to be redrawn.
	 */

	goto redraw;
    } else if (eventPtr->type == DestroyNotify) {
	if (butPtr->tkwin != NULL) {
	    butPtr->tkwin = NULL;
            Tcl_DeleteCommandFromToken(butPtr->interp, butPtr->widgetCmd);
	}
	if (butPtr->updatePending) {
	    Tcl_CancelIdleCall(FloatDisplay, (ClientData) butPtr);
	}
	Tk_EventuallyFree((ClientData)butPtr, DestroyFloat);
    } 
    return;

    redraw:
    if ((butPtr->tkwin != NULL) && !(butPtr->updatePending)) {
	Tcl_DoWhenIdle(FloatDisplay, (ClientData) butPtr);
	butPtr->updatePending = 1;
    }
}
/*
 *----------------------------------------------------------------------
 *
 * FloatConfigure -
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a button widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for butPtr;  old resources get freed, if there
 *	were any.  The button is redisplayed.
 *
 *----------------------------------------------------------------------
 */

static int
FloatConfigure(interp, butPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    TkFloat *butPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;
    unsigned long mask;

    int width, height, x;
    Tk_FontMetrics fm;
    int listArgc;
    char **listArgv;

    char *str_elem[3],*str_pnt;
    int str_len[3],str_flag;
#ifdef DBUG
    fprintf(stderr,"FloatConfigure");
#endif

    if (Tk_ConfigureWidget(interp, butPtr->tkwin, tkFloatConfigSpecs,
	    argc, argv, (char *) butPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    Tk_SetBackgroundFromBorder(butPtr->tkwin, butPtr->normalBorder);

    if (butPtr->padX < 0) {
	butPtr->padX = 0;
    }
    if (butPtr->padY < 0) {
	butPtr->padY = 0;
    }

    /*
     * Recompute GCs.
     */

    gcValues.font = Tk_FontId(butPtr->tkfont);
    gcValues.foreground = butPtr->normalFg->pixel;
    gcValues.background = Tk_3DBorderColor(butPtr->normalBorder)->pixel;
    
    /*
     * Note: GraphicsExpose events are disabled in normalTextGC because it's
     * used to copy stuff from an off-screen pixmap onto the screen (we know
     * that there's no problem with obscured areas).
     */

    gcValues.graphics_exposures = False;
    mask = GCForeground | GCBackground | GCFont | GCGraphicsExposures;
    newGC = Tk_GetGC(butPtr->tkwin, mask, &gcValues);

    if (butPtr->normalTextGC != None) {
	Tk_FreeGC(butPtr->display, butPtr->normalTextGC);
    }
    butPtr->normalTextGC = newGC;
    
    gcValues.font = Tk_FontId(butPtr->tkfontbig);

    mask = GCForeground | GCBackground | GCFont;
    newGC = Tk_GetGC(butPtr->tkwin, mask, &gcValues);

    if (butPtr->bigTextGC != None) {
	Tk_FreeGC(butPtr->display, butPtr->bigTextGC);
    }
    butPtr->bigTextGC = newGC;

    /*

    if (butPtr->copyGC == None) {
      butPtr->copyGC = Tk_GetGC(butPtr->tkwin, 0, &gcValues);
    }
    */
    /* Split text string to 1, 2 or 3 components */
    str_pnt=butPtr->text;
    butPtr->gzahl = NULL;
    butPtr->zaehler = NULL;
    butPtr->nenner = NULL;
    str_flag=0;
    for (x=0;x<3;x++) {
      // Einfhrende whitespace ignorieren
      while (*str_pnt && isspace(*str_pnt))
	++str_pnt;
      if (!*str_pnt) break;
      str_flag=1;
      if (*str_pnt && *str_pnt=='{') {
	// wenn { dann bis } oder \0 absuchen
	str_elem[x]=++str_pnt;
	while (*str_pnt && *str_pnt!='}')
	  ++str_pnt;
	str_len[x]=str_pnt-str_elem[x];
	if (*str_pnt) ++str_pnt;
      } else {
	str_elem[x]=str_pnt++;
	while (*str_pnt && !isspace(*str_pnt))
	  ++str_pnt;
	str_len[x]=str_pnt-str_elem[x];
      }
      if (!*str_pnt) break;
      str_flag=0;
    }
    if (str_flag) x++;
    butPtr->len_gzahl = 0;
    butPtr->len_zaehler = 0;
    butPtr->len_nenner = 0;
    switch (x) {
    case 1:
      butPtr->gzahl = str_elem[0];
      butPtr->len_gzahl = str_len[0];
      break;
    case 2:
      butPtr->zaehler = str_elem[0];
      butPtr->len_zaehler = str_len[0];
      butPtr->nenner = str_elem[1];
      butPtr->len_nenner = str_len[1];
      break;
    case 3:
      butPtr->gzahl = str_elem[0];
      butPtr->len_gzahl = str_len[0];
      butPtr->zaehler = str_elem[1];
      butPtr->len_zaehler = str_len[1];
      butPtr->nenner = str_elem[2];
      butPtr->len_nenner = str_len[2];
      break;
    }
    /*
    if (butPtr->gzahl) ckfree(butPtr->gzahl);
    if (butPtr->zaehler) ckfree(butPtr->zaehler);
    if (butPtr->nenner) ckfree(butPtr->nenner);

    butPtr->gzahl = NULL;
    butPtr->zaehler = NULL;
    butPtr->nenner = NULL;

    if (butPtr->text) {
      if (Tcl_SplitList(interp,butPtr->text,&listArgc,&listArgv)==TCL_ERROR) {
	return TCL_ERROR;
      }
      butPtr->floattype=listArgc;
      switch (listArgc) {
      case 0:
	break;
      case 1:
	butPtr->gzahl=(char *)ckalloc(strlen(listArgv[0])+1);
	strcpy(butPtr->gzahl,listArgv[0]);
	break;
      case 2:
	butPtr->zaehler=(char *)ckalloc(strlen(listArgv[0])+1);
	strcpy(butPtr->zaehler,listArgv[0]);
	butPtr->nenner=(char *)ckalloc(strlen(listArgv[1])+1);
	strcpy(butPtr->nenner,listArgv[1]);
	break;
      case 3:
	butPtr->gzahl=(char *)ckalloc(strlen(listArgv[0])+1);
	strcpy(butPtr->gzahl,listArgv[0]);	
	butPtr->zaehler=(char *)ckalloc(strlen(listArgv[1])+1);
	strcpy(butPtr->zaehler,listArgv[1]);
	butPtr->nenner=(char *)ckalloc(strlen(listArgv[2])+1);
	strcpy(butPtr->nenner,listArgv[2]);
	break;
      default:
	butPtr->floattype=0;
	Tcl_AddErrorInfo(interp, "\n    (-text is a list 1 to 3 elements)");
	free((char *) listArgv);
	return TCL_ERROR;
      }
      // fprintf(stderr,"ulamki sa %s %s %s\n",butPtr->gzahl,butPtr->zaehler,butPtr->nenner);
      Tcl_Free((char *) listArgv); 
    }
    */

    Tk_GetFontMetrics(butPtr->tkfont, &fm);
    butPtr->font_height = fm.linespace;
    Tk_GetFontMetrics(butPtr->tkfontbig, &fm);
    butPtr->fontbig_height = fm.linespace;

    butPtr->w_gzahl=0;
    butPtr->w_zaehler=0;
    butPtr->w_nenner=0;
    
    if (butPtr->gzahl) 
      butPtr->w_gzahl=Tk_TextWidth(butPtr->tkfontbig, butPtr->gzahl,
				   butPtr->len_gzahl);
    if (butPtr->zaehler) 
      butPtr->w_zaehler=Tk_TextWidth(butPtr->tkfont, butPtr->zaehler,
				     butPtr->len_zaehler);
    if (butPtr->nenner) 
      butPtr->w_nenner=Tk_TextWidth(butPtr->tkfont, butPtr->nenner,
				    butPtr->len_nenner);

    width=butPtr->w_gzahl+ max(butPtr->w_zaehler,butPtr->w_nenner) +
      ((butPtr->w_gzahl>0 && butPtr->w_zaehler>0) ? FLOAT_RAUMA: 0);
    height=max(butPtr->fontbig_height,butPtr->font_height*2+FLOAT_RAUMB);

    /*
    hight=max(butPtr->fontbig_height,
              (butPtr->zaehler) ? butPtr->font_height*2+FLOAT_RAUMB : 0);
    */

    butPtr->textWidth=width;
    butPtr->textHeight=height;

    Tk_GeometryRequest(butPtr->tkwin, 
                       (int) (width + 2*butPtr->borderWidth + 2*butPtr->padX), 
		       (int) (height + 2*butPtr->borderWidth + 2*butPtr->padY));
    Tk_SetInternalBorder(butPtr->tkwin, butPtr->borderWidth);

    /*
     * Lastly, arrange for the button to be redisplayed.
     */

    if (Tk_IsMapped(butPtr->tkwin) && !(butPtr->updatePending)) {
	Tcl_DoWhenIdle(FloatDisplay, (ClientData) butPtr);
	butPtr->updatePending = 1;
    }    
#ifdef DBUG
    fprintf(stderr,"Configured gzw=%d zw=%d nw=%d fonth=%d bigfonth=%d width=%d height=%d\n",
	    butPtr->w_gzahl,butPtr->w_zaehler,butPtr->w_nenner,
	    butPtr->font_height,butPtr->fontbig_height,width,height);
#endif

    return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * FloatDisplay --
 *
 *	This procedure is invoked to display a float widget.  It is
 *	normally invoked as an idle handler.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the button in its
 *	current mode.  The REDRAW_PENDING flag is cleared.
 *
 *----------------------------------------------------------------------
 */

static void
FloatDisplay(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register TkFloat *butPtr = (TkFloat *) clientData;
    GC gc;
    Tk_3DBorder border;
    Pixmap pixmap;
    int x = 0;			/* Initialization only needed to stop
				 * compiler warning. */
    int y, relief;
    register Tk_Window tkwin = butPtr->tkwin;
    int width, height;
    int linewidth,gx,gy,zx,zy,nx,ny,lx,ly;

    butPtr->updatePending = 0;
    if ((butPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    border = butPtr->normalBorder;
    gc = butPtr->normalTextGC;
    relief = butPtr->relief;

    /*
     * In order to avoid screen flashes, this procedure redraws
     * the button in a pixmap, then copies the pixmap to the
     * screen in a single operation.  This means that there's no
     * point in time where the on-sreen image has been cleared.
     */

    pixmap = Tk_GetPixmap(butPtr->display, Tk_WindowId(tkwin),
	    Tk_Width(tkwin), Tk_Height(tkwin), Tk_Depth(tkwin));
    Tk_Fill3DRectangle(tkwin, pixmap, border, 0, 0, Tk_Width(tkwin),
	    Tk_Height(tkwin), butPtr->borderWidth , relief );

    /*
     * Display text for float widget.
     */
    width=butPtr->textWidth;
    height=butPtr->textHeight;


    TkComputeAnchor(butPtr->anchor, tkwin, butPtr->padX, butPtr->padY,
                    width, height, &x, &y);
    
    gx=0;
    lx=0;
    if (butPtr->gzahl) {
      gy=max(0,(butPtr->font_height*2+FLOAT_RAUMB-butPtr->fontbig_height)/2) + butPtr->fontbig_height ;
      lx+=FLOAT_RAUMB+butPtr->w_gzahl;
      Tk_DrawChars(butPtr->display, pixmap, butPtr->bigTextGC, 
		   butPtr->tkfontbig,
		   butPtr->gzahl, butPtr->len_gzahl, gx+x, gy+y);
    }
    if (butPtr->zaehler) {
      linewidth=max(butPtr->w_zaehler,butPtr->w_nenner);
      zx=lx+max(0,(linewidth-butPtr->w_zaehler)/2);
      zy= max( 0,(butPtr->fontbig_height-butPtr->font_height*2-FLOAT_RAUMB)/2) + butPtr->font_height;
      nx=lx+max(0,(linewidth-butPtr->w_nenner)/2);
      ly=zy+FLOAT_RAUMB/2;
      ny=zy+butPtr->font_height+FLOAT_RAUMB;
      Tk_DrawChars(butPtr->display, pixmap, gc, butPtr->tkfont,
                    butPtr->zaehler, butPtr->len_zaehler, zx+x, zy+y);
      Tk_DrawChars(butPtr->display, pixmap, gc, butPtr->tkfont,
                    butPtr->nenner, butPtr->len_nenner, nx+x, ny+y);
      /* Tk Xlib primitive */
      XDrawLine(butPtr->display, pixmap, gc, lx+x, ly+y, lx+linewidth+x, ly+y);
    }
#ifdef DBUG
    fprintf(stderr,"Displayed gy=%d gx=%d zy=%d zx=%d ny=%d nx=%d x=%d y=%d ly=%d lx=%d\n",
	    gy,gx,zy,zx,ny,nx,x,y,ly,lx);
#endif
    
    XCopyArea(butPtr->display, pixmap, Tk_WindowId(tkwin),
	    gc , 0, 0, (unsigned) Tk_Width(tkwin),
	    (unsigned) Tk_Height(tkwin), 0, 0);
    Tk_FreePixmap(butPtr->display, pixmap);
}
/*
 *--------------------------------------------------------------
 *
 * FloatWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a widget managed by this module.
 *	float widget have only configure command.
 *
 * Results:
 *	A standard Tcl result.
 *
 *--------------------------------------------------------------
 */

static int
FloatWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about button widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register TkFloat *butPtr = (TkFloat *) clientData;
    int result = TCL_OK;

    if (argc < 2) {
	Tcl_SetObjResult(interp,Tcl_ObjPrintf("wrong # args: should be \"%.50s option ?arg arg ...?\"",
		argv[0]));
	return TCL_ERROR;
    }
    Tcl_Preserve((ClientData) butPtr);

    if (strcmp(argv[1], "configure") == 0) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, butPtr->tkwin,
		    tkFloatConfigSpecs, (char *) butPtr, (char *) NULL,0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, butPtr->tkwin,
		    tkFloatConfigSpecs, (char *) butPtr, argv[2],0);
	} else {
	    result = FloatConfigure(interp, butPtr, argc-2, argv+2,
				     TK_CONFIG_ARGV_ONLY);
	}
    } else {
	Tcl_SetObjResult(interp,Tcl_ObjPrintf("bad option \"%.50s\": must be configure", argv[1]));
	goto error;
    }
    Tcl_Release((ClientData) butPtr);
    return result;

    error:
    Tcl_Release((ClientData) butPtr);
    return TCL_ERROR;
}
/*
 *----------------------------------------------------------------------
 *
 * DestroyFloat --
 *
 *	This procedure is invoked by Tcl_EventuallyFree or Tcl_Release
 *	to clean up the internal structure of a button at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the widget is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyFloat(char *clientData)
{
    /*
     * Free up all the stuff that requires special handling, then
     * let Tk_FreeOptions handle all the standard option-related
     * stuff.
     */
  TkFloat *butPtr=(TkFloat *)clientData;
    if (butPtr->normalTextGC != None) {
	Tk_FreeGC(butPtr->display, butPtr->normalTextGC);
    }
    if (butPtr->bigTextGC != None) {
	Tk_FreeGC(butPtr->display, butPtr->bigTextGC);
    }
    /*
    if (butPtr->gzahl) ckfree(butPtr->gzahl);
    if (butPtr->zaehler) ckfree(butPtr->zaehler);
    if (butPtr->nenner) ckfree(butPtr->nenner);
    */
    Tk_FreeOptions(tkFloatConfigSpecs, (char *) butPtr, butPtr->display,0);
    ckfree((char *)butPtr);
}

/*----------------------------------------------------------------------
 *
 * ButtonCmdDeletedProc --
 *
 *	This procedure is invoked when a widget command is deleted.  If
 *	the widget isn't already in the process of being destroyed,
 *	this command destroys it.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The widget is destroyed.
 *
 *----------------------------------------------------------------------
 */

static void
FloatCmdDeletedProc(clientData)
    ClientData clientData;	/* Pointer to widget record for widget. */
{
    TkFloat *butPtr = (TkFloat *) clientData;
    Tk_Window tkwin = butPtr->tkwin;

    /*
     * This procedure could be invoked either because the window was
     * destroyed and the command was then deleted (in which case tkwin
     * is NULL) or because the command was deleted, and then this procedure
     * destroys the widget.
     */

    if (tkwin != NULL) {
	butPtr->tkwin = NULL;
	Tk_DestroyWindow(tkwin);
    }
}
/*
 *---------------------------------------------------------------------------
 *
 * TkComputeAnchor --
 *
 *      Determine where to place a rectangle so that it will be properly
 *      anchored with respect to the given window.  Used by widgets
 *      to align a box of text inside a window.  When anchoring with
 *      respect to one of the sides, the rectangle be placed inside of
 *      the internal border of the window.
 *
 * Results:
 *      *xPtr and *yPtr set to the upper-left corner of the rectangle
 *      anchored in the window.
 *
 * Side effects:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
TkComputeAnchor(anchor, tkwin, padX, padY, innerWidth, innerHeight, xPtr, yPtr)
    Tk_Anchor anchor;           /* Desired anchor. */
    Tk_Window tkwin;            /* Anchored with respect to this window. */
    int padX, padY;             /* Use this extra padding inside window, in
                                 * addition to the internal border. */
    int innerWidth, innerHeight;/* Size of rectangle to anchor in window. */
    int *xPtr, *yPtr;           /* Returns upper-left corner of anchored
                                 * rectangle. */
{
    switch (anchor) {
        case TK_ANCHOR_NW:
        case TK_ANCHOR_W:
        case TK_ANCHOR_SW:
            *xPtr = Tk_InternalBorderWidth(tkwin) + padX;
            break;

        case TK_ANCHOR_N:
        case TK_ANCHOR_CENTER:
        case TK_ANCHOR_S:
            *xPtr = (Tk_Width(tkwin) - innerWidth) / 2;
            break;

        default:
            *xPtr = Tk_Width(tkwin) - (Tk_InternalBorderWidth(tkwin) + padX)
                    - innerWidth;
            break;
    }

    switch (anchor) {
        case TK_ANCHOR_NW:
        case TK_ANCHOR_N:
        case TK_ANCHOR_NE:
            *yPtr = Tk_InternalBorderWidth(tkwin) + padY;
            break;

        case TK_ANCHOR_W:
        case TK_ANCHOR_CENTER:
        case TK_ANCHOR_E:
            *yPtr = (Tk_Height(tkwin) - innerHeight) / 2;
            break;   
        default:
            *yPtr = Tk_Height(tkwin) - Tk_InternalBorderWidth(tkwin) - padY
                    - innerHeight;
            break;
    }
}    
/*
 * Tcl initialization function
 * create float command
*/
#ifdef _WINDOWS
__declspec( dllexport )
#endif
int Float_Init(Tcl_Interp *interp) {

  if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
    return TCL_ERROR;
  if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL)
    return TCL_ERROR;
  if (Tcl_PkgProvide(interp, "Float" , "1.0") != TCL_OK)
    return TCL_ERROR;

    Tcl_CreateCommand (interp, "float",
                       Tk_FloatCmd,
                       (ClientData) Tk_MainWindow (interp),
                       (Tcl_CmdDeleteProc*) NULL);
    #ifdef DBUG
    // fprintf(stderr,"matrixtcl_Init\n");
    #endif

    /* register float widget as available package */
    return TCL_OK;
}
 
int Float_SafeInit(Tcl_Interp *interp) {
    return Float_Init (interp);
}

