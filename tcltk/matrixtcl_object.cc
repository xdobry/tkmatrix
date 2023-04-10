//
// This is a part of matrixtcl tcl inteface
// matrix_tcl class implementation
// Autor: Artur Trzewik
//
#include "matrixtcl.h"
#include <algorithm>
#include <sstream>

//extern void matrixkx<double>::PrintOn(ostream &,protformat &) const;
//extern void matrixkx<rational>::PrintOn(ostream &,protformat &) const;

template class matrix<rational>;
template class matrix<double>;
template class matrix<widefloat>;


typedef matrix<rational> mrational;
typedef matrix<double> mdouble;

#if !defined(NO_GMP)
typedef matrix<widefloat> mwidefloat;
//extern void matrixkx<widefloat>::PrintOn(ostream &,protformat &) const;
#endif

#define alg_count 15
char *algorithmen[alg_count]={"none","gauss","jordan","loesung","kern","bild",
			      "inverse","determinante","pivotiere","naehrung",
			      "eckenfindung","simplex","transponiere",
			      "chpolynom","matrixspiel"};
enum talgorithmus {none,gauss,jordan,loesung,kern,bild,inverse,determinante,
                   pivotiere,naehrung,eckenfindung,simplex,transponiere,
                   ch_polynom,matrixspiel};


#define elemunf_count 15
char *elemunfs[elemunf_count]={"swap","factormul","factoradd","pivot","changestep"};
enum telemunf {eu_swap,factormul,factoradd,pivot,changestep};

#define ptype_count 7
char *prottypen[7]={"matrix","gleichung","simplex","loesung","determinante",
                   "polynom","string"};

