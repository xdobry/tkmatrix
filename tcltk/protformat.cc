#include "protformat.h"
#include <stdio.h>
// Unix abhängig
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char *protformat::objects[]={ "matrix","string","main","eqnarray","solution",
			      "determinante","simplex","polynom","scalar"};
char *protformat::smatrix[]= { "head","headflex","headtail","rowhead","bruch1",
			       "bruch2","bruch3","double","cellseparator",
			       "rowtail","tail"};
char *protformat::sstring[]= { "head","tail" };
char *protformat::smain[]= { "head","tail" };
char *protformat::seqnarray[]= { "head","rowhead","bruch1","bruch2","bruch3",
				 "double","cellseparator","equal","rbruch1",
				 "rbruch2","rbruch3","rdouble","rowtail","tail" };
char *protformat::ssolution[]= { "head","arrayhead","bruch1","bruch2","bruch3",
				 "double","separator","index","arraytail","tailmore",
				 "tail" };
char *protformat::sdeterminante[]= { "head","bruch1","bruch2","bruch3","double",
				     "tail" };
char *protformat::ssimplex[]= { "head","headflex","headtail","rowhead","bruch1",
				"bruch2","bruch3","double","cellseparator",
				"rowtail","fhead","fbruch1","fbruch2","fbruch3",
				"fdouble","tail" };
char *protformat::spolynom[]= { "head","bruch1","bruch2","bruch3",
				 "double","obruch1","obruch2","obruch3","odouble",
                                "tail" };
char *protformat::sscalar[]= { "bruch1","bruch2","bruch3","double" };

int protformat::feldcount[objectcount]={ sizeof(smatrix)/sizeof(smatrix[0])
					 ,sizeof(sstring)/sizeof(sstring[0])
					 ,sizeof(smain)/sizeof(smain[0])
					 ,sizeof(seqnarray)/sizeof(seqnarray[0])
					 ,sizeof(ssolution)/sizeof(ssolution[0])
				,sizeof(sdeterminante)/sizeof(sdeterminante[0])
					 ,sizeof(ssimplex)/sizeof(ssimplex[0]) 
                                         ,sizeof(spolynom)/sizeof(spolynom[0])
                                         ,sizeof(sscalar)/sizeof(sscalar[0])
};

bool protformat::nurbruch=true,protformat::widefloat_reformat=true;
int protformat::widefloat_showprec=0,protformat::widefloat_maxnum=6,protformat::widefloat_maxzeros=6;

