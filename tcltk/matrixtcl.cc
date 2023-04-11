/*
 * matrixtcl.c
 *
 * Tcl Interface for matrix algorithmus
 * Autor: Artur Trzewik
 * Date: 28.11.1998
 * Version: 0.2
 */

#include "matrixtcl.h"

template class polynom<rational>;
template class polynom<double>;
template class polynom<widefloat>;


//#define DEBUG 1
//#undef DEBUG

//#if (TCL_MAJOR_VERSION < 8)
//this program requires TCL 8.0
//#endif


//#define std_noremove

matrix_ConfigSpec MatrixConfigSpecs[] = {
  {MATRIX_CONFIG_BOOLEAN, "-noprot", "noprot","0",
   matrix_Offset(matrixtcl_config, noprot),0},
  {MATRIX_CONFIG_BOOLEAN, "-onlyfraction", "onlyfraction","0",
   matrix_Offset(matrixtcl_config, onlyfraction),0},
  {MATRIX_CONFIG_STRING, "-dir", "dir",".",
   matrix_Offset(matrixtcl_config, dir),0},
  {MATRIX_CONFIG_INT, "-widefloat_prec", "widefloat_prec","64",
   matrix_Offset(matrixtcl_config, widefloat_prec),0},
  {MATRIX_CONFIG_INT, "-widefloat_showprec", "widefloat_showprec","0",
   matrix_Offset(matrixtcl_config, widefloat_showprec),0},
  {MATRIX_CONFIG_INT, "-widefloat_maxzeros", "widefloat_maxzeros","6",
   matrix_Offset(matrixtcl_config, widefloat_maxzeros),0},
  {MATRIX_CONFIG_INT, "-widefloat_maxnum", "widefloat_maxnum","6",
   matrix_Offset(matrixtcl_config, widefloat_maxnum),0},
  {MATRIX_CONFIG_BOOLEAN, "-widefloat_reformat", "widefloat_reformat","1",
   matrix_Offset(matrixtcl_config, widefloat_reformat),0},
  {MATRIX_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,0,0}
};

