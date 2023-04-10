#if !defined(matrix3_class)
#define matrix3_class

#ifdef DEBUG
#include <stdio.h>
#endif

// Deklarationen von sets,rational,matrix
// protstream wird deklariert vor Matrix wegen Rekursion
// Im Impementierungdatei mu�allerdings die volle Deklaration angebetet werden
#include <iostream> // Gebraucht von Rational und Matrix
// #include <cstring.> // funktioniert mit gnu nicht
// #include <strclass.h>
#include <string>
#include <utility>

#include "rational.h"
#if !defined(NO_GMP)
#include "widefloat.h"
#endif
#include "polynom.h"

using namespace std;
using namespace rel_ops;

class sets {
  long int *adr;
  int leng,pos;
public:
  sets(int=32);  // initialiesierung fr x elemente; vorsicht elemente  0..int-1
  sets(const sets &);	// Kopierconstruktor
  sets & operator = (const sets &);	// operator des Zuschreibens
  ~sets();		// werden von 0 nummeriert �n wie matrizen bei C
  void clear();  // alle l�chen
  void add(int);
  void del(int);
  void sxor(int);
  int exists(int) const;
  void init();		// initialiesirung des Iterationsverfahren
  int next();		// ob der element existiert
  int next_full();	// nr. der n�hsten existierenden Elementen, -1 keiner
  int all() const;			// ? alle Elemente wurden nachgefragt
  int howmany() const;		// wieviel Elemente
};


// Abstrakte klasse MatrixObject

class protstream; // Es handeld sich um eine rekursive deklaration

class MatrixObject {
#ifdef DEBUG
private:
  static int matrixcount;
  static int matrixlive;
  int matrixnum;
#endif
public:
  protstream &prot;  // Nicht so sauber aber was sonst
protected:
  MatrixObject(protstream &prot);
  int hoehe,breite;
public:
  enum MatrixType {trational,tfloat,twidefloat}; 
  enum KontextType {tmatrix,tgleichung,tsimplex,tloesung,tdeterminante,tpolynom};
  enum MyTEdge { NoEdge=-1,TopLeft=0,TopRight,BottomLeft,BottomRight };
  virtual MatrixType whoamI()=0;
  virtual ~MatrixObject();
  virtual MatrixObject *clone(bool withprot=false)=0; // Vorsicht: clone erzuegt ein neues protstream
  // Grundfunktinnen
  int nbreite(void)const;
  int mhoehe(void)const;
  // Algorithmen
  
  virtual void gauss(int *a=0)=0;
  virtual void gauss_jordan()=0;
  virtual void loesung()=0; // Vorsicht liefert nur richtiges Ergebniss wenn Matrix
  // in Treppen-Normall-Form (nach Gauss-Jordan)
  virtual void inverse()=0;
  virtual void bild()=0;
  virtual void kern()=0;
  virtual void determinante()=0;
  virtual int  eckenfindung()=0;
  virtual void optimiere()=0;
  virtual void naehrungsloesung()=0;
  virtual void chpol()=0;
  virtual void matrixspiel()=0;
  // Manipulation des Matrix
  virtual void transponiere()=0;
  virtual void resize(int neuehoehe,int neuebreite,bool linksveraendern,bool obenveraendern)=0;
  // elementare Unformungen (alle die sich mit Hilfe von virtaul wiedregeben lassen)
  virtual void swap(int,int,bool row)=0;
  virtual void pivotiere(int,int,bool row=true)=0;
  virtual void austauschschritt(int pzeile,int pspalte)=0;
  // Speichern und Wiederherstellen
  virtual void Store(ostream &)=0;
  virtual void Restore(istream &)=0;
  
  // Zugriff auf Elemente
  //  virtual char* ElemToString(int,int)=0;
  //  virtual void StringToElem(char *,int,int)=0;
  //  friend ostream & operator << (ostream &os,matrix<T> &m) { m.Store(os); return os; }
  //  friend istream & operator >> (istream &is,matrix<T> &m) { m.Restore(is); return is; }
};





// V3.0 Die Klasse Matrix wird in eine Template umgewandelt
// Auf T mssen folgende Operatoren definiert werden
// +,- (auch Vorzeichenwechseln),*,/,>,>=,==,<<,>> und Funktionen T inv(T),int sign(T),T betrag(T) {wie abs()}

template <class T> 
class matrix: public MatrixObject {
private:
  int m_gauss(int,int* );  //  ein gauss schritt ab zeile=spalte=x
  // unsauber; sollte zu m_gauss(y,x) ve�dert werden.
  int m_gauss_jordan(int);	//  ein gauss schritt ab zeile=spalte=x
  // return= welche Spalte hat Nullstelle >=x oder -1 wenn keine
  void tausche(int,int);		// tausche a mit b zeile
  sets sprungstellen();		// Sets mit Sprugstellen
  int sstellencount();		// gibt die zahl der sprungstellen zurck
  int sstellencount(const sets &);	// gibt die zahl der sprungstellen zurck
  int nichtnull(int,int=0);   // gibt nr der ersten nicht 0 zeile
  // in x spalte ab y zeile oder -1
  int maxkriterium(sets &);	// gibt den spaltennummer nach maximum
  // Kriterium; wenn -1 keine Spaltein; sets=markierte spalten
  int max2kriterium();		// ben�igt bei eckenaustauschal.
  int quotkriterium(int);		// gibt	Zeile nach quotentien
  // Kriterium; wenn -1 keine Zeile int=spalte
  int eckenaustausch();		// eckenaustauschalgorithmus
  // 0-all b negativ (OK) 1-keine Spalte 2-keine Zeile
  // 3-Zyklus; keine gute Zyklus bedingung
  int sucheecke();			// eckenfindung und eckenaustausch zusammen
  // 1- OK 0-Kein Optimum
  sets einheitszeilen(); 		// gibt den sets mit einheitszeilen
  // von Matrix nach Eckenfindung und Austausch
  matrix<T> rechneloesung(sets &); //Berechnet die L�ung aus Optimierungs
  // matrix nach gefundener Ecke mit Einheitszeilen in sets
  matrix<T> sucheloesung(); // sucht eine L�ung fr Optimierung Ver.
  // Prozeduren fr L�ung von Matrixspielen
  void Spaltentausch(int k,int l,int *ozeile=0,int *uzeile=0);
  matrix<T> Anfangstabelau(int *&ozeile,int *&ospalte,int *&lspalte,int *&rspalte);
  bool ms_ende();
  int pivotzeile();
  int pivotspalte(int pzeile);
  void Austauschschritt(int pzeile,int pspalte,int *lspalte=0,int *uzeile=0,int *ozeile=0,int *rspalte=0);
  
protected:
  T  *adrmatrix;
  // nicht berprfte zugriff auf Matrixelemente (berprfte bei operator())
  // y zeile x spalte - Index beginnt bei 0 !!!
  T& elem(int y,int x) {
    // if (y>=hoehe || x>=breite) cerr<<"Out of Index "<<y<<x<<hoehe<<breite<<endl;
    return adrmatrix[y*breite+x];
  }
public:
  matrix(int,int,protstream&);		// hoehe * breite
  matrix(const matrix<T> &);
  matrix(const matrix<T> &,int x1,int y1,int x2,int y2);
  // ein Construktor der der Funktion Copy entspechen soll
  matrix(const matrix<T> &,int y,int x);
  // noch ein Construktor fr Streichungsmatrix
  matrix<T> & operator = (const matrix<T> &);
  virtual ~matrix();
  virtual MatrixType whoamI();
  virtual MatrixObject *clone(bool withprot=false);

