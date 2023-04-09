#include <iostream>

using namespace std;

#ifndef reel_class
#define reel_class
class reel {
	int l,m;
public:
	reel();
	reel(int,int=1);
	void print () const;
	void skroc ();
	reel abs() const; // gibt den Betrag von reel
    reel inv() const; // gibt die Inverse zurück 1/x
	int operator [] (int);  // gibt für 0 zaehler und für 1 nenner zurück
	friend reel operator + (const reel &,const reel &);
	friend reel operator - (const reel &,const reel &);
	friend reel operator * (const reel &,const reel &);
	friend reel operator / (const reel &,const reel &);
	friend int operator > (const reel &,const reel &);
	friend int operator >= (const reel &,const reel &);
	friend int operator == (const reel &,const reel &);
	friend reel operator - (const reel &);
	friend ostream & operator << (ostream &,const reel &);
	friend istream & operator >> (istream &,reel &);
};
#endif
