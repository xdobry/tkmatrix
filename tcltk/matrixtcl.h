#if !defined(matrixtcl_class)
#define matrixtcl_class

#include <tcl.h>
// #include <errno.h>
// #include <assert.h>
#include <string.h>


//#include <fstream.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "matrix3.h"

using namespace std;

// #include <std/list> bereits in protstream.h

struct matrixtcl_config {
  int noprot;
  int onlyfraction;
  char *dir;
  int widefloat_prec;
  int widefloat_maxnum;
  int widefloat_showprec;
  int widefloat_reformat;
  int widefloat_maxzeros;
  matrixtcl_config():noprot(0),onlyfraction(0),dir(0),widefloat_prec(0),
    widefloat_maxnum(0),widefloat_showprec(0),widefloat_reformat(false),
    widefloat_maxzeros(0) {}
};


class matrix_tcl {
private:
  MatrixObject *DuplicateMatrixObject(MatrixObject *mo);
  void VorbereiteAlgorithmus();
  void addProt(MatrixObject *m,MatrixObject::KontextType kt);
  MatrixObject *list2matrix(Tcl_Interp *interp,const char *list);
  void read_octavebin(istream &);
  void read_mathlabbin(istream &);
  void save_octavebin(ostream &);
  void save_mathlabbin(ostream &);
  MatrixObject *pmatrix; // primary matrix (input for algortihmus)
  MatrixObject *smatrix; // solution matrix
public:
  enum LoadType{tkmatrix,octavebin,mathlabbin};
  matrix_tcl(MatrixObject *mo);
  matrix_tcl(int row,int col,MatrixObject::MatrixType mtype);
  matrix_tcl(const char *filename,LoadType t=tkmatrix);
  ~matrix_tcl();

  int algorithmus(Tcl_Interp *interp,int argc,const char *argv[]);
  int elemunf(Tcl_Interp *interp,int argc,const char *argv[]);
  //  int undo(Tcl_Interp *interp,int argc,char *argv[]);

  int operation(Tcl_Interp *interp,matrix_tcl *matrixtcl2,int argc,const char *argv[]);
  
  int info(Tcl_Interp *interp,const char *typ);
  int getform(Tcl_Interp *interp,protformat *pformat,const char *option);

  void setprotstat(bool noprot);
  
  int setelem(Tcl_Interp *interp,int argc,const char *argv[], matrixtcl_config &);
  int getelem(Tcl_Interp *interp,int argc,const char *argv[], matrixtcl_config &);
  int resize(Tcl_Interp *interp,int argc,const char *argv[]);
  int setsolution(Tcl_Interp *interp,int argc,const char *argv[]);
  int cmd_addprot(Tcl_Interp *interp,int argc,const char *argv[]);
  int deleteprot(Tcl_Interp *interp,int argc,const char *argv[]);

  int save(Tcl_Interp *interp,const char *filename,LoadType t=tkmatrix);
  // Dupplicate MatrixObject
  MatrixObject *copy(Tcl_Interp *interp,int argc,const char *argv[]);
  MatrixObject *dupplicate();
  MatrixObject *givesolution();
  MatrixObject *tofloat(Tcl_Interp *interp,int argc,const char *argv[]); 
};

//
// matrixtcl object configure procedures was adapted from
// tkConfig.c file tk distribution
//

typedef struct matrix_ConfigSpec {
    int type;			/* Type of option, such as MATRIX_CONFIG_COLOR;
				 * see definitions below.  Last option in
				 * table must have type MATRIX_CONFIG_END. */
    char *argvName;		/* Switch used to specify option in argv.
				 * NULL means this spec is part of a group. */
    char *dbName;		/* Name for option in option database. */
  //    char *dbClass;		/* Class for option in database. */
    char *defValue;		/* Default value for option if not
				 * specified in command line or database. */
    int offset;			/* Where in widget record to store value;
				 * use matrix_Offset macro to generate values
				 * for this. */
  int specFlags;		/* Any combination of the values defined */
} matrix_ConfigSpec;

/*
 * Type values for matrix_ConfigSpec structures.  See the user
 * documentation for details.
 */

#define MATRIX_CONFIG_BOOLEAN	1
#define MATRIX_CONFIG_INT	2
#define MATRIX_CONFIG_DOUBLE	3
#define MATRIX_CONFIG_STRING	4
#define MATRIX_CONFIG_END	22

#define MATRIX_CONFIG_OPTION_SPECIFIED  0x10
/*
 * Macro to use to fill in "offset" fields of matrix_ConfigInfos.
 * Computes number of bytes from beginning of structure to a
 * given field.
 */

#ifdef offsetof
#define matrix_Offset(type, field) ((int) offsetof(type, field))
#else
#define matrix_Offset(type, field) ((int) ((char *) &((type *) 0)->field))
#endif


class Manager_matrix {
private:
  Tcl_HashTable matrixTable;
  Tcl_HashTable formTable;
  string dir;
  int matrixcount;
  matrixtcl_config config;
public:
  Manager_matrix();
  ~Manager_matrix();
  void create(Tcl_Interp *interp,const char *row,const char *col,const char *isfloat);
  void load(Tcl_Interp *interp,const char *filename,matrix_tcl::LoadType t=matrix_tcl::tkmatrix);
  void add(Tcl_Interp *interp,matrix_tcl *m);
  void del(const char *mname);
  matrix_tcl *find(const char *mname);
  protformat *getprot(const char *pname);
  //  void setpath(char *pdir);
  const char *getpath();
  int info (Tcl_Interp *interp);
  int handlematrix(Tcl_Interp *interp,int argc,const char *argv[]);

  int Configure(Tcl_Interp *interp,int argc,const char **argv);
  int ConfigureInfo(Tcl_Interp *interp,const char *argvName);
private:
  int matrix_Configure(Tcl_Interp *interp,
		       matrix_ConfigSpec *specs,
		       int argc,const char **argv,char *widgRec);
  int matrix_ConfigureInfo(Tcl_Interp *interp,matrix_ConfigSpec *specs,
			   char *widgRec,const char *argvName);
  matrix_ConfigSpec * FindConfigSpec(Tcl_Interp *interp,
				     matrix_ConfigSpec *specs,
				     const char *argvName);
  int DoConfig (Tcl_Interp *interp,matrix_ConfigSpec *specPtr,
		const char *value, char *widgRec);
  char * FormatConfigInfo (Tcl_Interp *interp, 
			   matrix_ConfigSpec *specPtr,char *widgRec);
  char * FormatConfigValue (Tcl_Interp *interp,matrix_ConfigSpec *specPtr,
			    char *widgRec, char *buffer,
			    Tcl_FreeProc **freeProcPtr);
  int matrix_ConfigureValue(Tcl_Interp *interp,
			    matrix_ConfigSpec *specs,
			    char *widgRec,char *argvName);
  void matrix_FreeOptions( matrix_ConfigSpec *specs,char *widgRec);
};

#endif



