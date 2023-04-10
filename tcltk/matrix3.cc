/* Dies ist eine Zusamenensetzung von 3 Codeteile sets rational und matrix die allgemein nur der
Aufgabe der matrizen Manipulationen dienen
*/
#include "matrix3.h"
#include "protstream.h"
#include <cstdio>


#ifdef DEBUG
int MatrixObject::matrixcount=0;
int MatrixObject::matrixlive=0;
#endif

#ifdef DEBUG
MatrixObject::MatrixObject(protstream &prot):prot(prot) {
  matrixnum=matrixcount++;
  fprintf(stderr,"matrix object num %d created %d lived\n",matrixnum,++matrixlive);
}
MatrixObject::~MatrixObject() {
  fprintf(stderr,"matrix object num %d deleted %d lived\n",matrixnum,matrixlive--);
}
#else
MatrixObject::MatrixObject(protstream &prot):prot(prot) {}
MatrixObject::~MatrixObject() {}
#endif

int MatrixObject::nbreite(void)const { return breite; }
int MatrixObject::mhoehe(void)const { return hoehe; }


///////////////
// Sets Implementierung
//////////////
// Ich hatte hier Riesenprobleme, weil 1<<17 wegen 16 Bit zu 0 wurde
// Bei reinen 32 Bit Machinnen (wie AMIGA) w�e das nicht passiert !!

int hmint(int a) {
  return (a+31)>>5;
}
sets::sets(int lenght) {
  int x,lint=hmint(lenght);
  leng=lenght;
  adr=new long int[lint];
  for (x=0;x<lint;x++) adr[x]=0;
}
sets::sets(const sets & z) {
  pos=z.pos;
  leng=z.leng;
  int x,lint=hmint(leng);
  adr=new long int[lint];
  for(x=0;x<lint;x++) adr[x]=z.adr[x];
}
sets & sets::operator = (const sets & z) {
  if (this!=&z) {
    delete adr;
    int x,lint=hmint(z.leng);
    adr=new long int[lint];
    leng=z.leng;
    pos=z.pos;
    for (x=0;x<lint;x++) adr[x]=z.adr[x];
  }
  return *this;
}
sets::~sets() {
  delete adr;
}
void sets::clear() {
  int x,l=hmint(leng);
  for(x=0;x<l;x++) adr[x]=0;
}
void sets::add(int x) {
  if (x<leng)
    adr [x>>5]|=1ul<<(x&0x1F);
}
void sets::del(int x) {
  if (x<leng)
    adr[x>>5]&=0xFFFFFFFFul^(1ul<<(x&0x1F)); // adr=adr AND ($FFFFFF XOR bit)
}
void sets::sxor(int x) {
  if (x<leng)
    adr[x>>5]^=(1ul<<(x&0x1F)); // adr=adr XOR bit
}
int sets::exists(int x) const {
  return ((adr[x>>5]&(1ul<<(x&0x1F)))!=0 ? 1 : 0); // um bei 32 to 16 bit convertierung Fehler zu vermeiden
}
void sets::init() {
  pos=0;
}
int sets::next() {
  return exists(pos++);
}
int sets::next_full() {
  while (!exists(pos) && pos<leng) pos++;
  if (pos<leng)
    return pos++;
  else
    return -1;
}
int sets::all() const {
  return pos>=leng;
}
int sets::howmany() const {
  int x,erg=0;
  for (x=0;x<leng;x++) {
    if (exists(x)) erg++;
  }
  return erg;
}
// Funktionne die bei double nicht definert sind aber von matrix<t> gebraucht werden
double inv(double a) {
  return 1/a;
}
double betrag(double a) {
  return (a>0 ? a:-a);
}
int sign(double a) {
  if (a>0) return 1;
  else if (a<0) return -1;
  else return 0;
}
// Diese Methode berechnet die Anzahl der Kombinationnen ohne
// Wiederholungen (x ber y)= x! / y! * (y-x)!
// x = Anzahl der Elemente der Wahlmenge
// y = Anzahl der Ziehungen
int kombie_zahl (int x,int y) {
  if (x==y) return 1;
  if (y==0 || y>x) return 0;
  if (y>x/2) y=x-y;
  if (y==1) return x;
  long long int res=(x-y)+1;
  int i;
  for (i=(x-y+2);i<=x;i++)
    res*=i;
  for (i=2;i<=y;i++)
    res/=i;
  // cout<<"\nmenge "<<x<<" kombie "<<y<<" result= "<<res<<endl;
  return (int)res;
}
// Berechnet einen eindeutigen Index der Kombination ohne Wiederholung
// Vorbedinungen 
// L�ge des s Arrays = kombie
// Elemente in s sind sortiert und ohne Wiederholungne
// z.B s = {0,1,3}  Bedeutet 1. 2. und 4. Element der Menge
int kombie_index(int *s,int menge,int kombie) {
  int first=-1,index=0,dif;
  for (int x=0;x<kombie;x++) {
    dif=s[x]-first;
    if (dif>1) {
      index+=kombie_zahl(menge,kombie-x)-kombie_zahl(menge-dif+1,kombie-x);
    }
    first=s[x];
    menge-=dif;
  }
  return index;
}