  MatrixType GiveTempType(rational r);
  MatrixType GiveTempType(double d);
#if !defined(NO_GMP)
  MatrixType GiveTempType(widefloat wf);
#endif
  virtual void gauss(int *a=0);  
  // Beim jedem Zeilentausch wird int mit -1 multipliziert, wichtig fr
  // determinanten ausrechnung, wenn Zeiger auf Null zeigt normalles Ablauf,
  // bei jedem Zeilentausch �dert sich die Determinante um Faktor -1
  // gauss und gausjordan unterscheiden sich auch in implementierung dadurch, da�beim Gauss
  // die Sprugstellenzeile nicht durch sich selber geteilt wird, um Sprugstelleneintrag
  // auf 1 zu bringen
  // m_gauss gibt die Spalte mit Sprungstelle zurck 0..breite-1
  virtual void gauss_jordan();		
  // Gau�ordan Algorithmus
  // gaus_jordan und m-gauss wurden ver�dert, weil die Sprungstellen nicht 
  // notwendig (x,x) sein mu�wie am Anfang vorausgesetzt, darum ist die Iplementirung 
  T& operator () (int,int); // gibt wert=?.matrix(zeile,spalte)
  T operator() (int,int)const;
  virtual void loesung();	 // gibt die matrixloesung von
  // der matrix nach Gau�ordan algorithmus
  // x=x1+a*x2+b*x3...
  // x1 - speziele L�ung
  // x2,x3 Basis des Kerns von Koefizienten matrix
  virtual void inverse(); 	// gibt die Inverse der matrix zurck
  // wenn es keine gibt matrix(0,0)
  virtual void bild();		// Gibt das Bild von Matrix
  virtual void kern();		// Berechne Kern von Matrix
  virtual void determinante();	// Es wird eine DETERMINANTE MIT HILFE DES GaussAlgorithmus ausgerechnet
  // Grunds�zlich kann man nur Determinanten aus n*n Matrizen ausrechnen hier wird auch eine
  // n*(n+1) Matrize zugelassen (aus praktischen Grnden) genau wird als Rckgabe eine Matrix(1*1)
  // zurckgegeben, obwohl man eine rational erwartet h�te.
  // proceduren zu Optimierungsverfahren
  virtual int eckenfindung(); 		// eckenfindungsalgorithmus
  // 0-keine Zeile 1-keine Spalte (also gibt es ein Optimum)
  virtual void optimiere();	  // Optimierungs verfahren zu Matrix in Form
  // a00*x1+a01*x2 >= b0
  // a10*x1+a11*x2 >= b1
  // Gewinnfunktion (zu Maximieren) g(x)=g0+g1*x1+g2*x2
  // Aussehen der dazugeh�iger Matrix 3*3
  // a00 a01 b0
  // a10 a11 b1
  // g1  g2  g0
  // Weitere Proceduren
  virtual void naehrungsloesung(); // gibt die NL nach (At*A)*x=At*b
  // Matrizen operationen
  void mmul(matrix<T> &); // Matrizen Multiplikation  (zu Verstehen als x*=y)
  void madd(matrix<T> &); // Matrizen Addition
  void msub(matrix<T> &); // Matrizen Substraktion
  // diese Definition ist mir sinnvoller, wegen sp�eren Ableiteung erschienen, als
  // matrix<T> mmul(matrix<T> &) also m=x*y;
  // charakteristisches polynom
  virtual void chpol();
  virtual void matrixspiel();
  // elementare Unformungen
  virtual void swap(int,int,bool row);
  virtual void pivotiere (int,int,bool row=true);	
          // pivotiere an der Stelle hoehe,breite Zeilen- oder Spaltenweise
  virtual void austauschschritt(int pzeile,int pspalte); 
          // wird bei matrixspiel  benutzt
  void factormul(int,bool row,const T&); 
           // Multiplizieren einer Zeile oder Spalte mit Faktor
  void factoradd(int,int,bool row,const T&); 
           // Addition einer Zeile oder Spalte zu voelfachens einer anderen
  // rekursives Determinantensuche 
  T rekdeterminante();
  // Funktion die das Editieren der Matrix erleichtern
  void paste(const matrix<T> &,int x,int y,MyTEdge);
  virtual void transponiere();
  virtual void resize(int neuehoehe,int neuebreite,bool linksveraendern,bool obenveraendern);
  void fill(const T &,int x1,int y1,int x2,int y2);
  // Output
  virtual void Restore(istream& is);
  virtual void Store(ostream& os);
  // Liest aus
  // istream die ben�ige Zahl von T
  // Vorsicht! kann nicht benutzt werden um matrix als Datei zu lesen;
};

#include "protstream.h"

// Type Definitionen
// typedef matrix<rational> mrational;
// typedef matrix<double> mdouble;

template <class T>
inline MatrixObject::MatrixType matrix<T>::GiveTempType(rational r) { return trational; }
template <class T>
inline MatrixObject::MatrixType matrix<T>::GiveTempType(double d) { return tfloat; }
#if !defined(NO_GMP)
template <class T>
inline MatrixObject::MatrixType matrix<T>::GiveTempType(widefloat d) { return twidefloat; }
#endif

//////////////////////////
// Matrix Implementierung
//////////////////////////
// Die frher benutzte Notation rational[0] um Nullwert nachzuprfen ist ungltig
// []operator geh�t jetzt zu private
// stattdessen die funktion rational::sign() benutzen
// int konversion (int)rational ist fehlertrechtig und werde aufgegeben
//
// V3.0 Es mu�gesichert werden ,da�beim Fehlerklasse mylongint::overflow alle dynamischen
// komponenten freigegebnen werden
// dies ist nicht der Fall aber beim �derungen mu�es bercksichtig werden
// Umwandlung in Template

template <class T>
matrix<T>::matrix (int h,int b,protstream& p):MatrixObject(p) // protstream wurde zugewiesen
{
  hoehe=h; breite=b;
  if (b>0 && h>0) {
    adrmatrix = new T [h*b];
    T *adr=adrmatrix;                      // Tabel mit Nullen initialisieren
    const int msize=h*b;                   // bei T=rational w�e das nicht n�ig
    for(int t=0;t<msize;*(adr++)=0,t++);	// nur bei double
    // Hier wrde sich bitten die Funktion als matrix<double>::matrix(int,int,protstream&) berladen
    // leider funktioniert es nicht
  }
  else
    adrmatrix = 0;
}
template <class T>
matrix<T>::matrix(const matrix<T> & m):MatrixObject(m.prot)
{
	hoehe=m.hoehe;
	breite=m.breite;
	if (hoehe*breite>0) {
		adrmatrix = new T[hoehe*breite];
		for(int i=0;i<hoehe*breite;i++) adrmatrix[i]=m.adrmatrix[i];
	} else adrmatrix=0;
}
template <class T>
matrix<T>::matrix(const matrix<T> &m,int y,int x):MatrixObject(m.prot) {
  // enspricht der Streichungs Matrix wenn y (Zeile) x (Spalte) au�r 
  // Bereich dann die gr�ten m�lichen Werte oder leere Matrix
  if (m.hoehe<=1 || m.breite<=1) {
    hoehe=0; breite=0; adrmatrix=0;
  } else {
    hoehe=m.hoehe-1;
    breite=m.breite-1;
    adrmatrix = new T[hoehe*breite];
    if (y>hoehe-1) y==hoehe-1;
    if (x>breite-1) x==breite-1;
    T *copyadr=adrmatrix;
    for (int ty=0;ty<m.hoehe;ty++) {
      for (int tx=0;tx<m.breite;tx++) {
	if (ty!=y && tx!=x) {
	  *copyadr=m.adrmatrix[ty*m.breite+tx];
	  copyadr++;
	}
      }
    }
  }
}
template <class T>
matrix<T>& matrix<T>::operator = (const matrix<T> &z) {
  if (this!=&z) {
    if (adrmatrix) delete[] adrmatrix;
    //		prot=z.prot   // Nicht n�ig wenn man voraussetz das die Gleicheit bereits besteht
    //    sonst msste der operator = fr protstream definiert werden
    hoehe=z.hoehe;
    breite=z.breite;
    if (hoehe*breite>0) {
      adrmatrix=new T[hoehe*breite];
      T *adr1=adrmatrix,*adr2=z.adrmatrix;
      int i;
      for(i=0;i<hoehe*breite;i++) *adr1++=*adr2++;
    } else adrmatrix=0;
  }
  return *this;
}
template <class T>
MatrixObject *matrix<T>::clone(bool withprot) {
  if (withprot) 
      return new matrix<T>(*this);
  else {
    matrix<T> *ret=new matrix<T>(hoehe,breite,* new protstream());
    if (hoehe*breite>0) {
      for(int i=0;i<hoehe*breite;i++) ret->adrmatrix[i]=adrmatrix[i];
    }
    return ret;
  }
}
template <class T>
matrix<T>::~matrix() {
  if (adrmatrix) delete[] adrmatrix;
}

