#include <iostream.h>
class reel {
	int l,m;
public:
	reel();
	reel(int,int=1);
	void print ();
	void skroc ();
    reel inv(); // gibt die Inverse zurück 1/x
	reel abs(); // gibt den Betrag von reel
	int operator [] (int); //gibt für 0 zaehler und für 1 nenner zurück
	friend reel operator + (reel &,reel &);
	friend reel operator - (reel &,reel &);
	friend reel operator * (reel &,reel &);
	friend reel operator / (reel &,reel &);
	friend int operator > (reel &,reel &);
	friend int operator >= (reel &,reel &);
	friend int operator == (reel &,reel &);
	friend reel operator - (reel &);
	friend ostream & operator << (ostream &,reel &);
	friend istream & operator >> (istream &,reel &); // wymaga poprawy
};
int abs(int); // prototyp aus c library 
int ggt(int a,int b) {
	a=abs(a);
	b=abs(b);
	int c;
	while (a!=0) {
		if (a>b) a=a%b; else { c=a; a=b%a; b=c; }
	}
	return b;
}
void reel::skroc () {
	int a=ggt(l,m);
	l=l/a;
	m=m/a;
	if (m<0) { l=-l; m=-m; }
}
reel reel::inv() {
	reel w;
	w.l=m;
	w.m=l;
	return w;
}
reel reel::abs() {
	reel w;
	w.l=abs(l);
	w.m=abs(m);
	return w;
}
int sign(int a) {
	return (a > 0 ? 1:-1);
}
reel::reel() {
	l=0;
	m=1;
}
reel::reel(int a,int b) {
	l=a*sign(b);
	m=(b==0 ? 1: abs(b));
}
int reel::operator [] (int x) {
	if (x==0) return l;
	else return m;
} 
reel operator + (reel &x, reel &y) {
	reel w;
	int a=ggt(x.m,y.m);
	a=x.m/a*y.m;		// wspolny mianownik
	w.l=x.l*(a/x.m)+y.l*(a/y.m);
	w.m=a;
	w.skroc();
	return w
}
reel operator - (reel &x) {
	reel w;
	w.l=-x.l;
	w.m=x.m;
	return w;
}
reel operator - (reel &x,reel &y) {
	reel w;
	int a=ggt(x.m,y.m);
	a=x.m/a*y.m;		// wspolny mianownik
	w.l=x.l*(a/x.m)-y.l*(a/y.m);
	w.m=a;
	w.skroc();
	return w;
}
reel operator * (reel & x,reel & y) {
	reel w1,w2;
	w1.l=x.l;	w1.m=y.m;	w1.skroc();
	w2.l=y.l;	w2.m=x.m;	w2.skroc();
	w1.l=w1.l*w2.l;
	w1.m=w1.m*w2.m;
	return w1;
}
reel operator / (reel & x,reel & y) {
	reel w1,w2;
	w1.l=x.l;	w1.m=y.l;	w1.skroc();
	w2.l=y.m;	w2.m=x.m;	w2.skroc();
	w1.l=w1.l*w2.l;
	w1.m=w1.m*w2.m;
	if (w1.m<0) { w1.l=-w1.l; w1.m=-w1.m; }
	return w1;
}
int operator > (reel & x,reel & y) {
	reel	w;
	w=x-y;
	if (w.l>0) return 1;
	else return 0;
}
int operator >= (reel & x,reel & y) {
	reel	w;
	w=x-y;
	if (w.l>=0) return 1;
	else return 0;
}
int operator == (reel & x,reel & y) {
	reel w;
	w=x-y;
	if (w.l==0) return 1;
	else return 0;
}
ostream & operator << (ostream & wy,reel & x) {
	wy << x.l;
	if (x.l!=0 && x.m!=1) wy<< "/" << x.m;
	return wy;
}
istream & operator >> (istream & we,reel & a) {
	char c='\0';
	int x,y=1;
	double d;
	we >>d; x=d;
	we.get(c);
	if (c=='/') { we>>d; y=d; }
	a.l=x; a.m=y;
	a.skroc();
	return we;
}
void reel::print () {
	cout << l;
	if (l!=0 && m!=1) cout <<"/"<< m;
}