Manager_matrix::Manager_matrix() {
  Tcl_InitHashTable (&matrixTable, TCL_STRING_KEYS);
  Tcl_InitHashTable (&formTable, TCL_STRING_KEYS);
  matrixcount=0;
}
Manager_matrix::~Manager_matrix() {
  Tcl_HashEntry *entryPtr;
  Tcl_HashSearch search;
  for (entryPtr=Tcl_FirstHashEntry(&matrixTable,&search); 
       entryPtr!=NULL;
       entryPtr=Tcl_NextHashEntry(&search)) {
    delete (matrix_tcl *)Tcl_GetHashKey(&matrixTable,entryPtr);
  }
  for (entryPtr=Tcl_FirstHashEntry(&formTable,&search);
       entryPtr!=NULL;
       entryPtr=Tcl_NextHashEntry(&search)) {
    delete (protformat *)Tcl_GetHashKey(&formTable,entryPtr);
  }
  Tcl_DeleteHashTable(&matrixTable);
  Tcl_DeleteHashTable(&formTable);
  matrix_FreeOptions(MatrixConfigSpecs,(char *)&config);
}
void Manager_matrix::add(Tcl_Interp *interp,matrix_tcl *m) {
  Tcl_HashEntry *entryPtr;
  int newflag;
  int length;
  Tcl_SetObjResult(interp,Tcl_ObjPrintf("matrix%d",matrixcount++));
  entryPtr=Tcl_CreateHashEntry(&matrixTable,Tcl_GetStringFromObj(Tcl_GetObjResult(interp),&length),&newflag);
  Tcl_SetHashValue(entryPtr,m);     
}
void Manager_matrix::create(Tcl_Interp *interp,const char *row,const char *col,const char *isfloat) {
  int irow,icol;
  MatrixObject::MatrixType type=MatrixObject::trational;
  char name[25];
  if (Tcl_GetInt(interp,row,&irow)==TCL_ERROR || 
      Tcl_GetInt(interp,col,&icol)==TCL_ERROR) 
    throw badformat(string("col or row numbers incorrect"));
  if (isfloat) {
      if (strcmp(isfloat, "-float")==0)
	type=MatrixObject::tfloat;
#if !defined(NO_GMP)
      else if (strcmp(isfloat, "-widefloat")==0)
	type=MatrixObject::twidefloat;
#endif	
      else if (strcmp(isfloat, "-rational")==0)
	type=MatrixObject::trational;
      else 
	throw badformat(string("only option -float -widefloat or -rational is proper"));
  }
  matrix_tcl *m=new matrix_tcl(irow,icol,type);
  add(interp,m);
}
void Manager_matrix::load(Tcl_Interp *interp,const char *filename,matrix_tcl::LoadType t) {
  Tcl_HashEntry *entryPtr;
  matrix_tcl *m=new matrix_tcl(filename,t);
  
  // fprintf(stderr,"after load\n");

  add(interp,m);
}
void Manager_matrix::del(const char *mname) {
  Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&matrixTable,mname);
  if (entryPtr) {
     matrix_tcl *m = (matrix_tcl *)Tcl_GetHashValue(entryPtr);
     delete m;
     Tcl_DeleteHashEntry(entryPtr);
  } else throw badformat(string("matrix object \"")+string(mname)+string("\" not exist")); 
}
matrix_tcl *Manager_matrix::find(const char *mname) {
  Tcl_HashEntry *entryPtr;
  int newflag;
  entryPtr = Tcl_FindHashEntry(&matrixTable,mname);
  if (entryPtr == NULL) {
    return 0;
  } else {
     return (matrix_tcl *)Tcl_GetHashValue(entryPtr);
  }
}
protformat *Manager_matrix::getprot(const char *pname) {
  Tcl_HashEntry *entryPtr;
  int newflag;
//   fprintf(stderr,"getprot %s\n",pname);
  entryPtr = Tcl_FindHashEntry(&formTable,pname);
  if (entryPtr == NULL) {
     protformat *pformat;
     pformat=new protformat(config.dir,"",false,pname);
// fprintf(stderr,"protformat created\n");
     entryPtr=Tcl_CreateHashEntry(&formTable,pname,&newflag);
     Tcl_SetHashValue(entryPtr,pformat);
     return pformat; 
  } else {
     return (protformat *)Tcl_GetHashValue(entryPtr);
  }
}
//  void Manager_matrix::setpath(char* pdir) {
//    dir.operator=(pdir);
//  }
//  const char *Manager_matrix::getpath() {
//    return dir.c_str();
//  }
int Manager_matrix::info(Tcl_Interp *interp) {
  // cerr<<"config noprot "<<config.noprot<<" onlyfraction"<<config.onlyfraction<<endl;
  Tcl_HashEntry *entryPtr;
  Tcl_HashSearch search;
  Tcl_Obj *list = Tcl_NewListObj(0,NULL);
  Tcl_SetObjResult(interp,list);
  for (entryPtr=Tcl_FirstHashEntry(&matrixTable,&search);
       entryPtr!=NULL;
       entryPtr=Tcl_NextHashEntry(&search)) {
        Tcl_ListObjAppendElement(interp,list,Tcl_NewStringObj((char *)Tcl_GetHashKey(&matrixTable,entryPtr),-1));
  }
  return TCL_OK;
}
int Manager_matrix::handlematrix(Tcl_Interp *interp,int argc,const char *argv[]) {
  int res=TCL_OK;
  matrix_tcl *matrixtcl=find(argv[0]);
  if (!matrixtcl) {
    Tcl_AddErrorInfo(interp,"matrixtcl handle not found");
    res=TCL_ERROR;
  } else {
    // setup current config
    matrixtcl->setprotstat(config.noprot);
    if (argc==1) { 
      Tcl_AddErrorInfo(interp,"no function specified");
      res=TCL_ERROR;
    } else if (strcmp(argv[1],"info")==0) {
      if (argc==3)
	return matrixtcl->info(interp,argv[2]);
      else {
	Tcl_AddErrorInfo(interp,"no info type specified size type issolution isprotocol");
	res=TCL_ERROR;
      }
    } else if (strcmp(argv[1],"setelem")==0) {
      return matrixtcl->setelem(interp,argc-2,&argv[2],config);
    } else if (strcmp(argv[1],"deleteprot")==0) {
      return matrixtcl->deleteprot(interp,argc-2,&argv[2]);
    } else if (strcmp(argv[1],"addprot")==0) {
      return matrixtcl->cmd_addprot(interp,argc-2,&argv[2]);
    } else if (strcmp(argv[1],"setsolution")==0) {
      return matrixtcl->setsolution(interp,argc-2,&argv[2]);
    } else if (strcmp(argv[1],"getelem")==0) {
      return matrixtcl->getelem(interp,argc-2,&argv[2],config);
    } else if (strcmp(argv[1],"algorithmus")==0) {
      return matrixtcl->algorithmus(interp,argc-2,&argv[2]);
    } else if (strcmp(argv[1],"elemunf")==0) {
      return matrixtcl->elemunf(interp,argc-2,&argv[2]);
    } 
    //else if (strcmp(argv[1],"undo")==0) {
    //  return matrixtcl->undo(interp,argc-2,&argv[2]);
    //} 
    else if (strcmp(argv[1],"resize")==0) {
      return matrixtcl->resize(interp,argc-2,&argv[2]);
    } else if (strcmp(argv[1],"duplicate")==0) {
      // man k�nte hier auch Ausschneiden inplementieren
      if (argc==3 && strcmp(argv[2],"-solution")==0) {
	MatrixObject *mo=matrixtcl->givesolution();
	if (mo) {
	  add(interp,new matrix_tcl(mo));
	  return TCL_OK;
	} else {
	  Tcl_AddErrorInfo(interp,"can't duplicate solution");
	  return TCL_ERROR;
	}
      } else {
	add(interp,new matrix_tcl(matrixtcl->dupplicate()));
	return TCL_OK;
      }
    } else if (strcmp(argv[1],"copy")==0) {
      // auch Ausschneiden
      MatrixObject *mo=matrixtcl->copy(interp,argc-2,&argv[2]);
      if (mo) {
	add(interp,new matrix_tcl(mo));
	return TCL_OK;
      } else {
	return TCL_ERROR;
      }
    } else if (strcmp(argv[1],"tofloat")==0) {
      add(interp,new matrix_tcl(matrixtcl->tofloat(interp,argc-2,&argv[2])));
      return TCL_OK;
    } else if (strcmp(argv[1],"operation")==0) {
      if (argc>=3) {
	matrix_tcl *mtcl2=find(argv[2]);
	if (mtcl2) {
	  return matrixtcl->operation(interp,mtcl2,argc-3,&argv[3]);
	} else {
	  Tcl_AddErrorInfo(interp,"handle for operant 2 not exist");
	  res=TCL_ERROR;
	}
      } else {
	Tcl_AddErrorInfo(interp,"too many argumnets for operation");
	res=TCL_ERROR;
      }
    } else if (strcmp(argv[1],"save")==0) {
      if (argc==3) { 
	return matrixtcl->save(interp,argv[2]);
      } else if (argc==4) {
	if (strcmp(argv[3],"-octavebin")==0) 
	  return matrixtcl->save(interp,argv[2],matrix_tcl::octavebin);
	else if (strcmp(argv[3],"-mathlabbin")==0)
	  return matrixtcl->save(interp,argv[2],matrix_tcl::mathlabbin);
	else {
	  Tcl_AddErrorInfo(interp,"syntax: matrix handler save filename -octavebin|-mathlabbin");
	  res=TCL_ERROR;
	} 
      } else {
	Tcl_AddErrorInfo(interp,"save need filename");
	res=TCL_ERROR;
      }
    } else if (strcmp(argv[1],"getform")==0) {
      if (argc>=3) { 
	protformat *pformat=getprot(argv[2]);
        // Setup current config
        pformat->nurbruch=config.onlyfraction;
	pformat->widefloat_reformat=config.widefloat_reformat;
	pformat->widefloat_maxnum=config.widefloat_maxnum;
	pformat->widefloat_maxzeros=config.widefloat_maxzeros;
	pformat->widefloat_showprec=config.widefloat_showprec;
	if (pformat) {
	  if (argc==4)
	    return matrixtcl->getform(interp,pformat,argv[3]);
	  else if (argc==3)
	    return matrixtcl->getform(interp,pformat,0);
	  else {
	    Tcl_AddErrorInfo(interp,"too many argumnets for getform");
	    res=TCL_ERROR;
	  }
	} else {
	  Tcl_AddErrorInfo(interp,"getform type cant be loaded");
	  res=TCL_ERROR;
	}
      } else {
	Tcl_AddErrorInfo(interp,"no getform type specified");
	res=TCL_ERROR;
      }
    } else {
	Tcl_AddErrorInfo(interp,"unknown function on matrix");
	res=TCL_ERROR;
    }
  }
  return res;
}

