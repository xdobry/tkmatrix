//
// Polynom Object by using STL map (own lists cound be better)
//
// used for bilding of charakteristic polynom
// polmatrix<T> is an extra simple matrix object expecially for polynom
// and rekersive determinant computing
// I could not use matrix<T>, polynom<T> have not enoght operator.
// Using of polynom<T> as matrix<polynom<T> > and one matrix class is not posible
// due to rekursive relation (matrix<polynom<polynom<T> > >) by compiling
//
// Autor: Artur Trzewik
// Date: 03.1999
//
#if !defined(polynom_class)
#define polynom_class

#include <map>
#include <iostream>

using namespace std;

struct lint
{
  bool operator()(const int s1, const int s2) const
  {
    return (s1 < s2);
  }
};

int sign(double a);

template <class T>
class polynom {
private:
  map<int,T,lint> poly;
public:
  polynom() {}
  polynom (const polynom<T> &);
  polynom (const T &);
  polynom<T> & operator = (const polynom<T> &);
  ~polynom() {}
  // Setzen von Einzelnen Werten
  void set(int key,T value);
  // Funktionswert
  T value(T &);
  // Eleminiere alle Nulleintr�e nach Mulltiplikation m�lich
  void clear_null();
  polynom<T> operator + (const polynom<T> &) const;
  polynom<T> operator - (const polynom<T> &) const;
  polynom<T> operator * (const polynom<T> &) const;
  // Multiplikation mit dem Skalar
  polynom<T> operator * (const T &) const;

  void operator += (const polynom<T> &);
  void operator -= (const polynom<T> &);
  void operator *= (const polynom<T> &); 

  // Liefert 0 bei leeren Polynom oder = 0
  int sign();

  void Store(istream &); 
  void Restore(ostream &);
  // extract erluabt Auslesen der einzelnen Elementen des Polynoms
  // speicher Platz mu�freigegeben werden !!!!
  // you must free memory yourself
  void extract(int &len,T *&values,int *&keys);
};

/*
template class polynom<double>;
template class polynom<widefloat>;
template class polynom<rational>;
*/

template <class T>
polynom<T>::polynom (const polynom<T> &cp):poly(cp.poly) {
}

template <class T>
polynom<T>::polynom (const T &value) {
  poly[0]=value;
}

template <class T>
polynom<T> & polynom<T>::operator = (const polynom<T> &cp) {
  if (this!=&cp) {
    poly=cp.poly;
  }
  return *this;
}

template <class T>
void polynom<T>::set(int key,T value) {
  if (::sign(value)!=0) 
    poly[key]=value;
  else
    poly.erase(key);
}

template <class T>
void polynom<T>::Restore(ostream &os) {
  typename std::map<int,T,lint,std::allocator<std::pair<const int,T> > >::iterator  i = poly.begin();
  while (i!=poly.end()) {
    os << "index:" <<(*i).first << " value:" << (*i).second << endl;
    ++i;
  }
}

template <class T>
void polynom<T>::Store(istream &is) {
  T temp;
  int i;
  while (1) {
    if (!(is>>i)) return;
    if (i==-1) break;
    if (!(is>>temp)) return;
    poly[i]=temp;
  }
}

