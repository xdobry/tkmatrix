#include "matrix3.h"
#include "stdio.h"
#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <cstring>

void myform(ostream &outs,const char* format,...) {
  va_list args;       // argument list
  va_start(args,format);
  const int bufsize = 5000;
  char buf[bufsize];
  int size = vsnprintf(buf,bufsize-1,format,args);
  outs<<buf;
  //cout<<":"<<size<<":"<<format<<":"<<buf<<":"<<endl;
  va_end(args); 
}

// Implementierung von protstring
protstring::protstring(const protstring& ps) {
  pstring=ps.pstring;
}
void protstring::PrintOn(ostream &wy,protformat &pformat)const {
  wy<<pformat.givefstring(protformat::fstring,0)<<pformat.translate(pstring)<<'\n'<<
    pformat.givefstring(protformat::fstring,1);
}
// Funktionne die von trmplate matrixkx gebraucht werden
bool printtype(const rational &r,ostream &wy,
	       protformat::tformobject fobject,int firststring,
	       protformat &pformat,int index, bool nozero,
	       bool alwayssign,bool ignoreone
	       ) {
  string z,l,m;    // z=GanzeZahlen l=Z�ler m=Nenner
  int bruchtyp=r.loadstring(z,l,m,!pformat.nurbruch);
  if (nozero) {
    if (bruchtyp==3) return false;
    if (!ignoreone) {
      if (r==rational(1)) {
	z="";
      } else if (r==rational(-1)) {
	z="-";
      }
    }
    if (alwayssign) {
      switch(bruchtyp) {
      case 0:
      case 2:
	if (r.sign()>0) 
	  z='+'+z;
	break;
      case 1: 
	z='+';
	bruchtyp=2;
	break;
      }
    }
  }
//   fprintf(stderr,"z %s\n",z.c_str());
//   fprintf(stderr,"l %s\n",l.c_str());
//   fprintf(stderr,"m %s\n",m.c_str());
  switch(bruchtyp) {
  case 1:
    // TODO !!
    myform(wy,pformat.givefstring(fobject,firststring+1),l.c_str(),m.c_str(),index);
    break;
  case 2:
    // TODO !!
    myform(wy,pformat.givefstring(fobject,firststring+2),z.c_str(),l.c_str(),m.c_str(),index);
    break;
  case 0:
  case 3:
   // TODO !! 
   myform(wy,pformat.givefstring(fobject,firststring),z.c_str(),index);
   break;
  }
  return true;
}
#if !defined(NO_GMP)
bool printtype(const widefloat &wf,ostream &wy,
	       protformat::tformobject fobject,int firststring,
	       protformat &pformat,int index,bool nozero,
	       bool alwayssign,bool ignoreone
	       ) {
  bool ret=true;
  if (!(wf.sign()==0.0 &&  nozero)) {
    string *s=wf.tostring(pformat.widefloat_reformat,pformat.widefloat_showprec,
			  pformat.widefloat_maxnum,pformat.widefloat_maxzeros);
    myform(wy,pformat.givefstring(fobject,firststring),s->c_str(),index);
    delete s;
  }
  else
    ret=false;
  return ret;
}
int makeindex(const widefloat &r) {
  return (int)r.todouble();
}
protstream& protstream::operator << (matrix<widefloat> &m) {
  if (!noprot) {
    mylist.push_back(new ProtTemplate<matrixkx<widefloat> >(matrixkx<widefloat>(m,MatrixObject::tmatrix)));
  }
  return *this;
}
protstream& protstream::operator << (const widefloat &wf) {
  if (!noprot) {
    mylist.push_back(new ProtTemplate<protscalar<widefloat> >(protscalar<widefloat>(wf)));
  }
  return *this;
}
void protstream::add(matrix<widefloat>& m,MatrixObject::KontextType ktx) {
    if (!noprot) {
      mylist.push_back(new ProtTemplate<matrixkx<widefloat> >(matrixkx<widefloat>(m,ktx)));
    }
}
#endif
bool printtype(double d,ostream &wy,
	       protformat::tformobject fobject,int firststring,
	       protformat &pformat,int index,bool nozero,
	       bool alwayssign,bool ignoreone
	       ) {
  bool ret=true;
  if (!(d==0.0 &&  nozero))
    myform(wy,pformat.givefstring(fobject,firststring+3),d,index);
  else
    ret=false;
  return ret;
}
int makeindex(const rational &r) {
  return (int)r.todouble();
}
int makeindex(double d) {
  return int(d);
}


