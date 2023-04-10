#include "rational.h"
#include <string>
#include <stdio.h>

#if !defined(NO_GMP)

rational::rational() {
  mpq_init(myrational);
  // fprintf(stderr,"Initialising Rational 0 %p\n",this);
}
rational::rational(long int a) {
  mpq_init(myrational);
  mpq_set_si(myrational,a,1ul);
  // fprintf(stderr,"Initialising Rational 1 %p\n",this);
}
rational::rational(long int a,long int b,long int c)  {
  // fprintf(stderr,"Initialising Rational 3 %p\n",this);
  mpq_t tr1;
  mpq_init(tr1);
  mpq_init(myrational);
  mpq_set_si(tr1,a,1);
  mpq_set_si(myrational,b,c);
  mpq_canonicalize(myrational);
  mpq_add(myrational,tr1,myrational);
  mpq_clear(tr1);
}
rational::rational(long int a,long int b) {
  mpq_init(myrational);
  mpq_set_si(myrational,a,b);
  mpq_canonicalize(myrational);
  // fprintf(stderr,"Initialising Rational 2 %p\n",this);
}
rational::rational(int argc,const char *argv[]) {
  mpq_init(myrational);
  mpq_t tr;
  mpz_t mint;
  switch (argc) {
  case 1:
    mpz_init(mint);
    mpz_set_str (mint,argv[0],10);
    mpq_set_z(myrational,mint);
    mpz_clear(mint);
    break;
  case 2:
    mpz_init(mint);
    mpz_set_str (mint,argv[0],10);
    mpq_set_num(myrational,mint);
    mpz_set_str (mint,argv[1],10);
    mpq_set_den(myrational,mint);
    mpq_canonicalize(myrational);
    mpz_clear(mint);
    break;
  case 3:
    // explore first number
    mpz_init(mint);
    mpq_init(tr);
    mpz_set_str(mint,argv[0],10);
    mpq_set_z(tr,mint);
    // explore fraction
    mpz_set_str (mint,argv[1],10);
    mpq_set_num(myrational,mint);
    mpz_set_str (mint,argv[2],10);
    mpq_set_den(myrational,mint);
    mpq_canonicalize(myrational);
    mpq_add(myrational,myrational,tr);
    mpz_clear(mint);
    mpq_clear(tr);
    break;
  }
}
rational::rational(const rational &r) {
  // fprintf(stderr,"copy  Rational %p\n",this);
  mpq_init(myrational);
  mpq_set(myrational,r.myrational);
}
rational & rational::operator = (const rational &r) {
  // fprintf(stderr,"copy = Rational %p\n",this);
  if (this!=&r) {
    mpq_set(myrational,r.myrational);
  }
  return *this;
}
rational::~rational() {
  // fprintf(stderr,"clearing Rational %p\n",this);
  mpq_clear(myrational);
}

rational rational::inv()  const{
  rational w;
  mpq_inv(w.myrational,myrational);
  return w;
}

rational rational::betrag() const {
  rational w;
  if (mpq_sgn(myrational)==-1) {
    mpq_neg(w.myrational,myrational);
  } else {
    mpq_set(w.myrational,myrational);
  }
  return w;
}
int rational::sign() const {
  return mpq_sgn(myrational);
}
rational rational::operator + (const rational &y) const {
  rational w;
  mpq_add(w.myrational,myrational,y.myrational);
  return w;
}
rational rational::operator - ()  const{
  rational w;
  mpq_neg(w.myrational,myrational);
  return w;
}
rational rational::operator - (const rational &y)  const{
  rational w;
  mpq_sub(w.myrational,myrational,y.myrational);
  return w;
}