template <class T>
polynom<T> polynom<T>::operator + (const polynom<T> &poly2) const {
  // fprintf(stdout,"addition operator 1\n");
  // Restore(cout);
  // fprintf(stdout,"+ operator2\n");
  // poly2.Restore(cout);
  // fprintf(stdout,"end of 2nd\n");
  polynom<T> r;
  T temp;
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i1=poly.begin();
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i2=poly2.poly.begin();
  while (i1!=poly.end() || i2!=poly2.poly.end()) {
    if (i1!=poly.end() && (i2==poly2.poly.end() || (*i1).first<(*i2).first)) {
       r.poly.insert((*i1));
       ++i1;
    } else if (i2!=poly2.poly.end() && (i1==poly.end() || (*i1).first>(*i2).first)) {
       r.poly.insert((*i2));
       ++i2;
    } else {
      temp=(*i1).second+(*i2).second;
      if (::sign(temp)!=0) 
        r.poly[(*i2).first]=temp;
      ++i1; ++i2;
    }
  }
  // fprintf(stdout,"=\n");
  // r.Restore(cout);
  return r;
}
template <class T>
polynom<T> polynom<T>::operator - (const polynom<T> &poly2) const {
  // fprintf(stdout,"substraktion operator 1\n");
  // Restore(cout);
  // fprintf(stdout,"- operator2\n");
  // poly2.Restore(cout);
  polynom<T> r;
  T temp;
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i1=poly.begin();
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i2=poly2.poly.begin();
  while (i1!=poly.end() || i2!=poly2.poly.end()) {
    if (i1!=poly.end() && (i2==poly2.poly.end() || (*i1).first<(*i2).first)) {
       r.poly.insert((*i1));
       ++i1;
    } else if (i2!=poly2.poly.end() && (i1==poly.end() || (*i1).first>(*i2).first)) {
      r.poly[(*i2).first]=-(*i2).second;
      ++i2;
    } else {
      temp=(*i1).second-(*i2).second;
      if (::sign(temp)!=0) 
        r.poly[(*i2).first]=temp;
      ++i1; ++i2;
    }
  }
  // fprintf(stdout,"=\n");
  // r.Restore(cout);
  return r;
}
// Vorsicht hier keine Eleminierung von Nullen
// wie beim (x-2)(x+2)=x^2-4
template <class T>
polynom<T> polynom<T>::operator * (const polynom<T> &poly2) const {
  // fprintf(stdout,"multiplikation operator 1\n");
  // Restore(cout);
  // fprintf(stdout,"* operator2\n");
  // poly2.Restore(cout);
  polynom<T> r;
  int key;
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i1=poly.begin();
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i2=poly2.poly.begin();
  while (i1!=poly.end()) {
    i2=poly2.poly.begin();
    while (i2!=poly2.poly.end()) {
      key=(*i1).first+(*i2).first;
      r.poly[key]=r.poly[key]+(*i1).second*(*i2).second;
      //      if (sign(r.poly[key])==0) {
      //   
      //}
      ++i2;
    }
    ++i1;
  }
  // fprintf(stdout,"=\n");
  // r.Restore(cout);
  return r;
}
template <class T>
polynom<T> polynom<T>::operator * (const T &s) const {
  polynom<T> r;
  //  if (::sign(s)!=0) {
    typename map<int,T,lint>::const_iterator i=poly.begin();
    while (i!=poly.end()) {
      r.poly[(*i).first]=(*i).second*(T)s;
      ++i;
    }
    // } 
  return r;
}
template <class T>
void polynom<T>::clear_null() {
  typename map<int,T,lint>::iterator i=poly.begin();
  while (i!=poly.end()) {
    if (::sign(((*i).second))==0)
      poly.erase(i);
    ++i;
  }
}
template <class T>
void polynom<T>::operator += (const polynom<T> &poly2) {
  T temp;
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::iterator i1=poly.begin();
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i2=poly2.poly.begin();
  while (i1!=poly.end() || i2!=poly2.poly.end()) {
    if (i1!=poly.end() && (i2==poly2.poly.end() || (*i1).first<(*i2).first)) {
       ++i1;
    } else if (i2!=poly2.poly.end() && (i1==poly.end() || (*i1).first>(*i2).first)) {
      poly.insert(i1,(*i2));
      ++i2;
    } else {
      temp=(*i1).second+(*i2).second;
      if (::sign(temp)!=0) 
        poly[(*i2).first]=temp;
      else 
	poly.erase((*i2).first);
      ++i1; ++i2;
    }
  }
  // ++fprintf(stdout,"=\n");
  // Restore(cout);

}
template <class T>
void polynom<T>::operator -= (const polynom<T> &poly2) {
  T temp;
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::iterator i1=poly.begin();
  typename map<int,T,lint,std::allocator<std::pair<const int,T> > >::const_iterator i2=poly2.poly.begin();
  while (i1!=poly.end() || i2!=poly2.poly.end()) {
    if (i1!=poly.end() && (i2==poly2.poly.end() || (*i1).first<(*i2).first)) {
       ++i1;
    } else if (i2!=poly2.poly.end() && (i1==poly.end() || (*i1).first>(*i2).first)) {
      poly[(*i2).first]=-(*i2).second;
      ++i2;
    } else {
      temp=(*i1).second-(*i2).second;
      if (::sign(temp)!=0) 
        poly[(*i2).first]=temp;
      else 
	poly.erase((*i2).first);
      ++i1; ++i2;
    }
  }
  // ++fprintf(stdout,"=\n");
  // Restore(cout);
}
// Hier ist keine gro�rtige Optimierung wie beim += oder -= m�lich
template <class T>
void polynom<T>::operator *= (const polynom<T> &poly2) {
  map<int,T,lint> tpoly;
  int key;
  typename map<int,T,lint>::const_iterator i1=poly.begin();
  typename map<int,T,lint>::const_iterator i2=poly2.poly.begin();
  while (i1!=poly.end()) {
    i2=poly2.poly.begin();
    while (i2!=poly2.poly.end()) {
      key=(*i1).first+(*i2).first;
      tpoly[key]=tpoly[key]+(*i1).second*(*i2).second;
      //      if (sign(r.poly[key])==0) {
      //   
      //}
      ++i2;
    }
    ++i1;
  }
  poly=tpoly;
}