/* GNU c++ gcc implemetiert diese Sorte von Templates nicht
template <rational>
MatrixObject::MatrixType matrix<T>::whoamI() {
  return trational;
}
template <double>
MatrixType matrix<double>::whoamI() {
  return tfloat;
}
*/
// Eine tricky L�ung

template <class T>
MatrixObject::MatrixType matrix<T>::whoamI() {
  T a;
  return GiveTempType(a);
}
template <class T>
T & matrix<T>::operator () (int i,int j) {
  static T dummy;
  if ((i<0) || (i>=hoehe)) i=0;
  if ((j<0) || (j>=breite)) j=0;
  if (adrmatrix)
    return adrmatrix[i*breite+j];  // ist adrmatrix eventuell 0 kann es zum Absturz bringen
  else return dummy;
}
template <class T>
T matrix<T>::operator () (int i,int j)const {
  static T dummy;
  if ((i<0) || (i>=hoehe)) i=0;
  if ((j<0) || (j>=breite)) j=0;
  if (adrmatrix)
    return adrmatrix[i*breite+j];  // ist adrmatrix eventuell 0 kann es zum Absturz bringen
  else return dummy;
}
template <class T>
int matrix<T>::nichtnull(int kol,int first) {   // Zeile,Spalte
  int i;
  for (i=kol;i<hoehe;i++)
    if (sign(adrmatrix[i*breite+first])) return i;
  return -1;
}
template <class T>
void matrix<T>::tausche(int a,int b) {  // Zeilentauschen
  T x;
  int i;
  if (a<0 || b<0 || a>=hoehe || b>=hoehe || b==a) return;
  for (i=0;i<breite;i++) {
    x=adrmatrix[a*breite+i];
    adrmatrix[a*breite+i]=adrmatrix[b*breite+i];
    adrmatrix[b*breite+i]=x;
  }
}