rational rational::operator * (const rational &y)  const{
  rational w;
  mpq_mul(w.myrational,myrational,y.myrational);
  return w;
}
rational rational::operator / (const rational & y)  const{
  rational w;
  mpq_div(w.myrational,myrational,y.myrational);
  return w;
}
void rational::operator += (const rational &y) {
  mpq_add(myrational,myrational,y.myrational);
}
void rational::operator -= (const rational &y) {
  mpq_sub(myrational,myrational,y.myrational);
}
void rational::operator *= (const rational &y) {
  mpq_mul(myrational,myrational,y.myrational);
}
void rational::operator /= (const rational &y) {
  mpq_div(myrational,myrational,y.myrational);
}

bool rational::operator < (const rational & y)  const{
  if (mpq_cmp(myrational,y.myrational)<0) 
    return true;
  else
    return false;
}
bool rational::operator == (const rational & y) const {
  if (mpq_cmp(myrational,y.myrational)==0) 
    return true;
  else
    return false;
}
ostream & operator << (ostream & wy,rational & x) {
  char *s;
  s=mpz_get_str (0, 10, mpq_numref(x.myrational));
  wy<<s;
  delete s;
  if (mpz_cmp_si(mpq_denref(x.myrational),1)!=0) {
    wy<<'/';
    s=mpz_get_str (0 , 10, mpq_denref(x.myrational));
    wy<<s;
    delete s;
  }
  return wy;
}
static void read_mpz(istream & we,mpz_t i) {
  mpz_init(i);
  string s;
  char c;
  we>>ws; // ignore blank chars
  if (!we.eof()) {
    do {
      we.get(c);
      if (!isdigit(c) && c!='-') break;
      s+=c;
    } while (!we.eof());
  }
  mpz_set_str (i,s.c_str(),10);
  we.putback(c);
}
istream & operator >> (istream & we,rational & a) {
  char c;
  mpz_t z,l,m;
  if (!we.eof())
    read_mpz(we,z);
  else { mpq_init(a.myrational); return we;}
  if (!we.eof()) {
    we.get(c);
    if (c=='/' || c==',') {
      if (!we.eof()) {  // Wenn nach dem ,/ nicht existiert sollte allerdings Fehlerstatus setzen
	read_mpz(we,l);
	if (!we.eof()) {
	  we.get(c);
	  if (c=='/' || c==',') {
	    if (!we.eof()) {  // Wenn nach dem ,/ nicht existiert sollte allerdings Fehlerstatus setzen
	      read_mpz(we,m);
	      mpq_t tq1,tq2;
	      mpq_init(tq1);
	      mpq_init(tq2);
	      mpq_set_num(tq1,l);
	      mpq_set_den(tq1,m);
	      mpq_canonicalize(tq1);
	      mpz_clear(l);
	      mpz_clear(m);
	      mpq_set_z(tq2,z);
	      mpq_add(a.myrational,tq1,tq2);
	      mpz_clear(z);
	      mpq_clear(tq1);
	      mpq_clear(tq2);
	      return we;
	    }
	  }
	}
	mpq_set_num(a.myrational,z);
	mpq_set_den(a.myrational,l);
	mpq_canonicalize(a.myrational);
	mpz_clear(z);
	mpz_clear(l);
	return we;
      }
    }
  }
  mpq_set_z(a.myrational,z);
  mpz_clear(z);
  return we;
}