// Optimierung m�lich siehe Algorithmenbuch
template <class T>
T polynom<T>::value(T &val) {
  T ret;
  T potenz=val;
  int c=1;
  typename std::map<int,T,lint,std::allocator<std::pair<const int,T> > >::iterator i1=poly.begin();
  while (i1!=poly.end()) {
    while ((*i1).first>c) {
      potenz=potenz*val;
      c++;
    }
    if ((*i1).first==0) {
      ret=(*i1).second;
    } else 
      ret=((*i1).second*potenz)+ret;
    ++i1;
  }
  return ret;
}
template <class T>
int polynom<T>::sign() {
  // fprintf(stdout,"operator sign\n");
  if (poly.empty()) return 0;
  if (poly.size()==1 && poly[0]==T(0)) return 0;
  return 1;
}
template <class T>
int sign(polynom<T> &r) { return r.sign(); }
template <class T>
void polynom<T>::extract(int &len,T *&values,int *&keys) {
  len=poly.size();
  if (len==0) return;
  values=new T[len];
  keys=new int[len];
  typename map<int,T,lint>::iterator i1=poly.begin();
  int t=len-1;
  while (i1!=poly.end()) {
    values[t]=(*i1).second;
    keys[t]=(*i1).first;
    t--;
    ++i1;
  }
}



