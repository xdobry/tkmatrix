#if !defined(widefloat_class)
#define widefloat_class
//
// This is a wrapper C++ class for gmp 
// (GNU multiple precision arithmetic library, version 2.0.2)
// float mpf_t
// Written by Artur Trzewik
// 02.2000
// 
//

#include <iostream>
#include <string>
#include <gmp.h>
#include "rational.h"

class widefloat {
  mpf_t mywidefloat;
public:
  widefloat();                        // Konstruktoren  Bruch=0
  widefloat(long int);
  widefloat(int);
  widefloat(const rational &);    // Aus rational
  widefloat(double);  
  widefloat(const char *); // Zeiger auf String
  widefloat(const widefloat &);
  widefloat & operator = (const widefloat &);
  ~widefloat();
  
  static void set_precision(long int);

  void print () const;
  widefloat inv() const; // gibt die Inverse zurck 1/x
  widefloat betrag() const; // gibt den Betrag von widefloat
  int sign() const;  // Vereinfacht die Prfung auf Nullwert  Sign von widefloat
  // -1 wenn rell negativ, 0 wen rell gleich Null und 1 wenn widefloat positiv
  string* tostring(bool reformat,int showprec, int maxnum, int maxzeros) const;
  // der zurckgegebene string mu�mit delete freigegeben werden

  double todouble() const;

  widefloat operator + (const widefloat &) const;
  widefloat operator - (const widefloat &) const;
  widefloat operator * (const widefloat &) const;
  widefloat operator / (const widefloat &) const;
  
  void operator += (const widefloat &);
  void operator -= (const widefloat &);
  void operator *= (const widefloat &);
  void operator /= (const widefloat &);

  // Die 3 n�hsten waren auch friend, in GNU geht das aber nicht weil in
  // funktion.h freundlicherweise templates dafr deklariert waren
  // fr !=,>,<=,>=
  bool operator < (const widefloat &)const;
  bool operator == (const widefloat &)const;

  widefloat operator - ()const;
  friend ostream & operator << (ostream &,widefloat &);    // Schreibt einhalbe als 1/2
  friend istream & operator >> (istream &,widefloat &);    // List 1,2 oder 1/2 als Einhalbe; 1,1,2 Eineinhalb
private:
  operator double() const; // Konversion widefloat zu double
};

inline widefloat inv(widefloat &r) { return r.inv(); }
inline widefloat betrag(widefloat &r) { return r.betrag(); }
inline int sign(widefloat &r) { return r.sign(); }

#endif
