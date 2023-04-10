#if !defined(protview_class)
#define protview_class
#ifdef DEBUG
#include <stdio.h>
#endif

#include "protformat.h"
#include "matrix3.h"

#include <ostream>
#include <sstream>
#include <list>

using namespace std;

// protstream ist ein Cointener von Objecten des Typs ProtBase
// die Klassen Hierarchie wird mit Templats aufgebaut (Is-Hold statt normallen Is-A Prinzip)
// diese Methode wird selten verwendet ist aber hier ntzlicher
// Klassendialgramm; -> bedeutet is abgeleitetet
// ProtTemplate<T> -> ProtBase
// matrixkx<type> -> matrix<type>      ;kx bedeutet Kontekst
// matrixfloatkx -> matrix<double>
// ProtTemplate kann die Objekte: matrixkx<type>, protstring, protstream, protscalar<type>
// behalten

void myform(ostream &,const char*,...);

template <class T>
class matrixkx:  public matrix<T> {
public:
  MatrixObject::KontextType kxt;
  matrixkx (const matrix<T> &mr,MatrixObject::KontextType kxt=MatrixObject::tmatrix);
  matrixkx (const matrixkx<T> &);
  matrixkx<T> & operator=(const matrixkx<T>&);

  void PrintOn(ostream &,protformat &)const;

  virtual ~matrixkx();
};

template <class T>
matrixkx<T>::matrixkx(const matrixkx<T>& m):matrix<T>(m) {
  kxt=m.kxt;
}
template <class T>
matrixkx<T>& matrixkx<T>::operator=(const matrixkx<T>& mr) {
  if (this==&mr) {
    return *this;
  }
  matrix<T>::operator= (mr);
  kxt=mr.kxt;
  return *this;
}
template <class T>
matrixkx<T>::matrixkx (const matrix<T> &mr,MatrixObject::KontextType kxt):matrix<T>(mr),kxt(kxt) {}
template <class T>
matrixkx<T>::~matrixkx() {}
//
// Vorsicht fr alle Benutzten Typen mu�die Funktion 
// bool printtyp (Typ,   ostream &wy
//		 protformat::tformobject fobject,int firststring,
//		 protformat &pformat,int index=0,bool nozero=false,
//		 bool alwayssign=false
//		 )
// definiert werden
//
bool printtype(double d,ostream &wy,
	       protformat::tformobject fobject,int firststring,
	       protformat &pformat,int index=0,bool nozero=false,
	       bool alwayssign=false,bool ignoreone=false
	       );
bool printtype(const rational &r,ostream &wy,
	       protformat::tformobject fobject,int firststring,
	       protformat &pformat,int index=0, bool nozero=false,
	       bool alwayssign=false,bool ignoreone=false
	       );
#if !defined(NO_GMP)
bool printtype(const widefloat &wf,ostream &wy,
	       protformat::tformobject fobject,int firststring,
	       protformat &pformat,int index=0, bool nozero=false,
	       bool alwayssign=false,bool ignoreone=false
	       );
int makeindex(const widefloat &wf);
#endif
int makeindex(const rational &r);
int makeindex(double d);