matrix_tcl::matrix_tcl(MatrixObject *mo):pmatrix(mo) {
#ifdef DEBUG
  fprintf(stderr,"matrix_tcl Object address %p created\n",this);
#endif
  smatrix=0;
}
matrix_tcl::matrix_tcl(int row,int col,MatrixObject::MatrixType mtype) {
#ifdef DEBUG
  fprintf(stderr,"matrix_tcl Object address %p created\n",this);
#endif
  if (row*col<=0) { 
    throw badformat("bad row or cols number");
  }
  switch (mtype) {
  case MatrixObject::trational:
    pmatrix=new matrix<rational>(row,col,*new protstream());
    break;
  case MatrixObject::tfloat:
    //    fprintf(stderr,"creating float\n");
    pmatrix=new matrix<double>(row,col,*new protstream());
    break;
#if !defined(NO_GMP)
 case MatrixObject::twidefloat:
    //    fprintf(stderr,"creating widefloat\n");
    pmatrix=new matrix<widefloat>(row,col,*new protstream());
    break;
#endif
  }
  smatrix=0;
}
matrix_tcl::matrix_tcl(const char *filename,LoadType type) {
#ifdef DEBUG
  fprintf(stderr,"matrix_tcl Object address %p created\n",this);
#endif
  ifstream we(filename);
  smatrix=0;
  pmatrix=0;
  if (we) {
    switch(type) {
    case tkmatrix:
      int m,n;
      char puffer[7]; // Eventuel fr Zeichenkette "FLOAT\0"
      if (we.eof()) { throw badformat(string("cant load matrix")); }
      we.read(puffer,6);
      puffer[6]=(char)0;
      if(strncmp(puffer,"FLOAT",5)==0) { // Eine Gleitkommamatrix
	we>>m; we>>n;
	if (!we || we.eof()) { throw badformat(string("cant load matrix")); }
	pmatrix=new matrix<double>(m,n,*new protstream());
	pmatrix->Restore(we);
	//    } else if (strcmp(puffer,"FLOAX")==0) {
	// we>>m; we>>n;
	// if (!we || we.eof()) {  return; }
	// readMatrix=new matrix<double>(m,n,*new protstream());
	// dynamic_cast<matrix<double> *>(readMatrix)->scanDigit(we);
#if !defined(NO_GMP)
      } else if (strncmp(puffer,"WFLOAT",6)==0) {
	we>>m; we>>n;
	if (!we || we.eof()) { throw badformat(string("cant load matrix")); }
	pmatrix=new matrix<widefloat>(m,n,*new protstream());
	pmatrix->Restore(we);
#endif	
      } else { // Eine Bruchzahlenmatrix
	// fprintf(stderr,"loading matrix2\n");
	we.seekg(0,ios::beg);
	we>>m; // matrix m x n m-hoehe n-breite
	we>>n;
	// fprintf(stderr,"hight %d with %d\n",m,n);
	if (!we || we.eof()) {  throw badformat(string("cant load matrix")); }
	// fprintf(stderr,"loading matrix3y\n");
	pmatrix=new matrix<rational>(m,n,*new protstream());
	pmatrix->Restore(we);
      }
      break;
    case octavebin:
      read_octavebin(we);
      break;
    case mathlabbin:
      read_mathlabbin(we);
      break;
    }
    we.close();
  } else {
    throw badformat(string("cant open file:")+string(filename));
  }
}
matrix_tcl::~matrix_tcl() {
#ifdef DEBUG
  fprintf(stderr,"matrix_tcl Object address %p deleted\n",this);
#endif
  delete &(pmatrix->prot);
  delete pmatrix;
  if (smatrix) delete smatrix;
}
int matrix_tcl::algorithmus(Tcl_Interp *interp,int argc,const char *argv[]) {
  char puf[200];
  int t;
  if (argc>=1) {
    for (t=0;t<alg_count;t++) {
      if (strcmp(argv[0],algorithmen[t])==0) break;
    }
    if (t==alg_count) {
      if(strlen(argv[0])>50)
	sprintf(puf,"bad algorithmus name line %d",(const char *)interp->errorLine);
      else
	sprintf(puf,"bad algorithmus name \"%s\" line %d",argv[0],interp->errorLine);
      Tcl_AddErrorInfo(interp,puf);
      return TCL_ERROR;
    }
    try {
      VorbereiteAlgorithmus();
      switch (t) {
      case gauss: smatrix->gauss(); break;
      case jordan: smatrix->gauss_jordan(); break;
      case loesung: 
	addProt(smatrix,MatrixObject::tgleichung); 
	smatrix->gauss_jordan(); smatrix->loesung(); 
	addProt(smatrix,MatrixObject::tloesung); 
	break;
      case kern: smatrix->kern(); break;
      case bild: smatrix->bild(); break;
      case inverse: smatrix->inverse(); break;
      case determinante: smatrix->determinante(); 
	addProt(smatrix,MatrixObject::tdeterminante);  
	break;
      case  naehrung: addProt(smatrix,MatrixObject::tgleichung); 
	smatrix->naehrungsloesung(); 
	addProt(smatrix,MatrixObject::tloesung); 
	break;
      case pivotiere:
	int x,y;
	if (argc==3) {
	  if (Tcl_GetInt(interp,argv[1],&y)==TCL_ERROR || 
	      Tcl_GetInt(interp,argv[2],&x)==TCL_ERROR) {
	    Tcl_AddErrorInfo(interp,"cant read col or row numbers");
	    delete smatrix;
	    smatrix=0;
	    return TCL_ERROR;
	  }
	  if (y>=smatrix->mhoehe() || x>=smatrix->nbreite() || x<0 || y<0) {
	    Tcl_SetResult(interp,"outof_range",TCL_STATIC);
	    Tcl_AddErrorInfo(interp,"col or row numbers not in matrix range");
	    delete smatrix;
	    smatrix=0;
	    return TCL_ERROR;
	  }
	  // *(e->prot)<<"Ausgagsmatrix zum Pivotieren\n"<<(*a);
	  smatrix->prot.operator<<("Ausgagsmatrix zum Pivotieren\n");
	  addProt(smatrix,MatrixObject::tmatrix);
	  smatrix->pivotiere(y,x);
	} else {
	    Tcl_AddErrorInfo(interp,"need col and row numbers");
	    return TCL_ERROR;
	}
	break;
      case eckenfindung: smatrix->eckenfindung(); break;
      case simplex: 
	addProt(smatrix,MatrixObject::tsimplex);
	smatrix->optimiere(); 	  
	addProt(smatrix,MatrixObject::tloesung);
	break;
      case transponiere: smatrix->transponiere(); break;
      case ch_polynom: 
	smatrix->chpol();
        break;
      case matrixspiel: 
	smatrix->matrixspiel();
        break;
      }
      if (smatrix->mhoehe()*smatrix->nbreite()==0) {
	Tcl_SetResult(interp,smatrix->prot.getError(),TCL_VOLATILE);
        delete smatrix;
        smatrix=0;
	return TCL_ERROR;
      }
    }
    catch (mylongint::overflow) {
      if (smatrix) { 
	delete smatrix;
	smatrix=0;
      }
      Tcl_SetResult(interp,"number_overflow",TCL_STATIC);
      Tcl_AddErrorInfo(interp,"overflow error");
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}
int matrix_tcl::elemunf(Tcl_Interp *interp,int argc,const char *argv[]) {
  char puf[200];
  int t;
  if (argc>=1) {
    for (t=0;t<elemunf_count;++t) {
      if (strcmp(argv[0],elemunfs[t])==0) break;
    }
    if (t==elemunf_count) {
      if(strlen(argv[0])>50)
	sprintf(puf,"bad elem. Unformung name line %d",(const char *)interp->errorLine);
      else
	sprintf(puf,"bad elem. Unformung name \"%s\" line %d",argv[0],interp->errorLine);
      Tcl_AddErrorInfo(interp,puf);
      return TCL_ERROR;
    }
    try {
      bool row=false,argnum_ok=true,arg_ok=true;
      int arg1,arg2;
      if (argc<3) argnum_ok=false;
      if (t>=eu_swap && t<=pivot && argnum_ok) {
	if (strcmp(argv[1],"row")==0) {
	  row=true;
	} else if (strcmp(argv[1],"col")!=0)
	  throw badformat(string("specify row or col as 1st arg for this operation"));
      }
      if (argnum_ok) {
	// Auslesen von Integer Argumenten
	switch (t) {
	case eu_swap:
	case factoradd:
	case pivot:
	  if (argc<4) { argnum_ok=false; break; }
	  if (Tcl_GetInt(interp,argv[2],&arg1)==TCL_ERROR || 
	      Tcl_GetInt(interp,argv[3],&arg2)==TCL_ERROR) {
	    arg_ok=false;
	  }
	  break;
	case factormul:
	  if (Tcl_GetInt(interp,argv[2],&arg1)==TCL_ERROR) {
	    arg_ok=false;
	  }
	  break;
	case changestep:
	  if (Tcl_GetInt(interp,argv[1],&arg1)==TCL_ERROR || 
	      Tcl_GetInt(interp,argv[2],&arg2)==TCL_ERROR) {
	    arg_ok=false;
	  }
	  break;
	}
      }
      if (argnum_ok) {
	if (!arg_ok) throw badformat(string("cant read integer arguments"));
	// Initieren von Operationnen
	switch (t) {
	case eu_swap:
	  pmatrix->swap(arg1,arg2,row);
	  break;
	case factormul:
	  switch(pmatrix->whoamI()) {
	  case MatrixObject::trational:
	    {
	      int listArgc;
	      const char **listArgv;
	      if (Tcl_SplitList(interp,argv[3],&listArgc,&listArgv)==TCL_ERROR)
		return TCL_ERROR;
	      ((matrix<rational> *)pmatrix)->factormul(arg1,row,rational(listArgc,listArgv));
	      Tcl_Free ((char *) listArgv);
	    }
	    break;
	  case MatrixObject::tfloat:
	    {
	      double d;
	      if (Tcl_GetDouble(interp,argv[3],&d)==TCL_ERROR)
		return TCL_ERROR;
	      ((matrix<double> *)pmatrix)->factormul(arg1,row,d);
	    }
	    break;
#if !defined(NO_GMP)
	  case MatrixObject::twidefloat:
	    {
	      ((matrix<widefloat> *)pmatrix)->factormul(arg1,row,widefloat(argv[3]));
	    }
	    break;
#endif
	  }
	  break;
	case factoradd:
	  if (argc<5) { argnum_ok=false; break; }
	  switch(pmatrix->whoamI()) {
	  case MatrixObject::trational:
	    {
	      int listArgc;
	      const char **listArgv;
	      if (Tcl_SplitList(interp,argv[4],&listArgc,&listArgv)==TCL_ERROR)
		return TCL_ERROR;
	      ((matrix<rational> *)pmatrix)->factoradd(arg1,arg2,row,rational(listArgc,listArgv));
	      Tcl_Free ((char *) listArgv);
	    }
	    break;
	  case MatrixObject::tfloat:
	    {
	      double d;
	      if (Tcl_GetDouble(interp,argv[4],&d)==TCL_ERROR)
		return TCL_ERROR;
	      ((matrix<double> *)pmatrix)->factoradd(arg1,arg2,row,d);
	    }
	    break;
#if !defined(NO_GMP)
	  case MatrixObject::twidefloat:
	    {
	      ((matrix<widefloat> *)pmatrix)->factoradd(arg1,arg2,row,widefloat(argv[4]));
	    }
	    break;
#endif
	  }
	  break;
	case pivot:
	  pmatrix->pivotiere (arg1,arg2,row);
	  break;
	case changestep:
	  pmatrix->austauschschritt(arg1,arg2);
	  break;
	}
      }
      if (!argnum_ok) {
	throw badformat(string("wrong # args for elemunf operation"));
      }
    }
    catch (mylongint::overflow) {
      Tcl_SetResult(interp,"number_overflow",TCL_STATIC);
      Tcl_AddErrorInfo(interp,"overflow error");
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}
int matrix_tcl::operation(Tcl_Interp *interp,matrix_tcl *matrixtcl2,int argc,const char *argv[]) {
  if (argc==0) {
      Tcl_AddErrorInfo(interp,"no operation typ");
      return TCL_ERROR;
  }
  if (pmatrix->whoamI()!=matrixtcl2->pmatrix->whoamI()) {
      Tcl_AddErrorInfo(interp,"matrix are not the same type");
      return TCL_ERROR;
  }
  if (strcmp(argv[0],"add")==0) {
    if (pmatrix->mhoehe()!=matrixtcl2->pmatrix->mhoehe() || 
	pmatrix->nbreite()!=matrixtcl2->pmatrix->nbreite()) {
      Tcl_AddErrorInfo(interp,"matrix are not the same size");
      return TCL_ERROR;
    } 
    VorbereiteAlgorithmus();
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      ((matrix<rational> *)smatrix)->madd(*(matrix<rational> *)matrixtcl2->pmatrix);
      return TCL_OK;
    case MatrixObject::tfloat:
      ((matrix<double> *)smatrix)->madd(*(matrix<double> *)matrixtcl2->pmatrix);
      return TCL_OK;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      ((matrix<widefloat> *)smatrix)->madd(*(matrix<widefloat> *)matrixtcl2->pmatrix);
      return TCL_OK;
#endif
    }
  } else if (strcmp(argv[0],"mul")==0) {
    if (pmatrix->nbreite()!=matrixtcl2->pmatrix->mhoehe()) {
      Tcl_AddErrorInfo(interp,"matrix size not match for mul (m*n X n*t)");
      return TCL_ERROR;
    } 
    VorbereiteAlgorithmus();
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      ((matrix<rational> *)smatrix)->mmul(*(matrix<rational> *)matrixtcl2->pmatrix);
      return TCL_OK;
    case MatrixObject::tfloat:
      ((matrix<double> *)smatrix)->mmul(*(matrix<double> *)matrixtcl2->pmatrix);
      return TCL_OK;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      ((matrix<widefloat> *)smatrix)->mmul(*(matrix<widefloat> *)matrixtcl2->pmatrix);
      return TCL_OK;
#endif
    }
  } else if (strcmp(argv[0],"sub")==0) {
    if (pmatrix->mhoehe()!=matrixtcl2->pmatrix->mhoehe() || 
	pmatrix->nbreite()!=matrixtcl2->pmatrix->nbreite()) {
      Tcl_AddErrorInfo(interp,"matrix are not the same size");
      return TCL_ERROR;
    } 
    VorbereiteAlgorithmus();
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      ((matrix<rational> *)smatrix)->msub(*(matrix<rational> *)matrixtcl2->pmatrix);
      return TCL_OK;
    case MatrixObject::tfloat:
      ((matrix<double> *)smatrix)->msub(*(matrix<double> *)matrixtcl2->pmatrix);
      return TCL_OK;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      ((matrix<widefloat> *)smatrix)->msub(*(matrix<widefloat> *)matrixtcl2->pmatrix);
      return TCL_OK;
#endif
    }
  } else {
      Tcl_AddErrorInfo(interp,"unknown operation typ");
      return TCL_ERROR;
  }
  return TCL_OK;
}
int matrix_tcl::info(Tcl_Interp *interp,const char *typ) {
  char puf[50];
  if (strcmp(typ, "size") == 0) {
    sprintf(puf,"%d",pmatrix->mhoehe());
    Tcl_AppendElement(interp,puf);
    sprintf(puf,"%d",pmatrix->nbreite());
    Tcl_AppendElement(interp,puf);
  } else if (strcmp(typ, "type") == 0) {
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      Tcl_SetResult(interp,"rational",TCL_STATIC);
      break;
    case MatrixObject::tfloat:
      Tcl_SetResult(interp,"float",TCL_STATIC);
      break;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      Tcl_SetResult(interp,"widefloat",TCL_STATIC);
      break;
#endif
    }
  } else if (strcmp(typ, "issolution") == 0) {
    if (smatrix)
      Tcl_SetResult(interp,"yes",TCL_STATIC);
    else
      Tcl_SetResult(interp,"no",TCL_STATIC);
  } else if (strcmp(typ, "isprotokol") == 0) {
    if (!pmatrix->prot.empty())
      Tcl_SetResult(interp,"yes",TCL_STATIC);
    else
      Tcl_SetResult(interp,"no",TCL_STATIC);
  } else {
    Tcl_AddErrorInfo(interp,"info can be type|size|issolution|isprotokol");
    return TCL_ERROR;
  } 
  return TCL_OK;
}
void matrix_tcl::setprotstat(bool noprot) {
  pmatrix->prot.noprot=noprot;
}
int matrix_tcl::getform(Tcl_Interp *interp,protformat *pformat,const char *option) {
  ostringstream os;
  if (option==0) {
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      {
	// #1 taj mit matrix<rational> staat mrational geht es nicht
	matrixkx<rational> mrkx(*(mrational *)pmatrix,MatrixObject::tmatrix);
	pformat->openmain(os);
	mrkx.PrintOn(os,*pformat);
      }
      break;
    case MatrixObject::tfloat: 
      {
	matrixkx<double> mfkx(*(mdouble *)pmatrix,MatrixObject::tmatrix);
	pformat->openmain(os);
	mfkx.PrintOn(os,*pformat);
      }
      break;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      {
	matrixkx<widefloat> mfkx(*(mwidefloat *)pmatrix,MatrixObject::tmatrix);
	pformat->openmain(os);
	mfkx.PrintOn(os,*pformat);
      }
      break;
#endif
    }
  } else if (strcmp(option, "-protokol") == 0) {
//  fprintf(stderr,"protokol\n");
    pformat->openmain(os);
    pmatrix->prot.PrintOn(os,*pformat);
  } else if (strcmp(option, "-solution") == 0) {
    if (!smatrix) { 
      Tcl_AddErrorInfo(interp,"no solution for this object yet");
      return TCL_ERROR;
    }
    switch(smatrix->whoamI()) {
    case MatrixObject::trational:
      // #1 taj mit matrix<rational> staat mrational geht es nicht
      {
	matrixkx<rational> mrkx(*(mrational *)smatrix,MatrixObject::tmatrix);
	pformat->openmain(os);
	mrkx.PrintOn(os,*pformat);
      }
      break;
    case MatrixObject::tfloat: 
      {
	matrixkx<double> mfkx(*(mdouble *)smatrix);
	pformat->openmain(os);
	mfkx.PrintOn(os,*pformat);
      }
      break;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat: 
      {
	matrixkx<widefloat> mfkx(*(mwidefloat *)smatrix);
	pformat->openmain(os);
	mfkx.PrintOn(os,*pformat);
      }
      break;
#endif
    }
  } else {
    Tcl_AddErrorInfo(interp,"bad option for getform [-solution|protokol]");
    return TCL_ERROR;
  }
  pformat->closemain(os);
  string res(os.str());
  Tcl_SetResult(interp,(char *)res.c_str(),TCL_VOLATILE);
  return TCL_OK;
}
int matrix_tcl::setelem(Tcl_Interp *interp,int argc,const char *argv[], matrixtcl_config &config) {
  if (argc>=3) {
    int x,y;
    if (Tcl_GetInt(interp,argv[0],&y)==TCL_ERROR || 
	Tcl_GetInt(interp,argv[1],&x)==TCL_ERROR) {
      Tcl_AddErrorInfo(interp,"cant read col or row numbers");
      return TCL_ERROR;
    }
    if (y>=pmatrix->mhoehe() || x>=pmatrix->nbreite() || x<0 || y<0) {
      Tcl_AddErrorInfo(interp,"col or row numbers not in matrix range");
      return TCL_ERROR;
    }
    switch(pmatrix->whoamI()) {
    case MatrixObject::trational:
      {
	int listArgc;
	const char **listArgv;
	if (Tcl_SplitList(interp,argv[2],&listArgc,&listArgv)==TCL_ERROR) {
	  return TCL_ERROR;
	}
	((matrix<rational> *)pmatrix)->operator()(y,x)=rational(listArgc,listArgv);
	Tcl_Free ((char *) listArgv);
	// rational r=((matrix<rational> *)pmatrix)->operator()(y,x);
	string *s;
	if (argc==4 && strcmp(argv[3], "-onlyfraction") == 0)
	  s=((matrix<rational> *)pmatrix)->operator()(y,x).tostring(false);
	else
	  s=((matrix<rational> *)pmatrix)->operator()(y,x).tostring(true);
	Tcl_SetResult(interp,(char *)s->c_str(),TCL_VOLATILE);
	delete s;
	return TCL_OK;
      }
      break;
    case MatrixObject::tfloat:
      {
	double d;
	if (Tcl_GetDouble(interp,argv[2],&d)==TCL_ERROR) {
 	  Tcl_AddErrorInfo(interp,"cant read double number");
	  return TCL_ERROR;
	}
	((matrix<double> *)pmatrix)->operator()(y,x)=d;
	d=((matrix<double> *)pmatrix)->operator()(y,x);
	char puf[30];
	sprintf(puf,"%lf",d);
	Tcl_SetResult(interp,puf,TCL_VOLATILE);
	return TCL_OK;
	break;
      }
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      {
	((matrix<widefloat> *)pmatrix)->operator()(y,x)=widefloat(argv[2]);
	string *s=((matrix<widefloat> *)pmatrix)->
	  operator()(y,x).tostring(
				   config.widefloat_reformat,
				   config.widefloat_showprec,
				   config.widefloat_maxnum,
				   config.widefloat_maxzeros
				   );
	Tcl_SetResult(interp,(char *)s->c_str(),TCL_VOLATILE);
	delete s;
	return TCL_OK;
	break;
      }
#endif
    }
  } else  {
    Tcl_AddErrorInfo(interp,"need 3 arguments col ,row and number");
    return TCL_ERROR;
  }
}
int matrix_tcl::getelem(Tcl_Interp *interp,int argc,const char *argv[], matrixtcl_config &config) {
  if (argc>=2) {
    int x,y;
    if (Tcl_GetInt(interp,argv[0],&y)==TCL_ERROR || 
	Tcl_GetInt(interp,argv[1],&x)==TCL_ERROR) {
      Tcl_AddErrorInfo(interp,"cant read col or row numbers");
      return TCL_ERROR;
    }
    if (y>=pmatrix->mhoehe() || x>=pmatrix->nbreite() || x<0 || y<0) {
      Tcl_AddErrorInfo(interp,"col or row numbers not in matrix range");
      return TCL_ERROR;
    }
    switch(pmatrix->whoamI()) {
    case MatrixObject::trational:
      {
	string *s;
	if (argc==3 && strcmp(argv[2], "-onlyfraction") == 0)
	  s=((matrix<rational> *)pmatrix)->operator()(y,x).tostring(false);
	else
	  s=((matrix<rational> *)pmatrix)->operator()(y,x).tostring(true);
	for (int i=0;i<s->length();i++) {
	  if (s->at(i)==',') s->at(i)=' ';
	}
	// const char *c=s->c_str();
	Tcl_SetResult(interp,(char *)s->c_str(),TCL_VOLATILE);
	delete s;
	return TCL_OK;
      }
      break;
    case MatrixObject::tfloat:
      {
	// fprintf(stderr,"Getting float\n");
	double d=((matrix<double> *)pmatrix)->operator()(y,x);
	char puf[30];
	sprintf(puf,"%lf",d);
	Tcl_SetResult(interp,puf,TCL_VOLATILE);
	return TCL_OK;
      }
      break;
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      {
	// fprintf(stderr,"Getting float\n");
	string *s=((matrix<widefloat> *)pmatrix)->
	  operator()(y,x).tostring(
				   config.widefloat_reformat,
				   config.widefloat_showprec,
				   config.widefloat_maxnum,
				   config.widefloat_maxzeros
				   );
	Tcl_SetResult(interp,(char *)s->c_str(),TCL_VOLATILE);
	delete s;
	return TCL_OK;
      }
      break;
#endif 
    }
  } else {
    Tcl_AddErrorInfo(interp,"need col and row");
    return TCL_ERROR;
  }
}
int matrix_tcl::setsolution(Tcl_Interp *interp,int argc,const char *argv[]) {
  if (argc!=1) {
    Tcl_AddErrorInfo(interp,"syntax setsolution matrixlist");
    return TCL_ERROR;
  }
  MatrixObject *m=list2matrix(interp,argv[0]);
  if (m) {
    if (smatrix) delete smatrix;
    smatrix=m;
  } else {
    return TCL_ERROR;
  }
  return TCL_OK;
}
int matrix_tcl::cmd_addprot(Tcl_Interp *interp,int argc,const char *argv[]) {
  // Ausgang Matrix zu protokoll hinzufgen
  if (argc==1 && strcmp(argv[0],"editmatrix")==0) {
    addProt(pmatrix,MatrixObject::tmatrix);
    return TCL_OK;
  }
  if (argc!=2) {
    Tcl_AddErrorInfo(interp,"syntax addprot prottype objectlist");
    return TCL_ERROR;
  }
  int t;
  for (t=0;t<ptype_count;t++) {
      if (strcmp(argv[0],prottypen[t])==0) break;
  }
  if (t==ptype_count) {
    Tcl_AddErrorInfo(interp,"prot types only matrix gleichung simplex loesung determinante polynom string")
      ;
    return TCL_ERROR;
  }
  if (t<ptype_count-1) {
    MatrixObject *m=list2matrix(interp,argv[1]);
    if (m) {
      addProt(m,(MatrixObject::KontextType) t);
    } else {
      return TCL_ERROR;
    }
  } else {
    pmatrix->prot<<argv[1]<<"\n";
  }
  return TCL_OK;
}
int matrix_tcl::deleteprot(Tcl_Interp *interp,int argc,const char *argv[]) {
  if (argc!=0) {
    Tcl_AddErrorInfo(interp,"syntax deleteprot");
    return TCL_ERROR;
  }
  pmatrix->prot.leereprot();
  return TCL_OK;
}
int matrix_tcl::resize(Tcl_Interp *interp,int argc,const char *argv[]) {
  if (argc!=4) {
    Tcl_AddErrorInfo(interp,"syntax resize newcol newrow fromlink(bool) fromtop(bool)");
    return TCL_ERROR;
  }
  int newcol,newrow,fromlink,fromtop;
  if (Tcl_GetInt(interp,argv[0],&newrow)==TCL_ERROR || 
      Tcl_GetInt(interp,argv[1],&newcol)==TCL_ERROR ||
      Tcl_GetBoolean(interp,argv[2],&fromtop)==TCL_ERROR ||
      Tcl_GetBoolean(interp,argv[3],&fromlink)==TCL_ERROR) {
    Tcl_AddErrorInfo(interp,"cant read argument properly");
    return TCL_ERROR;
  }
  pmatrix->resize(newrow,newcol,fromlink,fromtop);
  return TCL_OK;
}
int matrix_tcl::save(Tcl_Interp *interp,const char *filename,LoadType t) {
  ofstream wy (filename,ios::out);
  if (wy) {
    switch (t) {
    case tkmatrix:
      switch (pmatrix->whoamI()) {
      case MatrixObject::tfloat:
	//  wy<<"FLOAX";
	//  wy<<m.mhoehe()<<'\t'<<m.nbreite()<<'\n';
	//  static_cast<matrix<double> &>(m).printDigit(wy);
	wy<<"FLOAT\n"<<pmatrix->mhoehe()<<'\t'<<pmatrix->nbreite()<<'\n';
	pmatrix->Store(wy);
	break;
#if !defined(NO_GMP)
      case MatrixObject::twidefloat:
	//  wy<<"FLOAX";
	//  wy<<m.mhoehe()<<'\t'<<m.nbreite()<<'\n';
	//  static_cast<matrix<double> &>(m).printDigit(wy);
	wy<<"WFLOAT\n"<<pmatrix->mhoehe()<<'\t'<<pmatrix->nbreite()<<'\n';
	pmatrix->Store(wy);
	break;
#endif
      case  MatrixObject::trational:
	wy<<pmatrix->mhoehe()<<'\t'<<pmatrix->nbreite()<<'\n';
	pmatrix->Store(wy);
	break;
      }
      break;
    case octavebin:
      save_octavebin(wy);
      break;
    case mathlabbin:
      save_mathlabbin(wy);
      break;
    }
    wy.close();
  } else {
    Tcl_AddErrorInfo(interp,"can`t open file for save the matrix");
    return TCL_ERROR;
  }
  return TCL_OK;
}
MatrixObject *matrix_tcl::dupplicate() {
  return DuplicateMatrixObject(pmatrix);
}
MatrixObject *matrix_tcl::givesolution() {
  if (smatrix) 
    return DuplicateMatrixObject(smatrix);
  else
    return 0;
}
MatrixObject *matrix_tcl::tofloat(Tcl_Interp *interp,int argc,const char *argv[]) {
  if (argc==0) {
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      {
	MatrixObject *m=new matrix<double>(pmatrix->mhoehe(),pmatrix->nbreite(),* new protstream());
	for (int x=0;x<pmatrix->nbreite();x++)
	  for (int y=0;y<pmatrix->mhoehe();y++) {
	    ((matrix<double> *)m)->operator()(y,x)=
	      ((matrix<rational> *)pmatrix)->operator()(y,x).todouble();
	  }
	return m;
      }
    case MatrixObject::tfloat:
      return DuplicateMatrixObject(pmatrix);
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:  
      {
	MatrixObject *m=new matrix<double>(pmatrix->mhoehe(),pmatrix->nbreite(),* new protstream());
	for (int x=0;x<pmatrix->nbreite();x++)
	  for (int y=0;y<pmatrix->mhoehe();y++) {
	    ((matrix<double> *)m)->operator()(y,x)=
	      ((matrix<widefloat> *)pmatrix)->operator()(y,x).todouble();
	  }
	return m;
      }
#endif
    }
  } 
