
#if !defined(rational_class)
#define rational_class
//
// Modul fr Rationale Zahle (Brche) 
// in abh�gigkeit von der definition NO_GMP wird eine 
// Langzahlarithmetik der Biblothek gmp 
// (GNU multiple precision arithmetic library)
// oder eine eigene Impementierung von rationallen Zahlen benutzt,
// die allerdings begrentzte Genauigkeit besitzt

#include <iostream>
#include <string>

using namespace std;

//#define NO_GMP 1
#if !defined(NO_GMP)
#include <gmp.h>

class widefloat;

class rational {
  mpq_t myrational;
public:
  rational();                        // Konstruktoren  Bruch=0
  rational(long int);
  rational(long int,long int);    // Z�ler+Nenner
  rational(long int, long int, long int);  //Ganzah+Z�ler+Nenner
  // argc can be 0,1,2 or 3. Array holds pointer to strings from number
  rational(int argc,const char *argv[]);
  rational(const rational &);
  rational & operator = (const rational &);
  ~rational();

  void print () const;
  void skroc ();
  rational inv() const; // gibt die Inverse zurck 1/x
  rational betrag() const; // gibt den Betrag von rational
  int sign() const;  // Vereinfacht die Prfung auf Nullwert  Sign von rational
  // -1 wenn rell negativ, 0 wen rell gleich Null und 1 wenn rational positiv
  string* tostring(bool mitganzzahl=true) const; // Macht rational zu string mit Screibweise "-1,1,2"=(-1.5) "-,1,2"=(-0.5) "0,,"=(0) "5,,"=5
  // der zurckgegebene string mu�mit delete freigegeben werden
  // ist mitganzahlt false wird von 1.5= ",3,2"
  int loadstring(string &ganzzahl,string &zaehler,string &nenner,bool mitganzzahl=true) const;
  // macht das gleiche wie tostring aber fgt die Komponenten nicht Zusammen
  // und gibt die Art der Zahl
  // return Wert 0-nur Ganzahl 1-nur Bruch 2-alles 3-Null

  // Es war n�ig um protstream zu implementieren sonst immer todouble() benutzten
  double todouble() const;

  rational operator + (const rational &) const;
  rational operator - (const rational &) const;
  rational operator * (const rational &) const;
  rational operator / (const rational &) const;
  
  void operator += (const rational &);
  void operator -= (const rational &);
  void operator *= (const rational &);
  void operator /= (const rational &);

  // Die 3 n�hsten waren auch friend, in GNU geht das aber nicht weil in
  // funktion.h freundlicherweise templates dafr deklariert waren
  // fr !=,>,<=,>=
  // include <functional>
  // using namespace rel_ops
  bool operator < (const rational &) const;
  bool operator == (const rational &) const;

  rational operator - ()const;
  friend ostream & operator << (ostream &,rational &);    // Schreibt einhalbe als 1/2
  friend istream & operator >> (istream &,rational &);    // List 1,2 oder 1/2 als Einhalbe; 1,1,2 Eineinhalb
  friend class widefloat;
private:
  operator double() const; // Konversion rational zu double
  long int operator [] (int) const;  // gibt fr 0 GanzZahlen 1 zaehler und fr 2 Nenner zurck
};

inline rational inv(rational &r) { return r.inv(); }
inline rational betrag(rational &r) { return r.betrag(); }
inline int sign(rational &r) { return r.sign(); }

double inv(double a);
double betrag(double a);
int sign(double a);

// Deklaration von Prozeduren fr die Behandlung von Kombinationnen ohne Wiederholungen
int kombie_zahl (int x,int y);
int kombie_index(int *s,int menge,int kombie);

class mylongint {
public:
  class overflow {};
};

#else

