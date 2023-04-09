#include <iostream.h>
#ifndef reel_class
#define reel_class
class reel {
	int l,m;
public:
	reel();
	reel(int,int=1);
	void print ();
	void skroc ();
    reel inv(); // gibt die Inverse zurück 1/x
	reel betrag(); // gibt den Betrag von reel
	int operator [] (int);  // gibt für 0 zaehler und für 1 nenner zurück
	friend reel operator + (reel &,reel &);
	friend reel operator - (reel &,reel &);
	friend reel operator * (reel &,reel &);
	friend reel operator / (reel &,reel &);
	friend int operator > (reel &,reel &);
	friend int operator >= (reel &,reel &);
	friend int operator == (reel &,reel &);
	friend reel operator - (reel &);
	friend ostream & operator << (ostream &,reel &);
	friend istream & operator >> (istream &,reel &);
};
#endif