#if !defined(NO_GMP)
  else if (argc==1 && strcmp(argv[0],"-widefloat")==0) {
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      {
	MatrixObject *m=new matrix<widefloat>(pmatrix->mhoehe(),pmatrix->nbreite(),* new protstream());
	for (int x=0;x<pmatrix->nbreite();x++)
	  for (int y=0;y<pmatrix->mhoehe();y++) {
	    ((matrix<widefloat> *)m)->operator()(y,x)=
	      ((matrix<rational> *)pmatrix)->operator()(y,x);
	  }
	return m;
      }
    case MatrixObject::twidefloat:
      return DuplicateMatrixObject(pmatrix);
    case MatrixObject::tfloat:  
      {
	MatrixObject *m=new matrix<widefloat>(pmatrix->mhoehe(),pmatrix->nbreite(),* new protstream());
	for (int x=0;x<pmatrix->nbreite();x++)
	  for (int y=0;y<pmatrix->mhoehe();y++) {
	    ((matrix<widefloat> *)m)->operator()(y,x)=
	      ((matrix<double> *)pmatrix)->operator()(y,x);
	  }
	return m;
      }
    }
#endif
    return 0;
  }
}
MatrixObject *matrix_tcl::DuplicateMatrixObject(MatrixObject *mo) {
  return mo->clone();
}
MatrixObject *matrix_tcl::copy(Tcl_Interp *interp,int argc,const char *argv[]) {
  int x1,y1,x2,y2;
  if (argc==4) {
    if (Tcl_GetInt(interp,argv[0],&y1)==TCL_ERROR || 
	Tcl_GetInt(interp,argv[1],&x1)==TCL_ERROR ||
	Tcl_GetInt(interp,argv[2],&y2)==TCL_ERROR || 
	Tcl_GetInt(interp,argv[3],&x2)==TCL_ERROR) {
      Tcl_AddErrorInfo(interp,"resize need integers alrguments");
      return 0;
    }
    MatrixObject *mo;
    switch (pmatrix->whoamI()) {
    case MatrixObject::trational:
      // return new matrix<rational>(*(dynamic_cast<matrix<rational> *>(mo)));
      return new matrix<rational>(*((matrix<rational> *)(pmatrix)),x1,y1,x2,y2);
    case MatrixObject::tfloat:
      // return new matrix<double>(*(dynamic_cast<matrix<double> *>(mo)));
      return new matrix<double>(*((matrix<double> *)(pmatrix)),x1,y1,x2,y2);
#if !defined(NO_GMP)
    case MatrixObject::twidefloat:
      // return new matrix<double>(*(dynamic_cast<matrix<double> *>(mo)));
      return new matrix<widefloat>(*((matrix<widefloat> *)(pmatrix)),x1,y1,x2,y2);
#endif
    default: return 0;
    }
  }
  Tcl_AddErrorInfo(interp,"false num of argument: y1 x1 y2 x2");
  return 0;
}
void matrix_tcl::VorbereiteAlgorithmus() {
  if (smatrix) delete smatrix;
  smatrix=pmatrix->clone(true);
  pmatrix->prot.leereprot();
}
void matrix_tcl::addProt(MatrixObject *m,MatrixObject::KontextType kt) {
  switch (m->whoamI()) {
  case MatrixObject::trational:
    // return new matrix<rational>(*(dynamic_cast<matrix<rational> *>(m)));
    {
    m->prot.add(*((matrix<rational> *)(m)),kt);
    }
    break;
  case MatrixObject::tfloat:
    {
    m->prot.add(*((matrix<double> *)(m)),kt);
    }
    break;
  case MatrixObject::twidefloat:
    {
    m->prot.add(*((matrix<widefloat> *)(m)),kt);
    }
    break;
  }
}
// This Funktion make from tcl list an matrix object
// the type is given by pmatrix type
// {{32 23} {32 23} {43 13}} this is a 3*2 Matrix of float
// 3= height 2= bright
// {{{1 2 3} 23} {32 23} {43 13}} this is a 3*2 Matrix of float
// now is first element a fraction 1+2/3
MatrixObject *matrix_tcl::list2matrix(Tcl_Interp *interp,const char *list) {
  int listArgc,zeileArgc,nextArgc,bruchArgc;
  const char **listArgv,**zeileArgv,**bruchArgv;
  MatrixObject *ret;
  if (Tcl_SplitList(interp,list,&listArgc,&listArgv)==TCL_ERROR) {
    return 0;
  }
  if (listArgc==0) {
    Tcl_AddErrorInfo(interp," empty matrix list");
    return 0;
  }
  // cerr<<"list argc "<<listArgc<<endl;
  if (Tcl_SplitList(interp,listArgv[0],&zeileArgc,&zeileArgv)==TCL_ERROR) {
    return 0;
  }
  // cerr<<"list argc "<<zeileArgc <<endl;
  // cout<<"list2matrix "<<listArgc<<" * "<<zeileArgc<<endl;
  switch (pmatrix->whoamI()) {
  case MatrixObject::trational:
    ret=new matrix<rational>(listArgc,zeileArgc,pmatrix->prot);
    break;
  case MatrixObject::tfloat:
    ret=new matrix<double>(listArgc,zeileArgc,pmatrix->prot);
    break;
#if !defined(NO_GMP)
  case MatrixObject::twidefloat:
    ret=new matrix<widefloat>(listArgc,zeileArgc,pmatrix->prot);
    break;
#endif
  }
  double d;
  int gzahl,zaehler,nenner,errorflag=false;
  for (int y=0;y<listArgc;y++) {
    for (int x=0;x<zeileArgc;x++) {
      switch (pmatrix->whoamI()) {
      case MatrixObject::trational: {
	if (Tcl_SplitList(interp,zeileArgv[x],&bruchArgc,&bruchArgv)==TCL_ERROR) {
          delete ret;
	  return 0;
	}
	switch (bruchArgc) {
	case 1:
	  if (Tcl_GetInt(interp,bruchArgv[0],&gzahl)==TCL_ERROR) {
	    errorflag=true;
	    break;
	  }
            ((matrix<rational> *)ret)->operator()(y,x)=rational(gzahl);
            break;
	case 2:
        if (Tcl_GetInt(interp,bruchArgv[0],&gzahl)==TCL_ERROR ||
            Tcl_GetInt(interp,bruchArgv[1],&zaehler)==TCL_ERROR) {
	    errorflag=true;
	    break;
	  }
	  ((matrix<rational> *)ret)->operator()(y,x)=rational(gzahl,zaehler);
	  break;
	case 3:
	  if (Tcl_GetInt(interp,bruchArgv[0],&gzahl)==TCL_ERROR ||
	      Tcl_GetInt(interp,bruchArgv[1],&zaehler)==TCL_ERROR ||
	      Tcl_GetInt(interp,bruchArgv[2],&nenner)==TCL_ERROR) {
	    errorflag=true;
	    break;
	  }
	  ((matrix<rational> *)ret)->operator()(y,x)=rational(gzahl,zaehler,nenner);
	  break;
	default:
	  errorflag=true;
	  break;
	}
	free ((char *) bruchArgv);
	if (errorflag) {
 	  Tcl_AddErrorInfo(interp,"cant read rational number");
          delete ret;
	  return 0;
	}
        break;
      }
      case MatrixObject::tfloat:
        if (Tcl_GetDouble(interp,zeileArgv[x],&d)==TCL_ERROR) {
          Tcl_AddErrorInfo(interp,"cant read double number");
          delete ret;
          return 0;
        }
        (* (matrix<double> *)ret)(y,x)=d;
        break;
#if !defined(NO_GMP)
      case MatrixObject::twidefloat:
        (* (matrix<widefloat> *)ret)(y,x)=widefloat(zeileArgv[x]);
        break;
#endif
      }   
    }  
    free ((char *) zeileArgv); 
    if (y<listArgc-1) {
      if (Tcl_SplitList(interp,listArgv[y+1],&nextArgc,&zeileArgv)==TCL_ERROR) {
        delete ret;
        return 0;
      }
      if (nextArgc!=zeileArgc) {
	Tcl_AddErrorInfo(interp," bad matrix list");
	delete ret;
	return 0;
      }
    }
  }
  return ret;  
}
//
// This code is based on octave source
// file load-save.cc
// matrix can be loaded only as double float values 
// convesrion of float representation are not supported
// 
enum save_type {
  LS_U_CHAR,
  LS_U_SHORT,
  LS_U_INT,
  LS_CHAR,
  LS_SHORT,
  LS_INT,
  LS_FLOAT,
  LS_DOUBLE
};
enum float_format {
  native,
  unknown,
  ieee_little_endian,
  ieee_big_endian,
  vax_d,
  vax_g,
  cray
};