// Implementierung von protstream
protstream::protstream(void):errorInfo(0),noprot(false) {
}
void protstream::leereprot(void) {
  int lcount=mylist.size();
  ProtBase *temp;
  // Ich wie�nicht wie man es eleganter machen k�nte
  for (int x=0;x<lcount;x++) {
    temp=*mylist.begin();
    mylist.erase(mylist.begin());
    // ohne delete entseht ein dangling pointer
    delete temp;
  }
  if (errorInfo) {
    delete errorInfo;
    errorInfo=0;
  }
}
bool protstream::empty() {
  return mylist.empty();
}
protstream::~protstream() { 
  leereprot(); 
}
protstream& protstream::operator << (const char *s)
{
  // cerr<<"prot string added \""<<s<<"\"\n";
  if (!noprot) {
    int len = strlen(s);
    if (len!=0) {
      if (s[len-1]=='\n') {
        // schreibe ohne abschliessenden /n
        zeile.write(s,len-1);
        string szeile(zeile.str());
        mylist.push_back(new ProtTemplate<protstring>(szeile));
        zeile.clear();               // Zeile wieder entfrieren und zum schreiben vorbereiten
        // zeile.rdbuf()->freeze(0);    // Verfahren siehe Seite 517 Objectorientiertes Programieren in C++
        zeile.str("");
      } else {
          zeile<<s;
      }
    }
  }
  return *this;
}
protstream& protstream::operator << (int x) {
  // cerr<<"prot string int added "<<x<<"\n";
  if (!noprot) {
    zeile<<x;
  }
  return *this;
}
protstream& protstream::operator << (matrix<rational> &m) {
  if (!noprot) {
    mylist.push_back(new ProtTemplate<matrixkx<rational> >(matrixkx<rational>(m,MatrixObject::tmatrix)));
  }
  return *this;
}
protstream& protstream::operator << (matrix<double> &m) {
  if (!noprot) {
    mylist.push_back(new ProtTemplate<matrixkx<double> >(matrixkx<double>(m,MatrixObject::tmatrix)));
  }
  return *this;
}
protstream& protstream::operator << (const double &m) {
  if (!noprot) {
    mylist.push_back(new ProtTemplate<protscalar<double> >(protscalar<double>(m)));
  }
  return *this;
}
protstream& protstream::operator << (const rational &m) {
  // cerr<<"prot scalar added\n";
  if (!noprot) {
    mylist.push_back(new ProtTemplate<protscalar<rational> >(protscalar<rational>(m)));
  }
  return *this;
}
void protstream::add(matrix<double>& m,MatrixObject::KontextType ktx) {
  if (!noprot) {
    mylist.push_back(new ProtTemplate<matrixkx<double> >(matrixkx<double>(m,ktx)));
  }
}
void protstream::add(matrix<rational>& m,MatrixObject::KontextType ktx) {
    if (!noprot) {
      mylist.push_back(new ProtTemplate<matrixkx<rational> >(matrixkx<rational>(m,ktx)));
    }
}
typedef ProtBase* protbasepnt;
class feach
{
private:
  ostream &wy;
  protformat &pformat;
public:
  feach(ostream &,protformat &pformat);
  void operator()(const protbasepnt& x);
};
inline feach::feach(ostream &wy,protformat &pformat): wy(wy),pformat(pformat) { }
void  feach::operator()(const protbasepnt& x) { 
  //  fprintf(stderr,"printing stream elem\n");
  x->PrintOn(wy,pformat);
}

void protstream::PrintOn(ostream &wy,protformat &pformat)const {
  //  fprintf(stderr,"printing stream elems\n");
  feach f(wy,pformat);
  for_each(mylist.begin(),mylist.end(),f);
}
void protstream::putError(char *c) {
  if (errorInfo) {
    delete errorInfo;
    errorInfo=0;
  }
  if (c) {
    errorInfo=new char[strlen(c)+1];
    strcpy(errorInfo,c);
  }
}
char *protstream::getError() {
  return errorInfo;
}