template <class T>
void matrixkx<T>::PrintOn(ostream &wy,protformat &pformat)const {
  int x,y;
  // double d;
  switch(kxt) {
    // Matrix als Matrix
  case MatrixObject::tmatrix:
    if (pformat.form_exist(protformat::fmatrix)) {
      wy<<pformat.givefstring(protformat::fmatrix,0);
      for (x=0;x<this->nbreite();x++) wy<<pformat.givefstring(protformat::fmatrix,1);
      wy<<pformat.givefstring(protformat::fmatrix,2);
      for(y=0;y<this->mhoehe();y++) {
	wy<<pformat.givefstring(protformat::fmatrix,3);
	for (x=0;x<this->nbreite();x++) {
	  printtype((*this)(y,x),wy,protformat::fmatrix,4,pformat);
	  if (x!=this->nbreite()-1) {
	    wy<<pformat.givefstring(protformat::fmatrix,8);
	  }
	}
        if (y!=this->mhoehe()-1)
	  wy<<pformat.givefstring(protformat::fmatrix,9);
      }
      wy<<pformat.givefstring(protformat::fmatrix,10);
    } break;
// Matrix als lineares Gleichungssystem
  case MatrixObject::tgleichung:
    if (pformat.form_exist(protformat::feqnarray)){
      wy<<pformat.givefstring(protformat::feqnarray,0);
      for(y=0;y<this->mhoehe();y++) {
	wy<<pformat.givefstring(protformat::feqnarray,1);
	bool wassign=false;
	for (x=0;x<this->nbreite()-1;x++) {
	  wassign|=printtype((*this)(y,x),wy,protformat::feqnarray,2,pformat,x,true,wassign);
	  if (x!=this->nbreite()-2) {
	    wy<<pformat.givefstring(protformat::feqnarray,6);
	  }
	}
	// Wenn linke Seite leer dann eine Null
	if (!wassign) {
	  printtype((*this)(y,x-1),wy,protformat::feqnarray,8,pformat);
	}
	wy<<pformat.givefstring(protformat::feqnarray,7);
	printtype((*this)(y,x),wy,protformat::feqnarray,8,pformat);
	if (y!=this->mhoehe()-1) 
	  wy<<pformat.givefstring(protformat::feqnarray,12);
      }
      wy<<pformat.givefstring(protformat::feqnarray,13);
    } break;
// Matrix als ungleichungsystem und eine zu Maximierende funktion
  case MatrixObject::tsimplex:
    if (pformat.form_exist(protformat::fsimplex)){
      wy<<pformat.givefstring(protformat::fsimplex,0);
      for (x=0;x<this->nbreite();x++) wy<<pformat.givefstring(protformat::fsimplex,1);
      wy<<pformat.givefstring(protformat::fsimplex,2);
      for(y=0;y<this->mhoehe()-1;y++) {
	wy<<pformat.givefstring(protformat::fsimplex,3);
	for (x=0;x<this->nbreite();x++) {
	  printtype((*this)(y,x),wy,protformat::fsimplex,4,pformat,x);
	  if (x!=this->nbreite()-1) {
	    wy<<pformat.givefstring(protformat::fsimplex,8);
	  }
	}
	if (y!=this->mhoehe()-2)
	  wy<<pformat.givefstring(protformat::fsimplex,9);
      }
      wy<<pformat.givefstring(protformat::fsimplex,10);
      printtype((*this)(this->mhoehe()-1,this->nbreite()-1),wy,protformat::fsimplex,4,pformat,x,true);
      for (x=0;x<this->nbreite()-1;x++) {
	printtype((*this)(y,x),wy,protformat::fsimplex,11,pformat,x,true,true);
      }
      wy<<pformat.givefstring(protformat::fsimplex,15);
    } break;
// Matrix als L�ung eines Gleichungssystem 
  case MatrixObject::tloesung:
    if (pformat.form_exist(protformat::fsolution)){
      wy<<pformat.givefstring(protformat::fsolution,0);
      for (x=0;x<this->nbreite();x++) {
        if (x>0) {
            myform(wy,pformat.givefstring(protformat::fsolution,7),x,true);
        }
	wy<<pformat.givefstring(protformat::fsolution,1);
	for (y=0;y<this->mhoehe();y++) {
	  printtype((*this)(y,x),wy,protformat::fsolution,2,pformat);
	  if (y!=this->mhoehe()-1) wy<<pformat.givefstring(protformat::fsolution,6);
	}
	wy<<pformat.givefstring(protformat::fsolution,8);
      }
      if (this->nbreite()>1) 
	wy<<pformat.givefstring(protformat::fsolution,9);
      else
        wy<<pformat.givefstring(protformat::fsolution,10);
    } break;
// Matrix 1*1 als Determinante
  case MatrixObject::tdeterminante:
    if (pformat.form_exist(protformat::fdeterminante)){
       wy<<pformat.givefstring(protformat::fdeterminante,0);
       printtype((*this)(0,0),wy,protformat::fdeterminante,1,pformat);
       wy<<pformat.givefstring(protformat::fdeterminante,5);
    } break;
// Matrix als polynom
  case MatrixObject::tpolynom:
    if (pformat.form_exist(protformat::fpolynom)){
      wy<<pformat.givefstring(protformat::fpolynom,0);
      for(x=0;x<this->nbreite();x++) {
        // y ist hie als index benutzt
        // T mu�zu double konventierbar sein 
        // d=(*this)(0,x);
        // Vorsicht m�liche "Rundungsfehler" 1 wird zu 0.999999 und zu 0
	y=makeindex((*this)(0,x));
        // fprintf(stderr,"poylnom step %d y=%d\n",x,y);
        if (y!=0) {
          printtype((*this)(1,x),wy,protformat::fpolynom,1,pformat,y,true,(x==0) ? false : true);
        } else {
          printtype((*this)(1,x),wy,protformat::fpolynom,5,pformat,y,true,true,true);
        }
      }
      wy<<pformat.givefstring(protformat::fpolynom,9);
    } break;
  }
}
// class fr Unterbringung von Skalaren in protstream
template <class T>
class protscalar {
  T scalar;
public:
  protscalar(const T &t);
  void PrintOn(ostream &,protformat &)const;
  //  ~protscalar();
};
// Implementierung von protscalar
template <class T>
protscalar<T>::protscalar(const T &t):scalar(t) {
}
template <class T>
void protscalar<T>::PrintOn(ostream &wy, protformat &pformat) const {
  if(pformat.form_exist(protformat::fscalar)) {
    printtype(scalar,wy,protformat::fscalar,0,pformat);
  }
}
// class fr Kommentare als ASCII Ketten
class protstring {
  string pstring;
public:
  protstring(string &s);
  protstring(char *c);
  protstring(const protstring&);
  void PrintOn(ostream &,protformat &)const;