int MatrixtclCmd (ClientData manager,Tcl_Interp* interp,int  argc,const char**  argv) {
  if (argc == 1) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj("Usage: matrixtcl ?handle? command ...",-1));
    return TCL_ERROR;
  }
  Manager_matrix *mgr = (Manager_matrix *)manager;
  int res=TCL_OK;
  try {
 //     if (strcmp(argv[1],"setpath")==0) {
//        if (argc==3)
//  	mgr->setpath(argv[2]);
//        else if (argc==2)
//  	Tcl_SetResult(interp,(char *)mgr->getpath(),TCL_VOLATILE);
//        else {
//  	Tcl_AddErrorInfo(interp,"bad argument count for setpath");
//  	res=TCL_ERROR;
//        }
//      } else
    if (strcmp(argv[1],"create")==0) {
      if (argc==5)
	mgr->create(interp,argv[2],argv[3],argv[4]);
      else if (argc==4)
	mgr->create(interp,argv[2],argv[3],0);
      else {
	Tcl_AddErrorInfo(interp,"bad argument count for create");
	res=TCL_ERROR;
      }
    } else if (strcmp(argv[1],"load")==0) {
      if (argc==3)
	mgr->load(interp,argv[2]);
      else if (argc==4) {
	if (strcmp(argv[3],"-octavebin")==0) 
	  mgr->load(interp,argv[2],matrix_tcl::octavebin);
	else if (strcmp(argv[3],"-mathlabbin")==0)
	  mgr->load(interp,argv[2],matrix_tcl::mathlabbin);
	else {
	  Tcl_AddErrorInfo(interp,"syntax: matrix load filename -octavebin|-mathlabbin");
	  res=TCL_ERROR;
	}
      } else {
	Tcl_AddErrorInfo(interp,"syntax: matrix load filename -octavebin|-mathlabbin");
	res=TCL_ERROR;
      }
    } else if (strcmp(argv[1],"info")==0) {
      return mgr->info(interp);
    }  else if (strcmp(argv[1],"destroy")==0) {
      if (argc==3)
	mgr->del(argv[2]);
      else {
	Tcl_AddErrorInfo(interp,"syntax: matrixtcl destroy handle");
	res=TCL_ERROR;
      }
    } else if (strcmp(argv[1], "configure") == 0) {
      if (argc == 2) {
	res = mgr->ConfigureInfo(interp,(char *) NULL);
      } else if (argc == 3) {
	res = mgr->ConfigureInfo(interp,argv[2]);
      } else {
	res = mgr->Configure(interp, argc-2, argv+2);
      }
    } else {
      // expect matrix handle
      return mgr->handlematrix(interp,argc-1,&argv[1]);
    }
  }
  catch(badformat error) {
    string errors=error.givestring();
    const char *s=errors.c_str();
    Tcl_AddErrorInfo(interp,(char *)s);
    return TCL_ERROR;
  }
  return res;
}

