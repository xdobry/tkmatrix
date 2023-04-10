//
// This is a wrapper C++ class for gmp 
// (GNU multiple precision arithmetic library, version 2.0.2)
// float mpf_t
// Written by Artur Trzewik
// 02.2000
// 
//

#include "widefloat.h"
//#include <string>
#include <stdio.h>
void widefloat::set_precision(long int p) {
  mpf_set_default_prec (p);
}
widefloat::widefloat() {
  mpf_init(mywidefloat);
  // fprintf(stderr,"Initialising Widefloat 0 %p\n",this);
}
widefloat::widefloat(long int a) {
  mpf_init(mywidefloat);
  mpf_set_si(mywidefloat,a);
  // fprintf(stderr,"Initialising Widefloat 1 %p\n",this);
}
widefloat::widefloat(int a) {
  mpf_init(mywidefloat);
  mpf_set_si(mywidefloat,(long int)a);
  // fprintf(stderr,"Initialising Widefloat 1 %p\n",this);
}
widefloat::widefloat(double a)  {
  // fprintf(stderr,"Initialising Widefloat 3 %p\n",this);
  mpf_init(mywidefloat);
  mpf_set_d(mywidefloat,a);
}
widefloat::widefloat(const rational &r) {
  mpf_init(mywidefloat);
  mpf_set_q(mywidefloat,r.myrational);
  // mpf_set_q lose minus sugn !! probably a Bug by gmp-library
  if (mpq_sgn(r.myrational)<0 && mpf_sgn(mywidefloat)>0) {
    mpf_neg(mywidefloat,mywidefloat);
  } 
}
widefloat::widefloat(const char *str) {
  mpf_init(mywidefloat);
  mpf_set_str(mywidefloat,str,10);
}
widefloat::widefloat(const widefloat &wf) {
  // fprintf(stderr,"copy  Widefloat %p\n",this);
  mpf_init(mywidefloat);
  mpf_set(mywidefloat,wf.mywidefloat);
}
widefloat & widefloat::operator = (const widefloat &r) {
  // fprintf(stderr,"copy = Widefloat %p\n",this);
  if (this!=&r) {
    mpf_set(mywidefloat,r.mywidefloat);
  }
  return *this;
}
widefloat::~widefloat() {
  // fprintf(stderr,"clearing Widefloat %p\n",this);
  mpf_clear(mywidefloat);
}

widefloat widefloat::inv()  const {
  widefloat w;
  mpf_ui_div(w.mywidefloat,1,w.mywidefloat);
  return w;
}

widefloat widefloat::betrag() const {
  widefloat w(*this);
  mpf_abs(w.mywidefloat,mywidefloat);
  return w;
}
int widefloat::sign() const {
  return mpf_sgn(mywidefloat);
}
widefloat widefloat::operator + (const widefloat &y) const {
  widefloat w;
  mpf_add(w.mywidefloat,mywidefloat,y.mywidefloat);
  return w;
}
widefloat widefloat::operator - ()  const{
  widefloat w;
  mpf_neg(w.mywidefloat,mywidefloat);
  return w;
}
widefloat widefloat::operator - (const widefloat &y)  const{
  widefloat w;
  mpf_sub(w.mywidefloat,mywidefloat,y.mywidefloat);
  return w;
}

widefloat widefloat::operator * (const widefloat &y)  const{
  widefloat w;
  mpf_mul(w.mywidefloat,mywidefloat,y.mywidefloat);
  return w;
}
widefloat widefloat::operator / (const widefloat & y)  const{
  widefloat w;
  mpf_div(w.mywidefloat,mywidefloat,y.mywidefloat);
  return w;
}
void widefloat::operator += (const widefloat &y) {
  mpf_add(mywidefloat,mywidefloat,y.mywidefloat);
}
void widefloat::operator -= (const widefloat &y) {
  mpf_sub(mywidefloat,mywidefloat,y.mywidefloat);
}
void widefloat::operator *= (const widefloat &y) {
  mpf_mul(mywidefloat,mywidefloat,y.mywidefloat);
}
void widefloat::operator /= (const widefloat &y) {
  mpf_div(mywidefloat,mywidefloat,y.mywidefloat);
}

bool widefloat::operator < (const widefloat & y)  const{
  if (mpf_cmp(mywidefloat,y.mywidefloat)<0) 
    return true;
  else
    return false;
}
bool widefloat::operator == (const widefloat & y) const {
  if (mpf_cmp(mywidefloat,y.mywidefloat)==0) 
    return true;
  else
    return false;
}
ostream & operator << (ostream & wy,widefloat & x) {
  char *s;
  mp_exp_t exp;
  int sign=mpf_sgn(x.mywidefloat);
  s=mpf_get_str ((char *)0, &exp , 10, 0, x.mywidefloat);
  if (sign<0)
    wy<<"-0."<<(s+1);
  else if (sign>0) 
    wy<<"0."<<s;
  else
    wy<<'0'<<endl;

  if (exp!=0) wy<<"e"<<exp;
  delete s;
  return wy;
}

istream & operator >> (istream & we,widefloat & a) {
  char c;
  string s;
  if (!we.eof()) {
    do {
      we.get(c);
    } while (!we.eof() && isspace(c));
    s+=c;
    do {
      we.get(c);
      if (!isdigit(c) && c!='e' && c!='E' && c!='.' && c!='-') break;
      s+=c;
    } while (!we.eof());
    mpf_set_str (a.mywidefloat,s.c_str(),10);
    we.putback(c);
  }
  return we;
}

string* widefloat::tostring(bool reformat,int showprec, int maxnum, int maxzeros) const {
  char *s,*s2;
  char buf[30];
  mp_exp_t exp;
  string *ret;
  int sign=mpf_sgn(mywidefloat);
  if (sign==0) {
    ret=new string("0");
  } else {
    s=mpf_get_str (0, &exp, 10, showprec, mywidefloat);
    if (sign<0) {
      s2=s+1; // ignore '-' sign
      ret=new string("-");
    } else { 
      s2=s;
      ret=new string();
    }
    if (exp==0) {
      (*ret) += "0.";
      (*ret) += s2;
    } else {
      if (reformat && exp>0  && maxnum>exp) {
	if (exp>=strlen(s2)) {
	  (*ret) += s2;
	  (*ret) += string(exp-strlen(s2),'0');
	} else {
	string temps(s2);
	ret->append(temps,0,exp);
	(*ret)+='.';
	ret->append(temps,0,exp);
	}
      } else if (reformat && exp<0 && -exp<maxzeros) {
	(*ret) += "0.";
	(*ret) += string(-exp,'0');
	(*ret) += s2;
      } else {
	(*ret) += "0.";
	(*ret) += s2; 
	sprintf(buf,"e%ld",exp);
	(*ret)+=buf;
      } 
    } 
    delete s;
  }
  return ret;
}
widefloat::operator double()  const{
  return mpf_get_d(mywidefloat);
}
double widefloat::todouble() const {
  return mpf_get_d(mywidefloat);
}