string* rational::tostring(bool mitganzzahl) const {
  char *s;
  string *ret;
  bool sminus=false;
  if (mpz_cmp_si(mpq_denref(myrational),1)==0) {
  // Only one number denominator==1
    s = mpz_get_str (0, 10, mpq_numref(myrational));
    ret=new string(s);
    delete s;
    return ret;
  }
  mpz_t numtemp;
  //  mpz_init(numtemp);
  mpz_init_set(numtemp,mpq_numref(myrational));
  if (mpz_cmp_si(numtemp,0)<0) {
    sminus=true;
    mpz_neg(numtemp,numtemp);
  } 
  ret=new string();
  if (!mitganzzahl || mpz_cmp(numtemp,mpq_denref(myrational))<0) {
    // ohne Ganzzahl vor dem Bruch
    s = mpz_get_str (0, 10, numtemp);
    if (sminus)
      (*ret)+="- ";
    (*ret)+=s;
    delete s;
    (*ret)+=' ';
    s = mpz_get_str (0, 10, mpq_denref(myrational));
    (*ret)+=s;
    delete s;
  } else {
    // Eine Ganzzhal vor dem Bruch nenner>zaehler
    if (sminus) 
      (*ret)='-';
    mpz_t gztemp,remainder;
    mpz_init(gztemp);
    mpz_init(remainder);
    mpz_tdiv_qr(gztemp,remainder,numtemp,mpq_denref(myrational));
    s = mpz_get_str (0, 10, gztemp);
    (*ret)+=s;
    delete s;
    (*ret)+=' ';
    s = mpz_get_str (0, 10, remainder);
    (*ret)+=s;
    delete s;
    (*ret)+=' ';
    s = mpz_get_str (0, 10, mpq_denref(myrational));
    (*ret)+=s;
    delete s;
    mpz_clear(gztemp);
    mpz_clear(remainder);
  }
  mpz_clear(numtemp);
  return ret;
}
// return Wert 0-nur Ganzahl 1-nur Bruch 2-alles 3-Null
int rational::loadstring(string &ganzzahl,string &zaehler,string &nenner,bool mitganzzahl) const {
  // ganzzahl.erase();
  // zaehler.erase();
  // nenner.erase();

  //  ganzzahl.remove();
  //  zaehler.remove();
  //  nenner.remove();
  ganzzahl="";
  zaehler="";
  nenner="";
  bool sminus=false;
  int ret;
  char *s;
  if (mpz_cmp_si(mpq_numref(myrational),0)==0) {
  // Number is zero
    ganzzahl="0";
    return 3;
  }
  if (mpz_cmp_si(mpq_denref(myrational),1)==0) {
  // Only one number denominator==1
    s = mpz_get_str (0, 10, mpq_numref(myrational));
    ganzzahl=s;
    delete s;
    return 0;
  }
  mpz_t numtemp;
  //  mpz_init(numtemp);
  mpz_init_set(numtemp,mpq_numref(myrational));
  if (mpz_cmp_si(numtemp,0)<0) {
    sminus=true;
    mpz_neg(numtemp,numtemp);
  } 
  if (!mitganzzahl || mpz_cmp(numtemp,mpq_denref(myrational))<0) {
    // ohne Ganzzahl vor dem Bruch
    s = mpz_get_str (0, 10, numtemp);
    zaehler=s;
    delete s;
    s = mpz_get_str (0, 10, mpq_denref(myrational));
    nenner=s;
    delete s;
    if (sminus) {
      ganzzahl="-";
      ret=2;
    } else 
      ret=1;
  } else {
    ret=2;
    // Eine Ganzzhal vor dem Bruch nenner>zaehler
    if (sminus) 
      ganzzahl='-';
    mpz_t gztemp,remainder;
    mpz_init(gztemp);
    mpz_init(remainder);
    mpz_tdiv_qr(gztemp,remainder,numtemp,mpq_denref(myrational));
    s = mpz_get_str (0, 10, gztemp);
    ganzzahl+=s;
    delete s;
    s = mpz_get_str (0, 10, remainder);
    zaehler=s;
    delete s;
    s = mpz_get_str (0, 10, mpq_denref(myrational));
    nenner=s;
    delete s;
    mpz_clear(gztemp);
    mpz_clear(remainder);
  }
  mpz_clear(numtemp);
  return ret;
}
rational::operator double()  const{
  return mpq_get_d(myrational);
}
double rational::todouble() const {
  return mpq_get_d(myrational);
}
#else