// Klasse mylongint
// funktionirt wie gew�nliches long int
// beim �erl�fen (beim operationen wie +,-,/)
// wird eine Fehlerklasse mylongint::overflow eingeworfen
// wegen friend Funktion beim operator>> ist diese Klasse in dieser Form nicht geeignet
// andere Klassen daraus abzuleiten
class mylongint {
public:
  long int value;
  mylongint(long int v=0):value(v) {}
  mylongint operator + ( const mylongint&) const;
  mylongint operator * ( const mylongint&) const;
  mylongint operator - ( const mylongint&) const;
  mylongint operator *= (mylongint &a) { value=value*a.value; return *this; }
  mylongint operator -= (mylongint &a) { value=value-a.value; return *this; }
  mylongint operator += (mylongint &a) { value=value+a.value; return *this; }
  friend istream& operator>> (istream &,mylongint &a);
  //	mylongint operator / (mylongint& a) { return value/a.value; }
  //	mylongint operator % (mylongint& a) { return value%a.value; }
  operator long int ()  const { return value; }
  double todouble() const { return (double)value; }
  class overflow {};
};
// dieses mal neue Klasse mit ganzen Zahl und Bruch
// Version 3.0 um �erl�fe abfangen zu k�nen wird jetzt die klasse mylingint staat long int benutzt
// die Fehlerklasse mu�aber in "h�eren" Modulen abgefangen werden
class rational {
  mylongint z,l,m;  // z GanzeZahlen mit Vorzeichen, l-Z�ler mit Vorzeichen, m -Nenner
  // Darstellung
  // Null z=0,l=0,m=1
  // Eins z=1,l=0,m=1
  // Minus Einhalbe z=0,l=-1,m=2
  // Minus Ein Einhalb z=-1,l=-1,m=2
  // Aufl�eformel fr Bruch lautet z+l/m
public:
  rational();                        // Konstruktoren  Bruch=0
  rational(long int);
  rational(long int,long int);    // Z�ler+Nenner
  rational(long int, long int, long int);  //Ganzah+Z�ler+Nenner
  rational(int argc,char *argv[]);
  void print () const;
  void skroc ();
  rational inv() const; // gibt die Inverse zurck 1/x
  rational betrag() const; // gibt den Betrag von rational
  int sign() const;  // Vereinfacht die Prfung auf Nullwert  Sign von rational
  // -1 wenn rell negativ, 0 wen rell gleich Null und 1 wenn rational positiv
  string* tostring(bool mitganzzahl=true) const; // Macht rational zu string mit Screibweise "-1,1,2"=(-1.5) "-,1,2"=(-0.5) "0,,"=(0) "5,,"=5
  // der zurckgegebene string mu�mit delete freigegeben werden
  // ist mitganzahlt false wird von 1.5= ",3,2"
  int loadstring(string &ganzzahl,string &zaehler,string &nenner,bool mitganzzahl=true);
  // macht das gleiche wie tostring aber fgt die Komponenten nicht Zusammen
  // und gibt die Art der Zahl
  // return Wert 0-nur Ganzahl 1-nur Bruch 2-alles 3-Null
  operator double() const; // Konversion rational zu double
  // 2.3.98 ver�derung von frien auf Memberfunktionnen
  rational operator + (const rational &) const;
  rational operator - (const rational &) const;
  rational operator * (const rational &) const;
  rational operator / (const rational &) const;

  void operator += (const rational &);
  void operator -= (const rational &);
  void operator *= (const rational &);
  void operator /= (const rational &);

  // Die 3 n�hsten waren auch friend, in GNU geht das aber nicht weil in
  // funktion.h freundlicherweise templates dafr deklariert waren
  // fr !=,>,<=,>=
  bool operator < (const rational &)const;
  bool operator == (const rational &)const;
  rational operator - ()const;
  friend ostream & operator << (ostream &,rational &);    // Schreibt einhalbe als 1/2
  friend istream & operator >> (istream &,rational &);    // List 1,2 oder 1/2 als Einhalbe; 1,1,2 Eineinhalb
private:
  long int operator [] (int) const;  // gibt fr 0 GanzZahlen 1 zaehler und fr 2 Nenner zurck
};

inline rational inv(rational &r) { return r.inv(); }
inline rational betrag(rational &r) { return r.betrag(); }
inline int sign(rational &r) { return r.sign(); }

#endif
#endif
