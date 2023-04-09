#include <iostream>
#include <cmath>
#include "reel_class.c"

using namespace std;

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
reel reel::inv() const {
	reel w;
	w.l=m;
	w.m=l;
	return w;
}
reel reel::abs() const {
	reel w;
	w.l=std::abs(l);
	w.m=std::abs(m);
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
	m=(b==0 ? 1: std::abs(b));
}
int reel::operator [] (int x) {
	if (x==0) return l;
	else return m;
} 
reel operator + (const reel &x, const reel &y) {
	reel w;
	int a=ggt(x.m,y.m);
	a=x.m/a*y.m;		// wspolny mianownik
	w.l=x.l*(a/x.m)+y.l*(a/y.m);
	w.m=a;
	w.skroc();
	return w;
}
reel operator - (const reel &x) {
	reel w;
	w.l=-x.l;
	w.m=x.m;
	return w;
}
reel operator - (const reel &x,const reel &y) {
	reel w;
	int a=ggt(x.m,y.m);
	a=x.m/a*y.m;		// wspolny mianownik
	w.l=x.l*(a/x.m)-y.l*(a/y.m);
	w.m=a;
	w.skroc();
	return w;
}
reel operator * (const reel & x,const reel & y) {
	reel w1,w2;
	w1.l=x.l;	w1.m=y.m;	w1.skroc();
	w2.l=y.l;	w2.m=x.m;	w2.skroc();
	w1.l=w1.l*w2.l;
	w1.m=w1.m*w2.m;
	return w1;
}
reel operator / (const reel & x,const reel & y) {
	reel w1,w2;
	w1.l=x.l;	w1.m=y.l;	w1.skroc();
	w2.l=y.m;	w2.m=x.m;	w2.skroc();
	w1.l=w1.l*w2.l;
	w1.m=w1.m*w2.m;
	if (w1.m<0) { w1.l=-w1.l; w1.m=-w1.m; }
	return w1;
}
int operator > (const reel & x,const reel & y) {
	reel w;
	w=x-y;
	if (w.l>0) return 1;
	else return 0;
}
int operator >= (const reel & x,const reel & y) {
	reel w;
	w=x-y;
	if (w.l>=0) return 1;
	else return 0;
}
int operator == (const reel & x,const reel & y) {
	reel w;
	w=x-y;
	if (w.l==0) return 1;
	else return 0;
}
ostream & operator << (ostream & wy,const reel & x) {
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
void reel::print () const {
	cout << l;
	if (l!=0 && m!=1) cout <<"/"<< m;
}