void matrix_tcl::read_octavebin (istream &is) {
  char tmp = 0;
  int len = 0;
  int magic_len = 10;
  char magic [magic_len+1];
  is.read (magic, magic_len);
  magic[magic_len] = '\0';
  if (strncmp (magic, "Octave-1-L", magic_len) != 0)
    goto data_read_error;
	
  is.read (&tmp, 1);
  if (tmp!=0) 
    throw badformat(string("only ieee little endian float values can be read"));

  if (!is.read ((char *)&len, 4))
     goto data_read_error;
  // ignore variable name
  if (!is.ignore(len))
    goto data_read_error;
  if (!is.read ((char *)&len, 4))
    goto data_read_error;
  // ignore doc
  if (!is.ignore(len))
    goto data_read_error;
  //  global flag
  if (! is.read ((char *)&tmp, 1))
    goto data_read_error;
  tmp = 0;
  // data type
  if (! is.read ((char *)&tmp, 1))
    goto data_read_error;
  
  switch (tmp) {
  case 2:
    int nr, nc;
    if (! is.read ((char *)&nr, 4))
      goto data_read_error;
    if (! is.read ((char *)&nc, 4))
	goto data_read_error;
    if (! is.read ((char *)&tmp, 1))
      goto data_read_error;
    // number type
    if (tmp!=LS_DOUBLE)
      throw badformat(string("only double float values can be read"));
    pmatrix = new matrix<double>(nr, nc, *new protstream());
    double d;
    for(int x=0;x<nc;x++)
      for(int y=0;y<nr;y++) {
	if (!is.read((char *)&d,8))
	  goto data_read_error;
	(* (matrix<double> *)pmatrix)(y,x)=d;
      }
    break;
    default:
    data_read_error:
    if (pmatrix) {
      delete &(pmatrix->prot);
      delete pmatrix;
    }
    throw badformat(string("cant load matrix as octave binary file"));
    break;
  }
}
void matrix_tcl::read_mathlabbin (istream &is) {
  int mopt,nr,nc,imag,len;
  int type = 0, prec = 0, order = 0, mach = 0, dlen = 0;
  
  if (! is.read((char *)&mopt, 4))
    goto data_read_error;

  if (! is.read ((char *)&nr, 4))
    goto data_read_error;

  if (! is.read ((char *)&nc, 4))
    goto data_read_error;

  if (! is.read ((char *)&imag, 4))
    goto data_read_error;

  if (! is.read ((char *)&len, 4))
    goto data_read_error;

  if (mopt > 9999 || mopt < 0 || imag > 1 || imag < 0)
    goto data_read_error;

  type = mopt % 10;  // Full, sparse, etc.
  mopt /= 10;        // Eliminate first digit.
  prec = mopt % 10;  // double, float, int, etc.
  mopt /= 10;        // Eliminate second digit.
  order = mopt % 10; // Row or column major ordering.
  mopt /= 10;        // Eliminate third digit.
  mach = mopt % 10;  // IEEE, VAX, etc.

  if (mach != 0)
    goto data_read_error;

  if (type != 0 && type != 1)
    goto data_read_error;
  
  if (imag || type == 1)
    goto data_read_error;

  if (prec != 0)     // only double float
    goto data_read_error;

  // LEN includes the terminating character, and the file is also
  // supposed to include it, but apparently not all files do.  Either
  // way, I think this should work.

  if (! is.ignore (len))

  dlen = nr * nc;
  if (dlen < 0)
    goto data_read_error;

  if (order) {
    int tmp = nr;
    nr = nc;
    nc = tmp;
  }

  pmatrix=new matrix<double>(nr,nc, *new protstream());

  double d;
  if (order) {
    for(int y=0;y<nr;y++)
      for(int x=0;x<nc;x++) {
	if (!is.read((char *)&d,8))
	  goto data_read_error;
	(* (matrix<double> *)pmatrix)(y,x)=d;
      }
  } else {
    for(int x=0;x<nc;x++)
      for(int y=0;y<nr;y++) {
	if (!is.read((char *)&d,8))
	  goto data_read_error;
	(* (matrix<double> *)pmatrix)(y,x)=d;
      }
  }

  return;

 data_read_error:
  if (pmatrix) {
    delete &(pmatrix->prot);
    delete pmatrix;
  }
  throw badformat(string("cant load matrix as MathLab binary file"));
  return;
}
void matrix_tcl::save_octavebin (ostream &os) {
  // Magic Bytes
  int temp;
  os<<"Octave-1-L";
  // ieee little endian float type
  os.put(0);
  char *name="tkmatrix";
  temp=strlen(name);
  os.write((char *)&temp,4);
  os<<name;
  // no doc
  temp=0;
  os.write((char *)&temp,4);
  // global floag
  os.put(0);
  // matrix type
  os.put(2);
  int rn=pmatrix->mhoehe(),cn=pmatrix->nbreite();
  os.write((char *)&rn,4);
  os.write((char *)&cn,4);
  // DOUBLE type
  os.put((char)LS_DOUBLE);
  double d;
  for (int x=0;x<cn;x++)
    for (int y=0;y<rn;y++) {
      switch(pmatrix->whoamI()) {
      case MatrixObject::trational:
	d=((matrix<rational> *)pmatrix)->operator()(y,x).todouble();
	break;
      case MatrixObject::tfloat:
	d=((matrix<double> *)pmatrix)->operator()(y,x);
#if !defined(NO_GMP)
      case MatrixObject::twidefloat:
	d=((matrix<widefloat> *)pmatrix)->operator()(y,x).todouble();
#endif 
      }
      os.write((char *)&d,8);
    }
}
void matrix_tcl::save_mathlabbin (ostream &os) {
  int temp=0;
  int rn=pmatrix->mhoehe(),cn=pmatrix->nbreite();
  // mopt
  os.write((char *)&temp,4);
  // height and width
  os.write((char *)&rn,4);
  os.write((char *)&cn,4);
  // imag
  os.write((char *)&temp,4);
  // namelen and len with ending \0
  char *name="tkmatrix";
  temp=strlen(name)+1;
  os.write((char *)&temp,4);
  os<<name;
  os.put(0);
  
  double d;
  for (int x=0;x<cn;x++)
    for (int y=0;y<rn;y++) {
      switch(pmatrix->whoamI()) {
      case MatrixObject::trational:
	d=((matrix<rational> *)pmatrix)->operator()(y,x).todouble();
	break;
      case MatrixObject::tfloat:
	d=((matrix<double> *)pmatrix)->operator()(y,x);
#if !defined(NO_GMP)
      case MatrixObject::twidefloat:
	d=((matrix<widefloat> *)pmatrix)->operator()(y,x).todouble();
#endif 
      }
      os.write((char *)&d,8);
    }
  
} 