template <class T>
int matrix<T>::m_gauss(int i,int *tausch) {
  int x=-1,y; // x - spalten y - zeilen Z�ler
  T *adr,*adr2; // nur fr Beschleunigung
  // und fr Zeiger Spezielisten
  T a,b,c,temp;
  int ai; // Nullspalten au�r acht lassen ai = erste Spalte die zur Betrachtung kommt
  for (ai=i;ai<breite && (x=nichtnull(i,ai))<0;ai++);
  if (ai!=breite) { 
    tausche(x,i);   // Wenn es wenigstens eine nicht Nullspalte gibt
    if (x!=i && tausch!=0) (*tausch)*=-1; // Tja, irgedwie mu�man Zeilentausch merken
    adr=adrmatrix+breite*i+ai;
    a=*adr;
    for (y=i+1;y<hoehe;y++) {
      adr2=adrmatrix+breite*y+ai;
      if (sign(*adr2)) {
	b=*adr2;     // y:Zeile=y.Zeile()-b/a*x.Zeile
	// a=erster Eintrag der x Zeile b=erster Eintrag der y Zeile
	*adr2++=0;	// ersten Antrag anmerken und sofort auf Null setzen
	// addition ab n�hster spalte
	adr=adrmatrix+breite*i+(ai+1);
	for (x=ai+1;x<breite;x++) {
	  temp=(*adr)/a;
	  c=b*temp;
	  *adr2=(*adr2)-c;
	  adr2++; adr++;
	}
      }
    }
    return ai;
  } else return -1;
}
// fast identisch mit m_gauss aber der Sprungstellen Eintrag wird erst auf 1 gebracht
template <class T>
int matrix<T>::m_gauss_jordan(int i) {
  int x=-1,y; // x - spalten y - zeilen Z�ler
  T *adr,*adr2; // nur fr Beschleunigung
  // und fr Zeiger Spezielisten
  T a,b;
  int ai; // Nullspalten au�r acht lassen ai = erste Spalte die zur Betrachtung kommt
  for (ai=i;ai<breite && (x=nichtnull(i,ai))<0;ai++);
  if (ai!=breite) { 
    tausche(x,i);   // Wenn es wenigstens eine nicht Nullspalte gibt
    // (i,ai) Antrag auf 1 skalieren zeile durch (i,i) teilen
    adr=adrmatrix+breite*i+ai;
    a=*adr; *adr++=1;
    for (x=ai+1;x<breite;x++) { *adr=*adr/a; adr++; }
    // durch Zeilen addierung Antrag (y,i) auf Null bringen
    // x.Zeile=x.Zeile-x.Zeile(y,i)*operation.Zeile
    for (y=i+1;y<hoehe;y++) {
      adr2=adrmatrix+breite*y+ai;
      if (sign(*adr2)) {
	a=*adr2;
	*adr2++=0;	// ersten Antrag anmerken und sofort auf Null setzen
	// addition ab n�hster spalte
	adr=adrmatrix+breite*i+(ai+1);
	for (x=ai+1;x<breite;x++) {
	  b=a*(*adr);
	  *adr2=(*adr2)-b;
	  adr2++; adr++;
	}
      }
    }
    return ai;
  } else return -1;
}
template <class T>
void matrix<T>::gauss(int *tausch) {
  int i;
  prot<<"Gauss von\n";
  prot<<*this;
  for (i=0;i<breite && i<hoehe-1 ;i++) {
    m_gauss(i,tausch);
    prot<<"Gauss nr."<<(i+1)<<"\n"<<*this;
  }
}
template <class T>
void matrix<T>::gauss_jordan() {
  int i,j,k;
  T a,b;
  T *adr,*adr2;
  prot<<"Gauss-Jordan von\n"<<*this;
  for (i=0;i<breite && i<hoehe ;i++) {
    int ai=m_gauss_jordan(i);
    // ai gibt die Spalte mit Springstelle i die Zeile
    // prot<<"ai ist "; prot<<ai; prot<<" "; prot<<i; prot<<"\n"; prot<<*this;
    if (ai>0 && sign(adrmatrix[i*breite+ai])) {
      // dieses Teil ist mit der aus m_gauss sehr �nlich
      for (j=0;j<i;j++) {
	adr2=adrmatrix+breite*j+ai;
	if (sign(*adr2)) {
	  a=*adr2;
	  *adr2++=0;	// ersten Antrag anmerken und sofort auf null setzen
	  // addition ab n�hster spalte
	  if (ai==breite-1) break; // Wenn bereits letzte Spalte dann fertig!!
	  adr=adrmatrix+breite*i+(ai+1);
	  for (k=ai+1;k<breite;k++) {
	    b=a*(*adr);
	    *adr2=(*adr2)-b;
	    adr2++; adr++;
	  }
	}
      }
    }
    prot<<"Gauss-Jordan nr."<<(i+1)<<"\n"<<*this;
  }
}
template <class T>
sets matrix<T>::sprungstellen() {
  int x,y,last;
  sets s(breite);
  last=breite-1;
  for (y=hoehe-1;y>=0 && last!=-1;y--)
    for (x=0;x<=last;x++)
      if (sign(adrmatrix[y*breite+x])) {
	last=x-1;
	s.add(x);
      }
  if (y!=-1) s.clear();
  return s;
}
template <class T>
int matrix<T>::sstellencount(const sets & a) {
  return a.howmany();
}
template <class T>
int matrix<T>::sstellencount() {
  return sstellencount(sprungstellen());
}
template <class T>
void matrix<T>::loesung() {
  sets s=sprungstellen();
  int c=s.howmany();
  if (c==breite || s.exists(breite-1)) { 
    prot.putError("no_solution_eqn");
    (*this)=matrix<T>(0,0,prot); return; 
  }
  matrix<T> l(breite-1,breite-c,prot);
  int x,y,z,v;
  y=0;
  for (x=0;x<breite-1;x++) {
    if (s.exists(x)) { l(x,0)=adrmatrix[y*breite+breite-1]; y++; }
    else l(x,0)=0;
  }
  y=1;
  for (x=0;x<breite-1;x++) {
    if (!s.exists(x)) {
      v=0;
      for (z=0;z<breite-1;z++) {
	if (s.exists(z)) { l(z,y)=-adrmatrix[v*breite+x]; v++; }
	else l(z,y)=0;
      }
      l(x,y)=1;
      y++;
    }
  }
  (*this)=l;
  prot<<"L\366sung\n"<<(*this);
}
template <class T>
void matrix<T>::inverse () {
  if (breite!=hoehe) {
    //		cout<<"Man kann nur eine Inverse von n*n Matrix ausrechnen\n";
    prot.putError("must_be_square");
    (*this)=matrix<T>(0,0,prot); return;
  }
  matrix<T> s(breite,breite*2,prot);
  int x,y;
  T a;
  for (y=0;y<hoehe;y++)
    for (x=0;x<breite;x++)
      s(y,x)=adrmatrix[y*breite+x];
  a=0;
  for (y=0;y<hoehe;y++)
    for (x=breite;x<breite*2;x++)
      s(y,x)=a;
  a=1;
  for(x=0;x<breite;x++)
    s(x,x+breite)=a;
  s.gauss_jordan();
  sets sstellen=s.sprungstellen();
  for (x=0;x<hoehe;x++)
    if (!sstellen.exists(x)) {
      // matrix nicht invertierbar spungstellen liegen nicht nacheinander
      prot<<"Matrix ist nicht invertierbar\n";
      prot.putError("not_invertible");
      (*this)=matrix<T>(0,0,prot); return;
    }
  matrix i(hoehe,breite,prot);
  for (y=0;y<hoehe;y++)
    for (x=0;x<breite;x++)
      i(y,x)=s(y,x+breite);
  prot<<"Inverse\n"<<i;
  (*this)=i;
}
template <class T>
void matrix<T>::bild() {
  matrix<T> b(*this);
  b.gauss_jordan();
  sets s=b.sprungstellen();
  int x,y,spalte,dim=s.howmany();
  matrix<T> l(hoehe,dim,prot);
  s.init();
  for (x=0;x<dim;x++) {
    spalte=s.next_full();
    for(y=0;y<hoehe;y++)
      l.adrmatrix[y*l.breite+x]=adrmatrix[y*breite+spalte];
  }
  prot<<"Bild von Matrix\n"<<l;
  (*this)=l;
}
template <class T>
void matrix<T>::kern() {
  int x,y;
  // Bilde eine matrix mit zus�zlicher Nullspalte
  matrix<T> b(hoehe,breite+1,prot);
  for (y=0;y<hoehe;y++)
    for(x=0;x<breite;x++)
      b.adrmatrix[y*b.breite+x]=adrmatrix[y*breite+x];
  b.gauss_jordan();
  b.loesung();
  if (b.breite==1) {
    prot<<"Kern von Matrix ist ein Nullvektor\n"<<b;
    (*this)=b; return;
  }
  matrix<T> l(b.hoehe,b.breite-1,prot);
  for (y=0;y<l.hoehe;y++)
    for(x=0;x<l.breite;x++)
      l.adrmatrix[y*l.breite+x]=b.adrmatrix[y*b.breite+x+1];
  prot<<"Kern von Matrix\n"<<l;
  (*this)=l;
}
template <class T>
void matrix<T>::determinante() {
  if (hoehe!=breite && (hoehe+1)!=breite) {
    prot.putError("must_be_square");
    (*this)=matrix<T>(0,0,prot); return;
  }
  matrix<T> hilfsmatrix(*this);
  int tausche=1; // bei jedem Zeilentausch �dert sich die Determinante um Fektor -1
  hilfsmatrix.gauss(&tausche);
  T det(1);
  int x;
  for (x=0;x<hoehe;x++) det*=hilfsmatrix(x,x);
  T temp(tausche);
  det*=temp;
  matrix<T> detmatrix(1,1,prot);
  detmatrix(0,0)=det;
  prot<<"Determinante durch Multiplikation der diagonalen Eintr\344ge\n";
  // prot<<detmatrix;
  // prot<<det;
  (*this)=detmatrix;
}
template <class T>
void matrix<T>::pivotiere (int zeile,int spalte,bool row) {
  // Koordinaten sinvoll ?
  if (zeile>=0 && spalte>=0 && zeile<hoehe && spalte<breite) {    
    T a=adrmatrix[zeile*breite+spalte];
    T b;
    if (sign(a)) { // Wenn Null dann keine Pivotierung
      if (row) { // Pivotieren Zeilenweise
	T *adr,*adr2;
	int  x,y;
	T temp(1);
	if (!(a==temp)) {// Wenn 1 dann Teilung unn�ig
	  adr=adrmatrix+spalte;
	  for (y=0;y<hoehe;y++) {
	    *adr/=a;
	    adr+=breite;
	  }
	}
	for (x=0;x<breite;x++) {
	  if (x!=spalte) {
	    adr2=adrmatrix+spalte;
	    adr=adrmatrix+x;
	    a=adrmatrix[zeile*breite+x];
	    for (y=0;y<hoehe;y++) {
	      b=a*(*adr2);
	      *adr-=b;
	      adr+=breite;
	      adr2+=breite;
	    }
	  }
	}
      } else { // Pivotieren Spaltenweise
	T *adr,*adr2;
	int  x,y;
	T temp(1);
	if (!(a==temp)) {// Wenn 1 dann Teilung unn�ig
	  adr=adrmatrix+zeile*breite;
	  for (x=0;x<breite;x++) {
	    *adr/=a;
	    adr++;
	  }
	}
	for (y=0;y<hoehe;y++) {
	  if (y!=zeile) {
	    adr2=adrmatrix+zeile*breite;
	    adr=adrmatrix+y*breite;
	    a=adrmatrix[y*breite+spalte];
	    for (x=0;x<breite;x++) {
	      b=a*(*adr2);
	      *adr-=b;
	      ++adr;
	      ++adr2;
	    }
	  }
	}
      } 
    }
    prot<<"pivotiere ("<<(zeile)<<","<<(spalte)<<")\n"<<*this;
  }
}
template <class T>
int matrix<T>::maxkriterium(sets &mark) {
  int erg=-1,x,y;
  T *adr=adrmatrix+(hoehe-1)*breite; // an Anfang der g zeile setzen
  T hoechst,a;
  for (x=0;x<breite-1;x++) {
    if (!mark.exists(x))  { // Keine spalte markiert
      if (erg<0) {
	for (y=0;(y<hoehe && (sign(adrmatrix[y*hoehe+x])==0));y++);
	// ob null spalte
	if (y<hoehe) {
	  erg=x;
	  hoechst=betrag(adr[x]);
	}
      } else {
	a=betrag(adr[x]);
	if (a>hoechst) {
	  erg=x;
	  hoechst=a;
	}
      }
    }
  }
  return erg;
}
template <class T>
int matrix<T>::max2kriterium() {
  int erg=-1,x,y;
  T *adr=adrmatrix+(hoehe-1)*breite; // an Anfang der g zeile setzen
  T hoechst;
  for (x=0;x<breite-1;x++) {
    if (erg<0) {
      for (y=0;(y<hoehe && (sign(adrmatrix[y*hoehe+x])==0));y++);
      // ob null spalte
      if (y<hoehe) {
	erg=x;
	hoechst=adr[x];
      }
    } else {
      if (adr[x]>hoechst) {
	erg=x;
	hoechst=adr[x];
      }
    }
  }
  return erg;
}
template <class T>
int matrix<T>::quotkriterium(int spalte) {
  int erg=-1,y;
  T min,a;
  for (y=0;y<hoehe-1;y++) {
    a=adrmatrix[breite*(hoehe-1)+spalte]*adrmatrix[breite*y+spalte];
    T temp(0);
    if (temp>a) {
      if (erg<0) {
	erg=y;
	a=betrag(adrmatrix[breite*y+spalte]);
	min=adrmatrix[breite*y+breite-1]/a;
      } else {
	a=betrag(adrmatrix[breite*y+spalte]);
	a=adrmatrix[breite*y+breite-1]/a;
	if (min>a) {
	  erg=y;
	  min=a;
	}
      }
    }
  }
  return erg;
}
template <class T>
int matrix<T>::eckenfindung() {
  int s,z;
  sets mark(breite-1);
  prot<<"Eckenfindung von\n"<<*this;

  while (1) {
    s=maxkriterium(mark);
    if (s<0) return 1;
    z=quotkriterium(s);
    if (z<0) return 0;
    pivotiere(z,s);
    mark.add(s);
  }
}
template <class T>
int matrix<T>::eckenaustausch() {
  int x,z,s,ind=0;
  for (x=0;x<breite-1;x++) { // es gibt breite-1 ecken
    T temp(0);
    for (z=0;(z<breite-1 && temp>=adrmatrix[breite*(hoehe-1)+z]);z++);
    if (z==breite-1) return 0;
    if (!ind) {
      prot<<"Eckenaustausch\n";
      ind=1;
    }
    s=max2kriterium();
    if(s<0) return 1;
    z=quotkriterium(s);
    if (z<0) return 2;
    pivotiere(z,s);
  }
  return 3;
}
template <class T>
int matrix<T>::sucheecke() {
  if(!eckenfindung()) {
    prot.putError("simplex_no_optimum");
    prot<<"Es gibt keine optimale L\366sung\n";
    return 0;
  }
  if(!eckenaustausch()) return 1;
  else return 0;
}
template <class T>
sets matrix<T>::einheitszeilen() {
  sets s(hoehe-1);
  int x,y;
  for(y=0;y<hoehe-1;y++) {
    for(x=0;x<breite && sign(adrmatrix[y*breite+x])==0;x++);
    T temp(1);
    if (adrmatrix[y*breite+x]==temp) {
      for(x=x+1;x<breite && sign(adrmatrix[y*breite+x])==0;x++);
      if (x==breite) s.add(y);
    }
  }
  return s;
}
template <class T>
matrix<T> matrix<T>::rechneloesung (sets &mark) {
  int z,x,y,h=mark.howmany();
  if (h==0) { return matrix<T>(0,0,prot); }
  matrix<T> l(h,breite,prot);
  mark.init();
  for(y=0;y<h;y++) {
    z=mark.next_full();
    for(x=0;x<breite;x++)
      l.adrmatrix[breite*y+x]=adrmatrix[breite*z+x];
  }
  l.gauss_jordan();
  l.loesung();
  return l;
}
template <class T>
matrix<T> matrix<T>::sucheloesung() {
  int x,y;
  // ob nullvektor die L�ung ist
  prot<<"Ich suche eine L\366sung\n";
  for (y=0;y<hoehe-1 && 0>=sign(adrmatrix[y*breite+breite-1]);y++);
  if (y==hoehe-1) { return matrix<T>(breite-1,1,prot); }
  matrix<T> b(hoehe,breite+1,prot);
  T a=adrmatrix[breite-1]; // auf erstes be setzen
  // kopiern zu B_dach matrix und suche gr�tes b gleichzeitig
  for (y=0;y<hoehe-1;y++) {
    if (adrmatrix[y*breite+breite-1]>a) a=adrmatrix[y*breite+breite-1];
    for (x=0;x<breite-1;x++)
      b.adrmatrix[y*b.breite+x]=adrmatrix[breite*y+x];
    b.adrmatrix[y*b.breite+b.breite-2]=-1;
  }
  // Letze Zeile in B_dach Einrichten b_max-b
  // Ausgang matrix B_dach sichern
  matrix<T> bausgang(b);
  for (y=0;y<hoehe-1;y++) {
    x=y*b.breite+b.breite-1;
    b.adrmatrix[x]=a-adrmatrix[y*breite+breite-1];
    bausgang.adrmatrix[x]=adrmatrix[y*breite+breite-1];
  }
  x=(b.hoehe-1)*b.breite+b.breite-1;
  // x auf letztes Antrag in der M. setzen
  b.adrmatrix[x]=-a;
  b.adrmatrix[x-1]=1;
  prot<<"Ausgangsmatrix zur Suche nach einer L\366sung\n";
  if (!b.sucheecke()) {
    a=b.adrmatrix[x];
    if (sign(a)>=0) {
      // Kommt nur vor bei den Ungleichungen ohne Beschr�kung
      // Suche l�ung wenn der Gewinn >=0
      // L�ung von Ursprung b_dach ohne gewinnzeile
      // Letzte Splate=Gewinn+(letzte splate)Ursprung B_dach+Letztes B_Dach
      T hr;
      matrix<T> bhilf(hoehe-1,breite+1,prot);
      for (y=0;y<bhilf.hoehe;y++) {
	for(x=0;x<bhilf.breite-2;x++)
	  bhilf.adrmatrix[y*bhilf.breite+x]=adrmatrix[y*breite+x];
	bhilf.adrmatrix[y*bhilf.breite+x]=-1;
	x++; // x auf letzte spalte setzen
	hr=a+b.adrmatrix[y*b.breite+x];
	hr=hr+adrmatrix[y*breite+x-1]; // matrix um eine spalte krzer
	bhilf.adrmatrix[y*bhilf.breite+x]=hr;
      }
      prot<<"Gewinn positiv es gibt also eine L\366sung\n";
      bhilf.gauss_jordan();
      bhilf.loesung();
      matrix<T> l(breite-1,1,prot);
      for(y=0;y<breite-1;y++)
	l.adrmatrix[y]=bhilf.adrmatrix[bhilf.breite*y];
      return l;
    } else {
      prot<<"Keine L\366sung\n";
      prot.putError("simplex_no_one_solution");
      return matrix<T>(0,0,prot);
    }
  } else {
    a=b.adrmatrix[x];
    if (sign(a)<0) {
      prot<<"Keine L\366sung\n";
      prot.putError("simplex_no_one_solution");
      return matrix<T>(0,0,prot);
    }
  }
  sets ls(b.einheitszeilen());
  matrix<T> lb(bausgang.rechneloesung(ls));
  if (lb.hoehe==0) {
    return matrix<T>(0,0,prot);
  }
  matrix<T> l(breite-1,1,prot);
  for(y=0;y<breite-1;y++)
    l.adrmatrix[y]=lb.adrmatrix[lb.breite*y];
  return l;
}
template <class T>
void matrix<T>::optimiere() {
  prot<<"optimiere\n";
  prot<<*this;
  matrix<T> l(sucheloesung());
  if (l.hoehe==0) {
    // Error Message bereits in sucheloesung gesetzt
    (*this)=matrix<T>(0,0,prot); return;
  }
  prot<<"Eine L\366sung\n"<<l;
  matrix<T> b_(*this);
  T a,b;
  int x,y;
  for (y=0;y<hoehe;y++) {
    a=0;
    for (x=0;x<breite-1;x++) {
      b=adrmatrix[y*breite+x]*l.adrmatrix[x*l.breite];
      a+=b;
    }
    b=adrmatrix[y*breite+breite-1];
    b_.adrmatrix[y*breite+breite-1]=a-b;
  }
  prot<<"Ausgangsmatrix zu Eckensuche\n";
  if (!b_.sucheecke()) {
    (*this)=matrix<T>(0,0,prot); return;
  }
  sets ls=b_.einheitszeilen();
  matrix<T> l2(rechneloesung(ls));
  (*this)=l2;
}
template <class T>
void matrix<T>::naehrungsloesung() {
  if (hoehe<=breite-1) { 
    prot<<"Es existiert genaue L\366sung\n"; 
    gauss_jordan(); 
    loesung(); 
    return; 
  }
  prot<<"N\344hrungsl\366sung zu\n"<<*this;
  matrix<T> b(breite-1,breite,prot);
  int x,y,z;
  T a,c;
  for (y=0;y<breite-1;y++) {
    for (x=0;x<breite-1;x++) {
      a=0;
      for (z=0;z<hoehe;z++) {
	c=adrmatrix[z*breite+x]*adrmatrix[z*breite+y];
	a+=c;
      }
      b.adrmatrix[y*b.breite+x]=a;
    }
    a=0;
    for(z=0;z<hoehe;z++) {
      c=adrmatrix[z*breite+y]*adrmatrix[z*breite+breite-1];
      a+=c;
    }
    b.adrmatrix[y*breite+breite-1]=a;
  }
  prot<<"Matrix nach (At*A)x=At*b\n";

  b.gauss_jordan();
  b.loesung();
  (*this)=b;
}
// Vorbedingung die Matrizen a,b musen die Formate (m*n) und (n*t) haben
// Erste Matrix ist die Klassen besitzende. Ergeniss hat Format m*t
template <class T>
void matrix<T>::mmul(matrix<T> &m) {
  if (breite!=m.hoehe) return;
  prot<<"Multiplikation von 2 Matrizen.\n"<<*this<<"mal\n"<<m;
  matrix<T> b(hoehe,m.breite,prot);
  T a,c;
  for (int y=0;y<hoehe;y++) {
    for (int x=0;x<m.breite;x++) {
      a=0;
      for (int z=0;z<breite;z++) {
	c=adrmatrix[y*breite+z]*m.adrmatrix[z*m.breite+x];
	a+=c;
      }
      b.adrmatrix[y*m.breite+x]=a;
    }
  }
  prot<<"Ergebniss = ("<<hoehe<<"*"<<m.breite<<") Matrix\n"<<b;
  (*this)=b;
}
template <class T>
void matrix<T>::madd(matrix<T> &m) {
  if (hoehe!=m.hoehe || breite!=m.breite) return;
  prot<<"Addition von 2 Matrizen.\n"<<*this<<"plus\n"<<m;
  for (int y=0;y<hoehe;y++) {
    for (int x=0;x<breite;x++) {
      adrmatrix[y*breite+x]=adrmatrix[y*breite+x]+m.adrmatrix[y*breite+x];
    }
  }
  prot<<"Ergebniss \n"<<*this;
}
template <class T>
void matrix<T>::msub(matrix<T> &m) {
  if (hoehe!=m.hoehe || breite!=m.breite) return;
  prot<<"Substraktion von 2 Matrizen.\n"<<*this<<"minus\n"<<m;
  for (int y=0;y<hoehe;y++) {
    for (int x=0;x<breite;x++) {
      adrmatrix[y*breite+x]=adrmatrix[y*breite+x]-m.adrmatrix[y*breite+x];
    }
  }
  prot<<"Ergebniss \n"<<*this;
}
// Rekursives Berechnen der Determinante
// Vorsicht Nicht zu gebrauchen wegen zu hohen Speicheraufwand
// sehe optimiertes chpol Algorithmus
template <class T>
T matrix<T>::rekdeterminante() {
  if (hoehe!=breite) return T();
  if (hoehe==1) return adrmatrix[0];
  T ret;
  for (int y=0;y<hoehe;y++) {
    if (sign(adrmatrix[y*breite])!=0) {
      matrix<T> tmatrix(*this,y,0);
      if (y%2==0) {
	ret+=(adrmatrix[y*breite]*tmatrix.rekdeterminante());
      } else {
	ret-=(adrmatrix[y*breite]*tmatrix.rekdeterminante());
      }
    }
  }
  return ret;
}
// Dieses Programmteil wurde nach dem Buch 
// (Georg Schrage, Rdeger Baumann, "Strategiespiele Computerorientierte 
// Einfrung in Algorithmen der Spieltheorie", 
// R. Oldenbourg Verlag, Mnchen Wien 1984)
// aus Pascal nachprogrammiert (Seite 198-201)
// die Algorithmen und Funktionbezeichnungen wurden nicht ver�dert
// es sei denn mit prefix ms_
// wegen c++ Beschaffenheiten beginnnen alle Indexzahlen mit 0
template <class T>
void matrix<T>::Spaltentausch(int k,int l,int *ozeile,int *uzeile) {
  // tausch k-te und l-te Tableauspalte gegeneinander aus
  T r;
  int i,h;
  if (ozeile!=0) {
    h=ozeile[k]; ozeile[k]=ozeile[l]; ozeile[l]=h;
    h=uzeile[k]; uzeile[k]=uzeile[l]; uzeile[l]=h;
  }
  for (i=0;i<hoehe;i++) {
    r=elem(i,k); elem(i,k)=elem(i,l); elem(i,l)=r;
  }
}
template <class T>
matrix<T> matrix<T>::Anfangstabelau(int *&lspalte,int *&uzeile,int *&ozeile,int *&rspalte) {
  matrix<T> tab(hoehe+1,breite+1,prot);
  int i,j,min;
  // n�hste Zeile eigentlich nicht n�ig weil mit Nullen vorgefllt
  tab.elem(0,0)=T(0);
  // Erste Zeile (Au�r 0,0) mit -1 Fllen
  for (j=1;j<=breite;j++) tab.elem(0,j)=T(-1);
  // Erste Spalte (Au�r 0,0) mit 1 Fllen
  for (i=1;i<=hoehe;i++) tab.elem(i,0)=T(1);
  // Ausgangsmatrix auf Tabelau kopieren
  tab.paste(*this,1,1,TopLeft);
  
  for (i=0;i<=hoehe;i++) lspalte[i]=i ;
  uzeile[0]=0;
  for (j=1;j<=breite;j++) uzeile[j]=hoehe+j;
  for (j=0;j<=breite;j++) ozeile[j]=j;
  rspalte[0]=0;
  for (i=1;i<=hoehe;i++) rspalte[i]=breite+i;
  // Durch Spaltentausch wird ein Minimum der untersten Matrixzeile 
  // in die rechte Tableauspalte gebracht
  min=1;
  for (j=2;j<=breite;j++) {
    if (tab.elem(hoehe,j)<tab.elem(hoehe,min)) min=j;
  }
  if (min!=breite) tab.Spaltentausch(min,breite,ozeile,uzeile);
  return tab;
}
template <class T>
bool matrix<T>::ms_ende() {
  for (int i=0;i<hoehe-1;i++) {
    if (sign(elem(i,breite-1))>0) return false;
  }
  return true;
}
template <class T>
int matrix<T>::pivotzeile() {
  int zeile,pzeile=0;
  for (zeile=1;zeile<hoehe-1;zeile++) 
    if (elem(pzeile,breite-1)<elem(zeile,breite-1)) pzeile=zeile;
  return pzeile;
}
// Wenn -1 dann keine pivotspalte 
template <class T>
int matrix<T>::pivotspalte(int pzeile) {
  int spalte,pspalte;
  T chquotient,minimum(-1);
  for (spalte=0;spalte<breite-1;spalte++) {
    if (sign(elem(pzeile,spalte))<0) {
      chquotient=-elem(hoehe-1,spalte)/elem(pzeile,spalte);
      if (chquotient<minimum || minimum==T(-1)) {
	minimum=chquotient;
	pspalte=spalte;
      }
    }
  }
  if (sign(minimum)<=0) {
    if (minimum==T(-1))
      prot<<"L�ung mit max. Zilefunktion existiert nicht\n";
    else 
      prot<<"Das Tabelau ist entartet\n";
    return -1;
  }
  return pspalte;
}
template <class T>
void matrix<T>::Austauschschritt(int pzeile,int pspalte,int *lspalte,int *uzeile,int *ozeile,int *rspalte) {
  int index,zeile,spalte;
  T pivotelement;
  // Variablentausch
  if (lspalte) {
    index=lspalte[pzeile];
    lspalte[pzeile]=uzeile[pspalte];
    uzeile[pspalte]=index;
    index=ozeile[pspalte];
    ozeile[pspalte]=rspalte[pzeile];
    rspalte[pzeile]=index;
  }
  pivotelement=elem(pzeile,pspalte);
  if (sign(pivotelement)==0) return;
  for (zeile=0;zeile<hoehe;zeile++)
    for (spalte=0;spalte<breite;spalte++)
      if (zeile!=pzeile && spalte!=pspalte) { 
	elem(zeile,spalte)-=elem(pzeile,spalte)*
	  elem(zeile,pspalte)/pivotelement;
      }
  for (spalte=0;spalte<breite;spalte++)
    if (spalte!=pspalte) 
      elem(pzeile,spalte)/=pivotelement;
  for (zeile=0;zeile<hoehe;zeile++)
    if (zeile!=pzeile) 
      elem(zeile,pspalte)=-elem(zeile,pspalte)/pivotelement;
  elem(pzeile,pspalte)=T(1)/pivotelement;
}
template <class T>
void matrix<T>::matrixspiel() {
  prot<<*this;
  int *lspalte,*uzeile,*ozeile,*rspalte;
  int pzeile,pspalte;
  ozeile=new int[breite+1];
  uzeile=new int[breite+1];
  lspalte=new int[hoehe+1];
  rspalte=new int[hoehe+1];

  matrix<T> tab=this->Anfangstabelau(lspalte,uzeile,ozeile,rspalte);

  prot<<"Anfangstabelau\n"<<tab;
  tab.Austauschschritt(0,tab.breite-1,lspalte,uzeile,ozeile,rspalte);
  tab.Austauschschritt(tab.hoehe-1,0,lspalte,uzeile,ozeile,rspalte);
  // cerr<<"Anfangstabelau nach 2 Austausch Schritten"<<endl;

  prot<<"Ausgangstabelau fr Simplexalgorithmus\n"<<tab;
  //return;
  while (!tab.ms_ende()) {
    pzeile=tab.pivotzeile();
    pspalte=tab.pivotspalte(pzeile);
    prot<<"Austausch an der Stelle ("<<pzeile<<","<<pspalte<<")\n";
    if (pspalte==-1) {
      (*this)=matrix<T>(0,0,prot);
    }
    // cerr<<"Pivotiere an "<<pzeile<<" "<<pspalte<<endl;
    tab.Austauschschritt(pzeile,pspalte,lspalte,uzeile,ozeile,rspalte);
    prot<<tab;
  }
  // Endinformation
  prot<<"Das Spiel hat den Wert\n"<<tab.elem(hoehe,breite);

  prot<<"Optimale Strategie fr Spieler 1\n";
//   for (int j=0;j<breite;j++) {
//     cerr<<"ozeile[j]="<<ozeile[j]<<" j="<<j<<endl;
//     cerr<<"uzeile[j]="<<uzeile[j]<<" j="<<j<<endl;
//     cerr<<"rsplate[j]="<<rspalte[j]<<" j="<<j<<endl;
//     cerr<<"lspate[j]="<<lspalte[j]<<" j="<<j<<endl;
//   }
  matrix<T> spieler1(1,hoehe,prot);
  for (int zeile=0;zeile<hoehe;zeile++)
    for (int j=0;j<breite;j++) {
      if (uzeile[j]==zeile+1) 
	spieler1.elem(0,zeile)=tab.elem(hoehe,j);
    }
  prot<<spieler1;

  prot<<"Optimale Strategie fr Spieler 2\n";
  matrix<T> spieler2(1,breite,prot);
  for (int spalte=0;spalte<breite;spalte++)
    for (int i=0;i<hoehe;i++)
      if (rspalte[i]==spalte+1)
	spieler2.elem(0,spalte)=-tab.elem(i,breite);

  prot<<spieler2;
  // Ergebnismatrix als spieler1 Ergebniss
  (*this)=spieler1;
	
  // Aufr�men
  delete lspalte;
  delete uzeile;
  delete ozeile;
  delete rspalte;
}
// Suchen des charakteristischen Polynom durch
// optimierte Backtacking Determinanten Ausrechnung
// (Iterativ)
// trotzdem sind Berechnungen nur bis Matrizen Gr�e 20 m�lich
// Der Aufwand gleich der Summe einer Zeile des paskalischen Dreiecks 
// mit Breite der Matrix
// d.h 2^n mit jeder neuer Spalte w�hst der Aufwand doppelt
// Ab gewisser Grenze mehr als doppelt, weil der Speichermangmentproblem
// deutlich zunimmt
template <class T>
void matrix<T>::chpol() {
  if (breite!=hoehe) {
    (*this)=matrix<T>(0,0,prot);
    prot.putError("must_be_square");
    return;
  }
  prot<<*this;
  prot<<"charakteristisches Polynom\n";

  polynom<T> *lasttab=0,*akttab=0;
  // breite 1 und 2 wird als sonderfall behandelt
  if (breite==1) {
    akttab=new polynom<T>[1];
    akttab[0].set(0,adrmatrix[0]);
    akttab[0].set(1,T(-1));
  } else if (breite==2) {
    akttab=new polynom<T>[1];
    polynom<T> temp1(adrmatrix[0]);
    temp1.set(1,T(-1));
    polynom<T> temp2(adrmatrix[3]);
    temp2.set(1,T(-1));	    
    akttab[0]=temp1*temp2- (polynom<T> (adrmatrix[1]*adrmatrix[2]));
  } else {
    int n,msize,*puf,*lastpuf,s,index,x,y,i,t;
    puf=new int[breite];
    lastpuf= new int[breite];
    for (msize=2;msize<=breite;msize++) {
      akttab=new polynom<T>[kombie_zahl(breite,msize)];
    
      // Erzeugung der Kombinationen o.W.
      s=0;  index=0;  puf[0]=0; 
      // cout<<" tab large "<<kombie_zahl(breite,msize)<<endl;
      while (s>=0) {
	while (s<msize-1) {
	  puf[s+1]=puf[s]+1;
	  s++;
	}
	// Berechnung der Determinante fr geg. Kombination
	x=breite-msize;
	if (msize==2) {
	  // Vereinfachte Determinanten Ausrechnung a, b; c, d; ad-cb
	  //akttab[index++]=elem(puf[0],x)*elem(puf[1],x+1)-elem(puf[1],x)*elem(puf[0],x+1);
	  // Vorsicht Diagonale Eitr�e haben a_ii-x
	  if (puf[1]!=x+1 && puf[0]!=x) {
	    akttab[index++]=polynom<T> (adrmatrix[puf[0]*breite+x]
					*adrmatrix[puf[1]*breite+x+1]
					-adrmatrix[puf[1]*breite+x]
					*adrmatrix[puf[0]*breite+x+1]);
	  } else {
	    // Generiere aii-x
	    polynom<T> temp1(adrmatrix[puf[0]*breite+x]);
	    polynom<T> temp2(adrmatrix[puf[1]*breite+x+1]);
	    if (puf[0]==x)
	      temp1.set(1,T(-1));
	    if (puf[1]==x+1)
	      temp2.set(1,T(-1));
	    akttab[index++]=temp1*temp2- 
	      (polynom<T> (adrmatrix[puf[1]*breite+x]*
			   adrmatrix[puf[0]*breite+x+1]));
	  }
	} else {
	  // akttab[index]=T(0); 
	  for (y=0;y<msize;y++) {
	    for (i=0,t=0;i<msize;i++) {
	      lastpuf[t]=puf[i];
	      if (i!=y) t++;
	    }
	    // cout<<"index "<<kombie_index(lastpuf,breite,msize-1);
	    if (y%2==0) {
	      // Nachprfen ob Diagonal Eintrag
	      if (puf[y]==x) {
		polynom<T> temp(adrmatrix[puf[y]*breite+x]);
		temp.set(1,T(-1));
		akttab[index]+=lasttab[kombie_index(lastpuf,breite,msize-1)]*temp;
	      } else {
		akttab[index]+=lasttab[kombie_index(lastpuf,breite,msize-1)]*
		  adrmatrix[puf[y]*breite+x];
	      }
	    } else {
	      if (puf[y]==x) {
		polynom<T> temp(adrmatrix[puf[y]*breite+x]);
		temp.set(1,T(-1));
		akttab[index]-=lasttab[kombie_index(lastpuf,breite,msize-1)]*temp;
	      } else {
		akttab[index]-=lasttab[kombie_index(lastpuf,breite,msize-1)]*
		  adrmatrix[puf[y]*breite+x];
	      }
	    }
	    // cout<<" akttab "<<akttab[index]<<endl;
	  }

	  index++;
	} 
	// cout<<"msize "<<msize<<" index"<<index-1<<" det ";
	// akttab[index-1].Restore(cout);
	// cout<<endl;
	puf[s]++;
    
	while ( puf[s] > (breite-(msize-s))) {
	  s--;
	  if (s<0) break;
	  puf[s]++;
	}
      }

      if (lasttab) delete[] lasttab;
      lasttab=akttab;
    }
    delete[] puf;
    delete[] lastpuf;
  }
  akttab[0].clear_null();

  // Polynom zu einer Matrix extrahieren
  T *values;
  int *keys,len;
  // fprintf(stdout,"step 1\n");
  akttab[0].extract(len,values,keys);
  // fprintf(stdout,"step 2\n");
  if (len==0) { (*this)=matrix<T>(0,0,prot); return; }
  matrix<T> ret(2,len,prot);
  // fprintf(stdout,"step 3 len %d\n",len);
  for (int x=0;x<len;x++) {
    ret(0,x)=T(keys[x]);
    ret(1,x)=values[x];
  }
  // fprintf(stdout,"step 4\n");
  delete[] values;
  delete[] keys;
  // fprintf(stdout,"step 5\n");
  prot.add(ret,MatrixObject::tpolynom);
  (*this)=ret;
  if (akttab) delete[] akttab;
}
// Elementare Unformungen. Diese Funktionnen greifen oft auf die
// vorhandene private Funktionnen, bei manchen mssen noch Argumentenbereiche
// berprft werden. Die Konstruktion resultiert aus historischen Grnden.
// Bei allem mu�noch das Protokoll vervollst�digt werden.
template <class T>
void matrix<T>::swap(int index0, int index1, bool row) {
  prot<<"Tausche ";
  if (row) {
    if (index0>=0 && index1>=0 && index0<hoehe && index1<hoehe && index0!=index1)
      tausche(index0,index1);
    prot<<"Zeilen ";
  } else {
    if (index0>=0 && index1>=0 && index0<breite && index1<breite && index0!=index1)
      Spaltentausch(index0,index1);
    prot<<"Spalten ";
  }
  prot<<index0<<" mit "<<index1<<"\n"<<*this;
}
template <class T>
void matrix<T>::austauschschritt(int pzeile,int pspalte) {
    if (pzeile>=0 && pspalte>=0 && pzeile<hoehe && pspalte<breite) {
      Austauschschritt(pzeile,pspalte);
    }
    prot<<"Austauschschritt an der Stelle ("<<pzeile<<","<<pspalte<<")\n"<<*this;
}
template <class T>
void matrix<T>::factormul(int index,bool row,const T &factor) {
  T *adr;
  int add,limit;
  prot<<"Multipliziere ";
  if (row) {
    add=1; limit=breite;
    adr=adrmatrix+index*breite;
    prot<<"Zeile ";
  } else {
    add=breite; limit=hoehe;
    adr=adrmatrix+index;
    prot<<"Spalte ";
  }
  prot<<index<<" mit \n";
  for (int x=0;x<limit;x++) {
    *adr*=factor;
    adr+=add;
  }
  prot<<T(factor)<<*this;
}
template <class T>
void matrix<T>::factoradd(int index1 ,int index2, bool row, const T &factor) {
  T *adr1,*adr2;
  int add,limit;
  prot<<"Addiere ";
  if (row) {
    add=1; limit=breite;
    adr1=adrmatrix+index1*breite;
    adr2=adrmatrix+index2*breite;
    prot<<"Zeile ";
  } else {
    add=breite; limit=hoehe;
    adr1=adrmatrix+index1;
    adr2=adrmatrix+index2;
    prot<<"Spalte ";
  }
  prot<<index1<<" zu "<<index2<<" multipliziert mit\n";
  for (int x=0;x<limit;x++) {
    *adr1+=(*adr2)*factor;
    adr1+=add;
    adr2+=add;
  }
  prot.operator << (factor);
  prot<<*this;
}
// Funktion die das Editieren der Matrix erleichtern
// Vorsicht !! die Prot nicht vergessen
// Es wird automatisch ein neues Protokoll Objekt protstream gebildet 
template <class T>
matrix<T>::matrix(const matrix<T> &source,int x1,int y1,int x2,int y2):MatrixObject(*new protstream()) {
  if (source.adrmatrix) {
    x1%=source.breite;
    x2%=source.breite;
    y1%=source.hoehe;
    y2%=source.hoehe;
    int t;
    if (x1>x2) { t=x1; x1=x2; x2=t; };
    if (y1>y2) { t=y1; y1=y2; y2=t; };
    hoehe=y2-y1+1;
    breite=x2-x1+1;
    adrmatrix=new T[hoehe*breite];
    int x;
     for (x=0;x<breite;x++)
       for(int y=0;y<hoehe;y++)
 	adrmatrix[x+y*breite]=source.adrmatrix[x1+x+(y+y1)*source.breite];
  } else {
    adrmatrix=0;
    hoehe=breite=0;
  }
}
template <class T>
void matrix<T>::paste(const matrix<T> &source,int x,int y,MatrixObject::MyTEdge edge) {
  if(source.adrmatrix) {
    x%=breite;
    y%=hoehe;
    int xa,ya,h,b;
    switch(edge) {
    case TopLeft:
      xa=0; ya=0;
      b=min((breite-x),source.breite); h=min((hoehe-y),source.hoehe); break;
    case TopRight:
      h=min((hoehe-y),source.hoehe);
      b=min(x+1,source.breite);
      xa=max((source.breite-x-1),0);
      ya=0;
      x-=(b-1); break;
    case BottomLeft:
      h=min(y+1,source.hoehe);
      b=min((breite-x),source.breite);
      xa=0;
      ya=max((source.hoehe-y-1),0);
      y-=(h-1);  break;
    case BottomRight:
      h=min(y+1,source.hoehe);
      b=min(x+1,source.breite);
      xa=max((source.breite-x-1),0);
      ya=max((source.hoehe-y-1),0);
      x-=(b-1); y-=(h-1); break;
    }
    for (int tx=0;tx<b;tx++)
      for (int ty=0;ty<h;ty++)
	adrmatrix[tx+x+(ty+y)*breite]=source.adrmatrix[(ya+ty)*source.breite+tx+xa];
  }
}
// Intuitiv soll es einen Algorihmus geben um die Matrix zu Transponieren ohne ein neues Speicehrplatz
// zu belegen. Die L�ung ist aber nicht weiter trivial
template <class T>
void matrix<T>::transponiere() {
  if (adrmatrix) {
    T *tempadr=new T[hoehe*breite];
    for (int x=0;x<breite;x++)
      for (int y=0;y<hoehe;y++)
	tempadr[x*hoehe+y]=adrmatrix[x+y*breite];
    delete[] adrmatrix;
    adrmatrix=tempadr;
    int t=hoehe;
    hoehe=breite;
    breite=t;
  }
}
template <class T>
void matrix<T>::resize(int newhoehe,int newbreite,bool change_left,bool change_above) {
  if ((newhoehe!=hoehe || newbreite!=breite) && adrmatrix!=0) {
    T *tempadr=new T[newhoehe*newbreite];
    int begxpos,begypos,x,y;
    if (change_left)  {
      begxpos=breite-newbreite;
    } else begxpos=0;
    if (change_above)   {
      begypos=hoehe-newhoehe;
    } else begypos=0;
    for (y=0;y<newhoehe;y++) {
      for(x=0;x<newbreite;x++) {
	if (x<breite-begxpos && x+begxpos>=0 && y<hoehe-begypos && y+begypos>=0)
	  tempadr[y*newbreite+x]=adrmatrix[(y+begypos)*breite+x+begxpos];
	else
	  tempadr[y*newbreite+x]=T(0);
      }
    }
    delete[] adrmatrix;
    adrmatrix=tempadr;
    breite=newbreite;
    hoehe=newhoehe;
  }
}
template <class T>
void matrix<T>::fill(const T &fill,int x1,int y1,int x2,int y2) {
  if (adrmatrix) {
    x1%=breite;
    x2%=breite;
    y1%=hoehe;
    y1%=hoehe;
    int t;
    if (x1>x2) { t=x1; x1=x2; x2=t; };
    if (y1>y2) { t=y1; y1=y2; y2=t; };
    int h=y2-y1+1;
    int b=x2-x1+1;
    for (int x=0;x<b;x++)
      for(int y=0;y<h;y++)
	adrmatrix[x+x1+(y+y1)*breite]=fill;
  }
}
template <class T>
void matrix<T>::Store(ostream &wy) {
  int m1,n1;
  for (m1=0;m1<hoehe;m1++) {
    for (n1=0;n1<breite;n1++) wy<<adrmatrix[m1*breite+n1]<<'\t';
    wy<<'\n';
  }
}
template <class T>
void matrix<T>::Restore(istream & we) {
  int m1,n1;
  for (m1=0;m1<hoehe;m1++)
    for (n1=0;n1<breite;n1++) we>>adrmatrix[m1*breite+n1];
}


#endif