// Tcl initialization function
// create a manager and new Tcl Command
extern "C" {
int Matrixtcl_Init(Tcl_Interp *interp);
}
#ifdef _WINDOWS
__declspec( dllexport )
#endif

int Matrixtcl_Init(Tcl_Interp *interp) {
    Manager_matrix *s = new Manager_matrix();

   if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
       return TCL_ERROR;
   if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL)
       return TCL_ERROR;
    Tcl_PkgProvide (interp, "Matrixtcl", "1.0");

   Tcl_CreateCommand (interp, "matrixtcl",
		       &MatrixtclCmd,(ClientData)s,NULL);
    
    /* register memory channels as available package */
    return TCL_OK;
}

extern "C" {
int Matrix_SafeInit(Tcl_Interp *interp);
}

int Matrixtcl_SafeInit(Tcl_Interp *interp) {
  return Matrixtcl_Init (interp);
}



int Manager_matrix::Configure(Tcl_Interp *interp,int argc,const char **argv) {
 return matrix_Configure(interp,MatrixConfigSpecs,argc,argv,(char *)&config);
}
int Manager_matrix::ConfigureInfo(Tcl_Interp *interp,const char *argvName) {
  matrix_ConfigureInfo(interp,MatrixConfigSpecs,(char *)&config,argvName);
#if !defined(NO_GMP)
  widefloat::set_precision(config.widefloat_prec);
#endif
  return TCL_OK;
}
// This Procedures are taken from tk Distribution file tkConfig.c
// and addapted for using with matrixtcl
//
/*
 *--------------------------------------------------------------
 *
 * matrix_ConfigureWidget --
 *
 *	Process command-line options and database options to
 *	fill in fields of a widget record with resources and
 *	other parameters.
 *
 * Results:
 *	A standard Tcl return value.  In case of an error,
 *	interp->result will hold an error message.
 *
 * Side effects:
 *	The fields of widgRec get filled in with information
 *	from argc/argv and the option database.  Old information
 *	in widgRec's fields gets recycled.
 *
 *--------------------------------------------------------------
 */