#include <limits.h>
/////////////////////////////////
// Implementierung von mylongint
/////////////////////////////////
mylongint mylongint::operator + ( const mylongint &a) const  {
  if (value>0 && LONG_MAX-value<a.value)
    throw overflow();
  return mylongint(a.value+value);
}
mylongint mylongint::operator - ( const mylongint &a) const {
  if (a.value>0 && LONG_MAX-a.value<-value)
    throw overflow();
  return mylongint(value-a.value);
}
mylongint mylongint::operator * ( const mylongint &a)  const{
  //Hier wrde sich bieten abs(long int) anzuwenden leider ist ins stlib.h abs() nur fr int definiert
  if (value!=0 && (a.value>=0 ? (a.value):-a.value)>LONG_MAX/(value>=0 ? value:-value))
    throw overflow();
  return mylongint(a.value*value);
}
inline istream& operator>> (istream &is,mylongint &a) {
  is>>a.value;
  return is;
}
////////////////////////
// Rational Zahlen Implementierung
////////////////////////
// V3.0 ver�dert auf inline Funktion und mylongint
// die n�hsten Linien braucht man um bei alten egcs und gcc zu compilieren 
// der neue egcs 1.1b mancht einen 
//  in../../gcc/function.c:3272: Internal compiler error in function instantiate_virtual_regs_1
//  line long int abs(mylongint a) {
//     return (a>0 ? a:mylongint(-a));
//    }
//    inline long int abs(long int a) {
//      return (a>0 ? a:-a);
//    }
// alle Operationen auf long int kein �erlauf m�lich
long int ggt(long int a,long int b) {
  a=abs(a);
  b=abs(b);
  long int c;
  while (a!=0) {
    if (a>b) a=a%b; else { c=a; a=b%a; b=c; }
  }
  return b;
}


void rational::skroc () {
  if (abs(l)>=m) {     // wenn Z�ler>Nenner konventieren
    z=z+mylongint(l/m);
    l=l%m;       // l kann evnt. Null werden es hei� es ist eine GanzZahl
  }
  if (l!=0) {
    long int a=ggt(l,m);
    l=l/a;
    m=m/a;
  } else m=1;
}


rational rational::inv()  const{
  rational w;
  if (l==0) {
    if (z==0 || z==1) {
      w.z=z;
      return w; // Null oder Eins
    }	else {
      w.m=z;    // GanzZahl
      w.l=1;
    }
  } else if (z==0) {
    w.l=m;        // Reines Bruch ohne GanzZahl
    w.m=l;
    w.z=m/l;
    w.l=abs(m%l);
  } else {
    w.m=mylongint(m*z)+l;
    w.l=m;
  }
  if (0>(long int)w.m) { w.l=-w.l; w.m=-w.m; }
  return w;
}

rational rational::betrag() const {
  rational w;
  w.z=abs(z);
  w.l=abs(l);
  w.m=m;
  return w;
}
rational::rational():z(0),l(0),m(1) {
}
rational::rational(long int a):z(a),l(0),m(1) {
}
rational::rational(long int a,long int b,long int c):z(abs(a)),l(abs(b)),m(abs(c))  {
  if (m==0) m=1;
  if (0>a || 0>b || 0>c) { l=-l; z=-z; } // wenn a,b oder c <0 ist Bruch negativ
  skroc();
}
rational::rational(long int a,long int b):z(0),l(abs(a)),m(abs(b)) {
  if (m==0) m=1;
  if (0>a || 0>b) l=-l;  // Wenn a oder b <0 ist Bruch als Negativ definiert
  skroc();
}
long int rational::operator [] (int x)  const{
  switch(x) {
  case 0:return z;
  case 1:return l;
  default: return m;
  }
}
rational::rational(int argc,char *argv[]):z(0),l(0),m(1) {
  switch (argc) {
  case 1:
    z=atoi(argv[0]);
    break;
  case 2:
    l=atoi(argv[0]);
    m=atoi(argv[1]);
    break;
  case 3:
    z=atoi(argv[0]);
    l=atoi(argv[1]);
    m=atoi(argv[2]);
    break;
  }
  skroc();
}
int rational::sign()  const{
  if (z==0 && l==0) return 0;
  if (z!=0) return (z>0 ? 1:-1);
  else return (l>0 ? 1:-1);
}
rational rational::operator + (const rational &y) const {
  rational w;
  long int a=ggt(m,y.m);
  a=mylongint(m/a)*y.m;		// wspolny mianownik
  w.l=l*mylongint(a/m)+y.l*mylongint(a/y.m);
  w.m=a;
  w.z=z+y.z;
  if (w.z>0 && 0>w.l) { w.z=w.z-mylongint(1); w.l=w.m+w.l; }
  if (0>w.z && w.l>0) { w.z=w.z+mylongint(1); w.l=-(w.m-w.l); }
  w.skroc();
  return w;
}
rational rational::operator - ()  const{
  return rational(-z,-l,m);
}
rational rational::operator - (const rational &y)  const{
  rational temp(-y);
  return (*this)+temp;
}