/*
Diese Prozedur war sehr uneffizient und wurde durch optimierte Implementierung
in der klasse matrix<T> ersetzt 

// Matrix nur fr Polynomen
// Berechnet den charakteristischen Polynom
template <class T>
class polmatrix {
  polynom<T> *adrmatrix;
  int hoehe,breite;
 private:
  polmatrix(const polmatrix<T> &);
  polmatrix<T> & operator = (const polmatrix<T> &);
 public:
  polmatrix(const matrix<T> &);
  polmatrix(const polmatrix<T> &,int,int);
  ~polmatrix();
  polynom<T> rekdeterminante();
};
template <class T>
polmatrix<T>::polmatrix(const matrix<T> &m) {
  hoehe=m.mhoehe();
  breite=m.nbreite();
  if (hoehe*breite==0) {
    adrmatrix=0; return;
  }
  if (hoehe!=breite) {
    adrmatrix=0; return;
  }
  adrmatrix = new polynom<T>[hoehe*breite];

  int x,y;
  for (y=0;y<hoehe;y++)
    for (x=0;x<breite;x++) {
      adrmatrix[y*breite+x].set(0,m(y,x));
    }
  for (x=0;x<breite;x++) {
    adrmatrix[x*breite+x].set(1,T(-1));
  }
}
template <class T>
polmatrix<T>::polmatrix(const polmatrix<T> &m,int y,int x) {
  // enspricht der Streichungs Matrix wenn y (Zeile) x (Spalte) au�r 
  // Bereich dann die gr�ten m�lichen Werte oder leere Matrix
  // fprintf(stdout,"streichungs polmatrix bilding %d %d\n",y,x);
  if (m.hoehe<=1 || m.breite<=1) {
    hoehe=0; breite=0; adrmatrix=0;
  } else {
    hoehe=m.hoehe-1;
    breite=m.breite-1;
    adrmatrix = new polynom<T>[hoehe*breite];
    if (y>hoehe-1) y==hoehe-1;
    if (x>breite-1) x==breite-1;
    polynom<T> *copyadr=adrmatrix;
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
polmatrix<T>::~polmatrix() {
  if (adrmatrix) delete[] adrmatrix;
}
template <class T>
polynom<T> polmatrix<T>::rekdeterminante() {
  if (hoehe!=breite || hoehe==0 || breite==0) {
    return polynom<T>();
  }
  if (hoehe==1) return adrmatrix[0];
  polynom<T> ret;
  if (hoehe==2) {
    // Vereinfachte Formel a00*a11-a01*a01
    ret=adrmatrix[0]*adrmatrix[3]-adrmatrix[2]*adrmatrix[1];
  } else {
    for (int y=0;y<hoehe;y++) {
      // fprintf(stdout,"rekdeterminante step %d breite %d\n",y,hoehe);
      if (sign(adrmatrix[y*breite])!=0) {
        polmatrix<T> tmatrix(*this,y,0);
        if (y%2==0) {
          ret=ret+(adrmatrix[y*breite]*tmatrix.rekdeterminante());
        } else {
	ret=ret-(adrmatrix[y*breite]*tmatrix.rekdeterminante());
        }
      }
    }
  }
  // ret.Restore(cout);
  // fprintf(stdout,"end of rek\n");
  return ret;
}
// Diese Prozedur kann nicht als Member von matrix<T> eingesetzt werden
// weil sonst auch chpolynom von polynomen bercksichtig werden msste
// was zu Endlosschleifen fhrt
// Diese Prozedur generiert den charakteristischen Polynom nach Schullmethode
// Zuerst wird die Matix in Polynom Matrix umgewandelt. Die Determinante davon
// ist der charakteristische Polynom. Der wiederum in eine Matrix umgewandelt
// wird
template <class T>
void chpolynom(matrix<T> &m) {
  if (m.nbreite()!=m.mhoehe()) {
    m=matrix<T>(0,0,m.prot);
    m.prot.putError("must_be_square");
    return;
  }
  m.prot<<m;
  m.prot<<"charakteristisches Polynom\n";
  polmatrix<T> pmatrix(m);
  polynom<T> chpol=pmatrix.rekdeterminante();
  T *values;
  int *keys,len;
  // fprintf(stdout,"step 1\n");
  chpol.extract(len,values,keys);
  // fprintf(stdout,"step 2\n");
  if (len==0) { m=matrix<T>(0,0,m.prot); return; }
  matrix<T> ret(2,len,m.prot);
  // fprintf(stdout,"step 3 len %d\n",len);
  for (int x=0;x<len;x++) {
    ret(0,x)=T(keys[x]);
    ret(1,x)=values[x];
  }
  // fprintf(stdout,"step 4\n");
  delete[] values;
  delete[] keys;
  // fprintf(stdout,"step 5\n");
  m.prot.add(ret,MatrixObject::tpolynom);
  m=ret;
}
*/
#endif