protformat::protformat(const char *dirpath,char *argv0,bool vnurbruch,const char *form) {
  string dpath;
  nurbruch=vnurbruch;
  if (dirpath) dpath.assign(dirpath);
  string fform(form);
  for (int i=0;i<fform.length();i++) {
    fform.at(i)=tolower(fform.at(i));
  }
  string fname;
  char buf[10];
  ifstream formfile;
  if (!dpath.empty() && dpath.at(dpath.length()-1)!='/') dpath+='/';
  int x;
  char **feldtab[objectcount];
  feldtab[fmatrix]=smatrix; feldtab[fstring]=sstring;  
  feldtab[fmain]=smain;     feldtab[feqnarray]=seqnarray;
  feldtab[fsolution]=ssolution;   feldtab[fdeterminante]=sdeterminante;
  feldtab[fsimplex]=ssimplex;    feldtab[fpolynom]=spolynom;  
  feldtab[fscalar]=sscalar;  
  int bytesread;
  for(x=0;x<objectcount;x++) sgraph[x]=0;
  for(x=0;x<objectcount;x++) {
    fname=string(objects[x])+string(".")+fform+string(".mform");
    // fname=dpath+string(objects[x])+string(".")+fform+string(".mform");
    suchedatei(formfile,dpath,fname,argv0);
    // formfile.open(fname,ios::in);
    if (formfile) {
      sgraph[x]=new string*[feldcount[x]];
      string sline;
      int linenumber=0;
      getline(formfile,sline);
      linenumber++;
      bytesread=sline.length();
      // bytesread=readline(formfile,sline); linenumber++;
      //cerr<<linenumber<<" A:"<<fname<<":"<<sline<<endl;
      for (int i=0;i<feldcount[x];i++) {
	if (bytesread && sline.at(0)=='%') {
	  if (sline.find(feldtab[x][i],1)==1) {
	    if (sline.length()>strlen(feldtab[x][i])+1)
	      sgraph[x][i]=new string(sline.substr(strlen(feldtab[x][i])+2));
	    else
	      sgraph[x][i]=new string(); 
	    // aless nach %feldname außer \n einfügen
	    getline(formfile,sline);
	    linenumber++;
	    bytesread=sline.length();
	    // bytesread=readline(formfile,sline); linenumber++; 
	    // gab es eine leere Zeile (nur \n) wird trotzdem byteread=0 
	    //cerr<<linenumber<<" B:"<<fname<<":"<<sline<<endl;
	    while (bytesread && sline.at(0)!='%') {
	      sgraph[x][i]->operator+=(sline.substr(1));
	      if (sline.at(sline.length()-1)!='\\')
		sgraph[x][i]->operator+=('\n');
	      else 
		sgraph[x][i]->erase(sgraph[x][i]->length()-1,1);
	      getline(formfile,sline);
	      linenumber++;
	      bytesread=sline.length();
	      // bytesread=readline(formfile,sline); linenumber++;
	      //cerr<<linenumber<<" C:"<<fname<<":"<<sline<<endl;
	    }
	  } else {
	    sprintf(buf,"%i",linenumber);
	    throw badformat(string("Erwarte Feldname: '%")+
			    string(feldtab[x][i])+string("' in Zeile ")+
			    string(buf)+string(" von ")+fname+string(" : ")+sline);
	  }
	} else if (x==fstring  && bytesread && sline.at(0)=='>') {
	  do {
	    struct translate t;
	    t.from=sline[1];  // Vorsicht! keine zusätzliche whitespaces  
	    t.to=sline.substr(3); // nur '>ä &auml'
	    translist.push_back(t);
	    getline(formfile,sline);
	    linenumber++;
	    bytesread=sline.length();
	    // bytesread=readline(formfile,sline); linenumber++;
	    //cerr<<linenumber<<" C:"<<fname<<":"<<sline<<endl;
	  } while (bytesread && sline.at(0)=='>');
          i--;
	} else {
	  sprintf(buf,"%i",linenumber);
	  throw badformat(string("Erwarte Feldname '%")+
			  string(feldtab[x][i])+string("' in Zeile ")+
			  string(buf)+string(" von ")+fname+string(" : ")+sline);
	}
      } 
    } else {
      if (x<formmustbe)
	throw badformat(string("Kann die Datei ")+fname+
			string(" nicht finden. Versuchen Sie mit -v Pfhad"));
    }
    formfile.close();
  }
  //cerr<<"end of format"<<endl;
}
protformat::~protformat() {
  for (int x=0;x<objectcount;x++) {
    if (sgraph[x]) delete[] sgraph[x];
  }
}
void protformat::openmain(ostream &wy) {
  if (sgraph[fmain]) wy<<*sgraph[fmain][0];
}
void protformat::closemain(ostream &wy) {
  if (sgraph[fmain]) wy<<*sgraph[fmain][1];
}
const char *protformat::givefstring(tformobject object,int feld) {
  return sgraph[object][feld]->c_str();
}
bool protformat::form_exist(tformobject object) {
  return (sgraph[object]!=0);
}
template <class InputIterator>
string alg_translate(InputIterator first, InputIterator last, char c) {
    while (first != last && (*first).from!=c) {
      first++;
    }
    if (first==last)
      return string(1,c);
    else
      return (*first).to;
}
string protformat::translate(const string &s) {
  string ret;
  for (int x=0;x<s.length();x++)
    ret+=alg_translate(translist.begin(),translist.end(),s[x]);
  return ret;  
}
badformat::badformat(string s) {
  strncpy(errorstring,s.c_str(),200);
  errorstring[199]=(char)0;
}
char *badformat::givestring() {
  return errorstring;
}
// Zuerst wird die Datei name in aktuellen Verzeichniss gesucht nacher in
// dem Verzeichniss wo das Programm liegt nacher in /usr/lib/matrix
// und schließlich in dem Verzeichniss worauf Sybolischerlink (wenn solcher ist)
// zeigt ich weiß nicht wie man Envirorment Variablen liest dann
// könnte man auch MATRIXPATH verwenden oder so etwa
void protformat::suchedatei(ifstream &in,string dpfad,string &name,char *argv0) {
  if (!dpfad.empty()) {
    name=dpfad+name;
    in.open(name.c_str(),ios::in);
  } else {
    in.open(name.c_str(),ios::in);
    if (in) return;
    string tname;
    string progloc(argv0);
    if (progloc.find('/')!=string::npos) {
      int x;
      for (x=progloc.length()-1;x>=0 && progloc[x]!='/';x--);
	if(x>=0) {
	  progloc.erase(0,x-1);
	  tname=progloc+name;
	  in.open(tname.c_str(),ios::in);
	  if (in) return;
	}
    } else progloc=string("");
    tname=string("/usr/lib/tkmatrix/")+name;
    in.open(tname.c_str(),ios::in);
    if (in) return;
    struct stat buf;
    if (lstat(argv0,&buf)==0) {
      if (S_ISLNK(buf.st_mode)) {
	char lbuf[50];
	int len=readlink(argv0,lbuf,49);
	if (len!=-1) {
	  for (;len>0 && lbuf[len]!='/';len--);
	  lbuf[len]=0;
	  tname=progloc+string(lbuf)+string("/")+name;
	  in.open(tname.c_str(),ios::in);
	  if (in) return;
	}
      }
    }
  }   
}