rational rational::operator * (const rational &y)  const{
  long int a,b;
  rational w,wt;    // nach dem Prinzip (a+b)*(c+d)=ac+ad+bc+bd ,wobei (a+b) ist a=z und b=l/m
  if (z!=0) {
    if(y.z!=0) {
      w.z=z*y.z;    // a*c
      w.l=0; w.m=1;  // eine GanzZahl Manchen
    }
    if (y.l!=0) {
      a=ggt(z,y.m);     // a*d
      wt.l=mylongint(z/a)*y.l;
      wt.m=y.m/a;
      w=w+wt;
    }
  }
  if (l!=0) {
    if (y.z!=0) {
      a=ggt(m,y.z);
      wt.l=mylongint(y.z/a)*l;
      wt.m=m/a;
      w=w+wt;
    }
    if (y.l!=0) {
      a=ggt(l,y.m);
      b=ggt(y.l,m);
      wt.l=mylongint(l/a)*mylongint(y.l/b);
      wt.m=mylongint(m/b)*mylongint(y.m/a);
      w=w+wt;
    }
  }
  w.skroc();
  return w;
}
rational rational::operator / (const rational & y)  const{
  // Nach dem Prinzip a/c=a*1/c
  rational temp(y.inv());
  return (*this)*temp;
}
void rational::operator += (const rational &r) {
  *this=*this+r;
}
void rational::operator -= (const rational &r) {
  *this=*this-r;
}
void rational::operator *= (const rational &r) {
  *this=*this*r;
}
void rational::operator /= (const rational &r) {
  *this=*this/r;
}
bool rational::operator < (const rational & y)  const{
  rational	w=(*this)-y;
  if (w.sign()<0) return true;   // die Konversion w zu int ist n�ig bei Ganzzahlversion
  else return false;
}
bool rational::operator == (const rational & y) const {
  rational w=(*this)-y;
  if (w.sign()==0) return true;
  else return false;
}
ostream & operator << (ostream & wy,rational & x) {
  if (x.z!=0) { wy<<x.z;
  if (x.l!=0)  wy<<'/'<<abs(x.l);
  else return wy;
  } else
    if(x.l!=0) wy<<x.l;
    else { wy<<0; return wy; }
  wy<<'/'<<x.m;
  return wy;
}
istream & operator >> (istream & we,rational & a) {
  char c;
  long int z,l,m;
  if (!we.eof())
    we>>z;
  else { a.z=0; a.l=0; a.m=1; return we;}
  if (!we.eof()) {
    we.get(c);
    if (c=='/' || c==',') {
      if (!we.eof()) {  // Wenn nach dem ,/ nicht existiert sollte allerdings Fehlerstatus setzen
	we>>l;
	if (!we.eof()) {
	  we.get(c);
	  if (c=='/' || c==',') {
	    if (!we.eof()) {  // Wenn nach dem ,/ nicht existiert sollte allerdings Fehlerstatus setzen
	      we>>m;
	      a=rational(z,l,m);
	      return we;
	    }
	  }
	}
	a=rational(z,l); return we;
      }
    }
  }
  a=rational(z);
  return we;
}
// schreibt die Zahl x*y zu string
// Vorbedingungen x,y>0 und r=0
string twointtostring(long int x,long int y,long int rest) {
  if ((x!=0) && (((LONG_MAX/x)<y) || ((LONG_MAX-x*y)<rest))) {
    long int z,yr,t,r;
    if (x<y) { z=x; x=y; y=z; }
    r=x%10;
    x/=10;
    if ( ((LONG_MAX/10)<y)||((LONG_MAX-y*r)<rest) ) {
      yr=y%10;
      t=yr*r+rest;
      z=t%10;
      rest=y/10*r+t/10;
    } else {
      yr=y*r+rest;
      z=yr%10;
      rest=yr/10;
    }
    return twointtostring(y,x,rest)+string(1,char(z+'0'));
  } else {
    char p[12];
    sprintf(p,"%ld",x*y+rest);
    return string(p);
  }
}
string* rational::tostring(bool mitganzzahl) const {
  char w[36]; // es soll reichen fr 32 bit rational Zahlen;
  if (mitganzzahl) {
    if (z==0 && l==0) strcpy(w,"0");
    else if(z==0 && l<0) sprintf(w,"- %ld %ld",(long int)abs(l),(long int)m);
    else if(z==0) sprintf(w,"%ld %ld",(long int)abs(l),(long int)m);
    else if(l==0) sprintf(w,"%ld",(long int)z);
    else sprintf(w,"%ld %ld %ld",(long int)z,(long int)abs(l),(long int)m);
  } else {
    if (z==0 && l==0) strcpy(w,"0");
    else if(z==0 && l<0) sprintf(w,"- %ld %ld",(long int)abs(l),(long int)m);
    else if(z==0) sprintf(w,"%ld %ld",(long int)abs(l),(long int)m);
    else if(l==0) sprintf(w,"%ld",(long int)z);
    else {
      string temp =twointtostring((long int)abs(z),(long int)abs(m),(long int)abs(l)); 
      if(l<0)  sprintf(w,"- %s %ld",temp.c_str(),(long int)m);
      else sprintf(w,"%s %ld",temp.c_str(),(long int)m);
    }
  }
  string* wynik=new string(w);
  return wynik;
}
// return Wert 0-nur Ganzahl 1-nur Bruch 2-alles 3-Null
int rational::loadstring(string &ganzzahl,string &zaehler,string &nenner,bool mitganzzahl=true) {
  // ganzzahl.erase();
  // zaehler.erase();
  // nenner.erase();

  //  ganzzahl.remove();
  //  zaehler.remove();
  //  nenner.remove();
  ganzzahl="";
  zaehler="";
  nenner="";

  if (z==0 && l==0) { ganzzahl="0"; return 3;}
  char sg[12],sz[12],sn[12];
  sprintf(sg,"%ld",(long int)z);
  sprintf(sz,"%ld",(long int)abs(l));
  sprintf(sn,"%ld",(long int)m);
  if(z==0 && l<0) {
    ganzzahl="-";
    zaehler=sz;
    nenner=sn;
    return 2;
  } else if(z==0) {
    zaehler=sz;
    nenner=sn;
    return 1;
  } else if(l==0) {
    ganzzahl=sg;
    return 0;
  } if (mitganzzahl) {
    zaehler=sz;
    nenner=sn;
    ganzzahl=sg;
    return 2;
  } else {
      zaehler=twointtostring((long int)abs(z),(long int)abs(m),(long int)abs(l)); 
      nenner=sn;
      if(l<0) {
	ganzzahl="-";
	return 2;
      } else {
	return 1;
      }
  }
}
rational::operator double()  const{
  return (l==0)? z.todouble():z.todouble()+l.todouble()/m.todouble();
}
#endif