  ~protstring() {}
};
inline  protstring::protstring(string &s):pstring(s) {}
inline  protstring::protstring(char *c):pstring(c) {}

class ProtBase {
public:
  virtual void PrintOn(ostream &,protformat &)const=0;
  virtual ~ProtBase() {}
};
template <class T>
class ProtTemplate: public ProtBase {
public:
  ProtTemplate(const T &o);
  virtual void PrintOn(ostream &os,protformat &)const;
  virtual ~ProtTemplate();
private:
  const T object;
};
#ifdef DEBUG
template <class T>
ProtTemplate<T>::ProtTemplate(const T &o):object(o) {
  fprintf(stderr,"ProtTemplate %p created\n",this);
}
template <class T>
ProtTemplate<T>::~ProtTemplate() {
  fprintf(stderr,"ProtTemplate %p deleted\n",this);    
}
#else
template <class T>
ProtTemplate<T>::ProtTemplate(const T &o):object(o) {}
template <class T>
ProtTemplate<T>::~ProtTemplate() {}
#endif
template <class T>
inline void ProtTemplate<T>::PrintOn(ostream &os,protformat &pformat)const {
  object.PrintOn(os,pformat);
}
class protstream {
private:
  list<ProtBase *> mylist;
  ostringstream zeile;
  char *errorInfo;
public:
  protstream(void);
  ~protstream(); // Liste  mu�gelert werden
  void PrintOn(ostream &,protformat &)const;
  void leereprot(void);
  bool empty();

  protstream& operator << (const char *);  // dateien werden zu prot geschrieben
  // \n darf nur am ande des String sein ; Es wird als neue Zeile interpretirt
  // und daraus ein neues protstring zu Liste  zugefgt
  protstream& operator << (matrix<rational>&);  // z.b prot<<matrixA<<"Das ist Matrix "<<12<<"\n";
  protstream& operator << (matrix<double>&);
  protstream& operator << (const double &);
  protstream& operator << (const rational &);
  void add(matrix<double>&,MatrixObject::KontextType);
  void add(matrix<rational>&,MatrixObject::KontextType);
#if !defined(NO_GMP)
  protstream& operator << (matrix<widefloat>&);
  protstream& operator << (const widefloat &);
  void add(matrix<widefloat>&,MatrixObject::KontextType);
#endif 
  protstream& operator << (int);       // nimmt alle Zeichen zu einen Puffer zeile bis \n angetroffen
  // wird, dann wird diese zeile ( ohne \n ) als protstring in die Liste angeh�gt und der Puffer wird fr n�hst eingabe vorbereitet
  void putError(char *);
  char *getError();

  bool noprot;
private:
  protstream(protstream&);
  protstream& operator= (protstream&);
};
#endif

