int Manager_matrix::matrix_Configure(Tcl_Interp *interp,
				     matrix_ConfigSpec *specs,
				     int argc,const char **argv,char *widgRec) {
  matrix_ConfigSpec *specPtr;
  char const *value;  

    /*
     * Pass one:  scan through all of the arguments, processing those
     * that match entries in the specs.
     */

  for ( ; argc > 0; argc -= 2, argv += 2) {
    specPtr = FindConfigSpec(interp, specs, *argv);
    if (specPtr == NULL) {
      return TCL_ERROR;
    }

    /*
     * Process the entry.
     */

    if (argc < 2) {
      Tcl_AppendResult(interp, "value for \"", *argv,
		       "\" missing", (char *) NULL);
      return TCL_ERROR;
    }
    if (DoConfig(interp, specPtr, argv[1], widgRec) != TCL_OK) {
      char msg[100];
      sprintf(msg, "\n    (processing \"%.40s\" option)",
	      specPtr->argvName);
      Tcl_AddErrorInfo(interp, msg);
      return TCL_ERROR;
    }
    specPtr->specFlags |= MATRIX_CONFIG_OPTION_SPECIFIED;
  }

  /*
     * Pass two:  scan through all of the specs again;  if no
     * command-line argument matched a spec, then check for info
     * in the option database.  If there was nothing in the
     * database, then use the default.
     */

  for (specPtr = specs; specPtr->type != MATRIX_CONFIG_END; specPtr++) {
    if (specPtr->specFlags & MATRIX_CONFIG_OPTION_SPECIFIED) continue;
    value = specPtr->defValue;
    // str 
    if (value != NULL) {
      if (DoConfig(interp, specPtr, value, widgRec) !=
	  TCL_OK) {
	char msg[200];
	sprintf(msg,"default value for\n");
	Tcl_AddErrorInfo(interp, msg);
	return TCL_ERROR;
      }
    }
  }
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * FindConfigSpec --
 *
 *	Search through a table of configuration specs, looking for
 *	one that matches a given argvName.
 *
 * Results:
 *	The return value is a pointer to the matching entry, or NULL
 *	if nothing matched.  In that case an error message is left
 *	in interp->result.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

matrix_ConfigSpec *
Manager_matrix::FindConfigSpec(Tcl_Interp *interp,matrix_ConfigSpec *specs,
			       const char *argvName) {
  matrix_ConfigSpec *specPtr;
  char c;		/* First character of current argument. */
  matrix_ConfigSpec *matchPtr;	/* Matching spec, or NULL. */
  size_t length;

  c = argvName[1];
  length = strlen(argvName);
  matchPtr = NULL;
  for (specPtr = specs; specPtr->type != MATRIX_CONFIG_END; specPtr++) {
    if (specPtr->argvName == NULL) {
      continue;
    }
    if ((specPtr->argvName[1] != c)
	|| (strncmp(specPtr->argvName, argvName, length) != 0)) {
      continue;
    }
    // 	if (((specPtr->specFlags & needFlags) != needFlags)
    // 		|| (specPtr->specFlags & hateFlags)) {
    // 	    continue;
    // 	}
    if (specPtr->argvName[length] == 0) {
      matchPtr = specPtr;
      goto gotMatch;
    }
    if (matchPtr != NULL) {
      Tcl_AppendResult(interp, "ambiguous option \"", argvName,
		       "\"", (char *) NULL);
      return (matrix_ConfigSpec *) NULL;
    }
    matchPtr = specPtr;
  }

  if (matchPtr == NULL) {
    Tcl_AppendResult(interp, "unknown option \"", argvName,
		     "\"", (char *) NULL);
    return (matrix_ConfigSpec *) NULL;
  }

  /*
     * Found a matching entry.  If it's a synonym, then find the
     * entry that it's a synonym for.
     */

			       gotMatch:
  specPtr = matchPtr;
  return specPtr;
}

/*
 *--------------------------------------------------------------
 *
 * DoConfig --
 *
 *	This procedure applies a single configuration option
 *	to a widget record.
 *
 * Results:
 *	A standard Tcl return value.
 *
 * Side effects:
 *	WidgRec is modified as indicated by specPtr and value.
 *	The old value is recycled, if that is appropriate for
 *	the value type.
 *
 *--------------------------------------------------------------
 */

int
Manager_matrix::DoConfig(Tcl_Interp *interp,matrix_ConfigSpec *specPtr,
			 const char *value,char *widgRec) {
  char *ptr;

  do {
    ptr = widgRec + specPtr->offset;
    switch (specPtr->type) {
    case MATRIX_CONFIG_BOOLEAN:
      if (Tcl_GetBoolean(interp, value, (int *) ptr) != TCL_OK) {
	return TCL_ERROR;
      }
      break;
    case MATRIX_CONFIG_INT:
      if (Tcl_GetInt(interp, value, (int *) ptr) != TCL_OK) {
	return TCL_ERROR;
      }
      break;
    case MATRIX_CONFIG_DOUBLE:
      if (Tcl_GetDouble(interp, value, (double *) ptr) != TCL_OK) {
	return TCL_ERROR;
      }
      break;
    case MATRIX_CONFIG_STRING: {
      char *old, *new_s;
      new_s = (char *) ckalloc((unsigned) (strlen(value) + 1));
      strcpy(new_s, value);

      old = *((char **) ptr);
      if (old != NULL) {
	ckfree(old);
      }
      *((char **) ptr) = new_s;
      break;
    }
    default: {
      Tcl_SetObjResult(interp,Tcl_ObjPrintf("bad config table: unknown type %d",specPtr->type));
      return TCL_ERROR;
    }
    }
    specPtr++;
  } while ((specPtr->argvName == NULL) && (specPtr->type != MATRIX_CONFIG_END));
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * matrix_ConfigureInfo --
 *
 *	Return information about the configuration options
 *	for a window, and their current values.
 *
 * Results:
 *	Always returns TCL_OK.  Interp->result will be modified
 *	hold a description of either a single configuration option
 *	available for "widgRec" via "specs", or all the configuration
 *	options available.  In the "all" case, the result will
 *	available for "widgRec" via "specs".  The result will
 *	be a list, each of whose entries describes one option.
 *	Each entry will itself be a list containing the option's
 *	name for use on command lines, database name, database
 *	class, default value, and current value (empty string
 *	if none).  For options that are synonyms, the list will
 *	contain only two values:  name and synonym name.  If the
 *	"name" argument is non-NULL, then the only information
 *	returned is that for the named argument (i.e. the corresponding
 *	entry in the overall list is returned).
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

int Manager_matrix::matrix_ConfigureInfo(Tcl_Interp *interp,
					 matrix_ConfigSpec *specs,
					 char *widgRec,const char *argvName) {
  matrix_ConfigSpec *specPtr;

  if (argvName != NULL) {
    specPtr = FindConfigSpec(interp, specs, argvName);
    if (specPtr == NULL) {
      return TCL_ERROR;
    }
    Tcl_SetObjResult(interp,FormatConfigInfo(interp, specPtr, widgRec));
    return TCL_OK;
  }

  /*
   * Loop through all the specs, creating a big list with all
   * their information.
   */
  Tcl_Obj *list = Tcl_NewListObj(0, NULL);
  for (specPtr = specs; specPtr->type != MATRIX_CONFIG_END; specPtr++) {
    if ((argvName != NULL) && (specPtr->argvName != argvName)) {
      continue;
    }
    if (specPtr->argvName == NULL) {
      continue;
    }
    Tcl_ListObjAppendElement(interp,list,FormatConfigInfo(interp, specPtr, widgRec));
  }
  Tcl_SetObjResult(interp,list);
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * FormatConfigInfo --
 *
 *	Create a valid Tcl list holding the configuration information
 *	for a single configuration option.
 *
 * Results:
 *	A Tcl list, dynamically allocated.  The caller is expected to
 *	arrange for this list to be freed eventually.
 *
 * Side effects:
 *	Memory is allocated.
 *
 *--------------------------------------------------------------
 */

Tcl_Obj* Manager_matrix::FormatConfigInfo(Tcl_Interp *interp,
				 matrix_ConfigSpec *specPtr,
				 char *widgRec) {
  Tcl_Obj* resList = Tcl_NewListObj(0, NULL);
  Tcl_ListObjAppendElement(interp, resList, Tcl_NewStringObj(specPtr->argvName,-1));
  Tcl_ListObjAppendElement(interp, resList, Tcl_NewStringObj(specPtr->dbName,-1));
  Tcl_ListObjAppendElement(interp, resList, Tcl_NewStringObj(specPtr->defValue,-1));
  Tcl_ListObjAppendElement(interp, resList, FormatConfigValue(interp, specPtr, widgRec));
  return resList;
}

/*
 *----------------------------------------------------------------------
 *
 * FormatConfigValue --
 *
 *	This procedure formats the current value of a configuration
 *	option.
 *
 * Results:
 *	The return value is the formatted value of the option given
 *	by specPtr and widgRec.  If the value is static, so that it
 *	need not be freed, *freeProcPtr will be set to NULL;  otherwise
 *	*freeProcPtr will be set to the address of a procedure to
 *	free the result, and the caller must invoke this procedure
 *	when it is finished with the result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj* Manager_matrix::FormatConfigValue(Tcl_Interp *interp,
				  matrix_ConfigSpec *specPtr,char *widgRec) {
  Tcl_Obj *result;
  char *ptr;
  ptr = widgRec + specPtr->offset;
  switch (specPtr->type) {
  case MATRIX_CONFIG_BOOLEAN:
    result = Tcl_NewBooleanObj(*((int *) ptr) == 0 ? 0 : 1);
    break;
  case MATRIX_CONFIG_INT:
    result = Tcl_NewIntObj(*((int *) ptr));
    break;
  case MATRIX_CONFIG_DOUBLE:
    result = Tcl_NewDoubleObj(*((double *) ptr));
    break;
  case MATRIX_CONFIG_STRING:
    result = Tcl_NewStringObj((*(char **) ptr),-1);
    break;
  default: 
    result = Tcl_NewStringObj("?? unknown type ??",-1);
  }
  return result;
}

/*
 *----------------------------------------------------------------------
 *
 * matrix_ConfigureValue --
 *
 *	This procedure returns the current value of a configuration
 *	option for a widget.
 *
 * Results:
 *	The return value is a standard Tcl completion code (TCL_OK or
 *	TCL_ERROR).  Interp->result will be set to hold either the value
 *	of the option given by argvName (if TCL_OK is returned) or
 *	an error message (if TCL_ERROR is returned).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Manager_matrix::matrix_ConfigureValue(Tcl_Interp *interp,
				      matrix_ConfigSpec *specs,
				      char *widgRec,char *argvName) {
  matrix_ConfigSpec *specPtr;
  specPtr = FindConfigSpec(interp, specs, argvName);
  if (specPtr == NULL) {
    return TCL_ERROR;
  }
  Tcl_SetObjResult(interp,FormatConfigValue(interp, specPtr, widgRec));
  return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * matrix_FreeOptions --
 *
 *	Free up all resources associated with configuration options.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Any resource in widgRec that is controlled by a configuration
 *	option (e.g. a matrix_3DBorder or XColor) is freed in the appropriate
 *	fashion.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
void
Manager_matrix::matrix_FreeOptions( matrix_ConfigSpec *specs,char *widgRec) {
  matrix_ConfigSpec *specPtr;
  char *ptr;

  for (specPtr = specs; specPtr->type != MATRIX_CONFIG_END; specPtr++) {
    ptr = widgRec + specPtr->offset;
    switch (specPtr->type) {
    case MATRIX_CONFIG_STRING:
      if (*((char **) ptr) != NULL) {
	ckfree(*((char **) ptr));
	*((char **) ptr) = NULL;
      }
      break;
    }
  }
}
