#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <ctype.h>  // for isdigit(...)

#include "matrix3.h"
#include "protstream.h"
// #include <std/list> bereits in protstream.h
#include <algorithm>
#include <cstring>

using namespace std;

/*
extern void matrixkx<double>::PrintOn(ostream &,protformat &) const;
extern void matrixkx<rational>::PrintOn(ostream &,protformat &) const;
*/

char help_text[]=
"matrix -a Algorithmus [pzeile pspalte] Matrixdatei(en)|[-s] [-o Outputdatei] [-t Outputtyp]\n"
"   [-v Vorlagenpfad] [-s] [-p] [-k] [-b] [-e]\n"
"Algorithmus={none,gauss,jordan,loesung,kern,bild,inverse,determinante,\n"
"             pivotiere,naehrung,eckenfindung,simplex,chpolynom,matrixspiel}\n"
"oder ein nummer z.B. 1 fr gauss\n"
"Lange Optionsnammen\n"
"-a, --algorithmus: muß definiert werden\n"
"-o, --output: wie matrix >Outputdatei\n"
"-t, --typeoutput : Type des Ausgabe z.B. {ascii,html,latex}\n"
"-v, --vorlagen: Pfad mit Vorlagen Darteien (*.mform) sonst gesucht da wo das Programm\n"
"-s, --stdin: Matrix wird aus Standarteingabe gelesen\n"
"-k, --konvert: Brüche werden zu Gleitkommazahlen umgewandelt\n"
"-b, --bruch: Brüche werden ohne den Ganzzahl geschrieben\n"
"-e, --ergebnis: Ergebnis als .mat Datei speichern\n"
"-p, --protokoll: Protokoll Alle Zwischenschritte ausgeben" 
"-h, --help: Tja\n"
"pzeile psalte - sind die Stellen an dem die Pivotierung vorgenommen werden soll\n"
"ohne Optionnen Interaktiver Modus; nicht zum Empfehlen\n\n"
"Geschrieben von Artur Trzewik (mail@xdobry.de)\n";
char komunikat[]="Matrix geschrieben von Artur Trzewik 07.1996 v 0.4\n"
" 1 Matrix Laden              11 Bild von Matrix\n"
" 2 Matrix Sichern            12 Inverse von Matrix\n"
" 3 Matrix Ausgeben           13 Determinante von Matrix\n"
" 4 Ergebnis Sichern          14 Nährungslösung\n"
" 5 Ergebnis Ausgeben         15 Pivotiere\n"
" 6 Protokoll Ausgeben        16 Eckenfindung\n"
" 7 Gauß algorithmus          17 Optiemiere\n"
" 8 Gauß-Jordan-algorithmus   18 charakteristisches Polynom\n"
" 9 Lösung Ausrechnen         19 Matrixspiel\n"
" 10 Kern von Matrix          20 Matrix = Ergebnis\n"
"                             21 Programm beenden\n"
" (jede andere Taste dises Text)\n";
char welcherpro[]=" Die Protokollmenu\n"
"  1 Ausgaben auf den Bildschrimm\n"
"  2 Ausgaben in eine Datei\n"
"? PM ";
#define opt_count 10
#define opt_par_count 5
bool interactiv;
char kurz_opt[opt_count+1]="otavespkhb";
char *lang_opt[opt_count]={ "output","typeoutput","algorithmus","vorlagen","ergebnis",
			    "stdin","protokoll","konvert","help","bruch" };
enum eoptparam { eoutput,etypeoutput,ealgorithmus,evorlagen,eergebniss };
enum eoptswitch { estdin,eprotokoll,ekonvert,ehelp,ebruch};
char *opt_param[opt_par_count]={ 0,0,0,0,0 };
bool opt_switch[opt_count-opt_par_count]={ false,false,false,false,false };
list<const char *> inputlist;
#define alg_count 14
char *algorithmen[alg_count]={"none","gauss","jordan","loesung","kern","bild",
			      "inverse","determinante","pivotiere","naehrung",
			      "eckenfindung","simplex","chpolynom","matrixspiel"};
enum talgorithmus {none,gauss,jordan,loesung,kern,bild,inverse,determinante,
                   pivotiere,naehrung,eckenfindung,simplex,ch_polynom,matrixspiel};
int pzeile,pspalte;
talgorithmus algorithmus_id;
protformat *pformat;
int full(MatrixObject *m) {
  if (!m) { cout<<"Zuerst Matrix Laden\n";
  return false;
  } else return true;
}
int schonende(int a) {
  char b;
  if (a==21) {
    cout<<"Programm beenden, bist du sicher ? (j/n) ";
    cin>>b;
    if (b=='j' || b=='J') return 1;
  }
  return 0;
}
#define DLMAX 79
MatrixObject* readMatrix (istream &we) {
  MatrixObject *readMatrix;
  int m,n;
  char puffer[6]; // Eventuel f<FC>r Zeichenkette "FLOAT\0"
  if (!we || we.eof()) { return 0; }
  we.read(puffer,5);
  puffer[5]=(char)0;
  if(strcmp(puffer,"FLOAT")==0) { // Eine Gleitkommamatrix
    we>>m; we>>n;
    if (!we || we.eof()) {  return 0; }
    readMatrix=new matrix<double>(m,n,*new protstream());
    readMatrix->Restore(we);
  } else if (strcmp(puffer,"FLOAX")==0) {
    we>>m; we>>n;
    if (!we || we.eof()) {  return 0; }
    // readMatrix=new matrix<double>(m,n,*new protstream());
    // dynamic_cast<matrix<double> *>(readMatrix)->scanDigit(we);
  } else { // Eine Bruchzahlenmatrix
    we.seekg(0,ios::beg);
    we>>m; // matrix m x n m-hoehe n-breite
    we>>n;
    if (!we || we.eof()) {  return 0; }
    readMatrix=new matrix<rational>(m,n,*new protstream());
    readMatrix->Restore(we);
  }
  return readMatrix;
}
void writeMatrix(MatrixObject *m,ostream &wy) {
  if  (m->whoamI()==MatrixObject::tfloat) {
    //  wy<<"FLOAX";
    //  wy<<m.mhoehe()<<'\t'<<m.nbreite()<<'\n';
    //  static_cast<matrix<double> &>(m).printDigit(wy);
    wy<<"FLOAT\n"<<m->mhoehe()<<'\t'<<m->nbreite()<<'\n';
    m->Store(wy);
  } else {
    wy<<m->mhoehe()<<'\t'<<m->nbreite()<<'\n';
    m->Store(wy);
  }
}
MatrixObject* loadmatrix() {
  char name[DLMAX+1];
  int m,n,m1,n1;
  cout<<"Gibt die Name des Datei: ";
  cin>>setw(DLMAX)>>name;        // mein compiler nimmt das nicht an
  ifstream we(name,ios::in);
  if (we) {
    return readMatrix(we);
  }
  cout<<"Datei: "<<name<<" konnte nicht geöffnet werden"<<endl;
  return 0;
}
void savematrix(MatrixObject *m) {
  char name[DLMAX];
  if (m) {
    cout<<"Speichert unter: ";
    cin>>setw(DLMAX)>>name;        // mein compiler nimmt das nicht an
    ofstream wy (name,ios::out);
    if (wy) {
      writeMatrix(m,wy);
      wy.close();
    } else cout<<"Datei konnte nicht geöffnet werden\n";
  } else cout<<"Es gibt nichts zu speichern\n";
}
void printmatrix(MatrixObject *m) {
  if (m) {
    cout<<"Matrix "<<m->mhoehe()<<" x "<<m->nbreite()<<'\n';
         
    m->Store(cout);
  } else cout<<"Nichts auszugeben\n";
}
void openprotokol(MatrixObject *m) {
  if (full(m)) {
    if (!pformat) {
      string dir,outputtyp;
      cout<<"Geben Sie das Verzeichniss wo die Formatdateien liegen oder ein Punkt wenn in aktuellen Verzeichniss :";
      cin>>dir;
      cout<<"\nGeben Sie den Typ des Outputs ascii,html oder latex :";
      for (int i=0;i<outputtyp.length();i++) {
	outputtyp.at(i)=tolower(outputtyp.at(i));
      }
      cin>>outputtyp;
    try {
	pformat=new protformat(dir.c_str(),"",false,outputtyp.c_str());
    } 
    catch(badformat error) {
      cout<<"In angegebenen Phad: "<<error.givestring()<<endl;
      return;
    }
    }
    char name[DLMAX];
    ofstream wy;
    int com;
    cout<<welcherpro;
    cin>>setw(9)>>name;
    com=0;
    sscanf(name,"%i",&com);
    switch(com) {  //   1-Bildschirm 2-Datei 3-Kein
    case 1:
      pformat->openmain(cout);
      m->prot.PrintOn(cout,*pformat);
      pformat->closemain(cout);
      break;
    case 2:
      cout<<"Protokoll speichern unter: ";
      cin>>setw(DLMAX)>>name;
      wy.open(name,ios::out);
      if (wy) {
        pformat->openmain(wy);
	m->prot.PrintOn(wy,*pformat);
        pformat->closemain(wy);
	wy.close();
      } else cout<<"Datei "<<name<<" könnte nicht geöffnet werden\n";
      break;
    default:
      cout<<"Falsche Eingabe\n";
    }
  }
}
void VorbereiteAlgorithmus(MatrixObject *a,MatrixObject *&e) {
  if (e) delete e;
  e=a->clone(true);
  a->prot.leereprot();
}
void addProt(MatrixObject *m,MatrixObject::KontextType kt) {
  switch (m->whoamI()) {
  case MatrixObject::trational:
    // return new matrix<rational>(*(dynamic_cast<matrix<rational> *>(m)));
    {
    m->prot.add(*((matrix<rational> *)(m)),kt);
    }
    break;
  case MatrixObject::tfloat:
    {
    m->prot.add(*((matrix<double> *)(m)),kt);
    }
    break;
  }
}
void put_error(string s) {
  cerr<<s<<endl;
  exit(EXIT_FAILURE);
}
// 
// Ergebnissmatrix wird in e ZUrckgegeben
// Wenn kein Ergebniss dann wird e auf NULL gesetzt
//
void doalgorithmus(MatrixObject *a,MatrixObject *&e,talgorithmus ta) {
  if (full(a)) {
    VorbereiteAlgorithmus(a,e);
    try {
      switch (ta) {
      case gauss: e->gauss(); break;
      case jordan: e->gauss_jordan(); break;
      case loesung: 
	addProt(e,MatrixObject::tgleichung); 
	e->gauss_jordan(); e->loesung(); 
	addProt(e,MatrixObject::tloesung); 
	break;
      case kern: e->kern(); break;
      case bild: e->bild(); break;
      case inverse: e->inverse(); break;
      case determinante: e->determinante(); 
	addProt(e,MatrixObject::tdeterminante);  
	break;
      case  naehrung: addProt(e,MatrixObject::tgleichung); 
	e->naehrungsloesung(); 
	addProt(e,MatrixObject::tloesung); 
	break;
      case pivotiere:
	int x,y;
	if (interactiv) {
	  cout<<"Gibt die Zeilen Nr 0-"<<a->mhoehe()-1<<": ";
	  cin>>y;
	  cout<<"Gibt die Spalten Nr 0-"<<a->nbreite()-1<<":  ";
	  cin>>x;
	  if (y<a->mhoehe() && x<a->nbreite()) {
	    // *(e->prot)<<"Ausgagsmatrix zum Pivotieren\n"<<(*a);
	    e->prot.operator<<("Ausgagsmatrix zum Pivotieren\n");
	    addProt(e,MatrixObject::tmatrix);
	    //  e->prot.operator<<(*a);
	    e->pivotiere(y,x);
	  } else cout<<"Zahl außer Bereich\n";
	} else if (pzeile<a->mhoehe() && pspalte<a->nbreite()) {
	    e->prot.operator<<("Ausgagsmatrix zum Pivotieren\n");
	    addProt(e,MatrixObject::tmatrix);
	    e->pivotiere(pzeile,pspalte);
	} else put_error(string("Die Pivotierungstelle liegt nicht in der Matrix"));
	break;
      case eckenfindung: e->eckenfindung(); break;
      case simplex: 
	addProt(e,MatrixObject::tsimplex);
	e->optimiere(); 	  
	addProt(e,MatrixObject::tloesung);
	break;
      case ch_polynom:
	e->chpol();
	break;
      case matrixspiel:
        e->matrixspiel();
	break;
      }
    }
    catch (mylongint::overflow) {
      if (e) { 
	delete e;
	e=0;
      }
      cerr<<"Überlaufsfehler; Versuchen Sie die Ruhe zu bewahren! \nProtokoll beinhaltet alle Schritte bis zum Fehlereintritt\n";
    }
    if (e->nbreite()==0) {
      cerr<<"Algorithmus lieferte kein Ergebniss wahrscheinlich schlechte Input Matrix\n";
    }
  }
}
void parse_param (int argc,char *argv[]) {
  int i;
  for (int x=1;x<argc;x++) {
    if (argv[x][0]=='-') { // option -
      if (argv[x][1]=='-') { // lange option --
	for (i=0;i<opt_count;i++) {
	  if (strcmp(&argv[x][2],lang_opt[i])==0) break;
	}
      } else {
	if (strlen(argv[x])!=2) { // Vieleicht mehrere Switches Optionnen wie -bsp
	  int y;
	  for (int z=0;z<strlen(argv[x])-1;z++) {
	    for (y=0;y<opt_count-opt_par_count;y++) {
	      if (argv[x][z+1]==kurz_opt[y+opt_par_count]) break;
	    }
	    if (y==opt_count-opt_par_count)
	      put_error(string("Option nicht bekannt: ")+string(argv[x]));
	    opt_switch[y-opt_par_count]=true;
	  }
	}
	for (i=0;i<opt_count;i++) {
	  if (argv[x][1]==kurz_opt[i]) break;
	}
      }
      if (i==opt_count) 
	put_error(string("Option nicht bekannt: ")+string(argv[x]));
      if (i<opt_par_count) { // Option mit Parameter 
	if (argc-1==x || argv[x+1][0]=='-')
	  put_error(string("Option braucht Parameter: ")+string(argv[x]));
	else {
	  x++;
	  opt_param[i]=argv[x];
	  // Beim -a --algorithmus wird er entschlieï¿½lt und es wird nachgeprft
	  // ob es sich dabei um pivotieren handelt dann zeilen und spalten Nummer lesen 
	  if (i==ealgorithmus) {
	    int t;
	    if (isdigit(opt_param[ealgorithmus][0])) {
	      sscanf(opt_param[ealgorithmus],"%i",&t);
	      if (t>=alg_count) 
		put_error("Nummer des Algorithums zu hoch");
	    } else {
	      for (t=0;t<alg_count;t++) {
		if (strcmp(opt_param[ealgorithmus],algorithmen[t])==0) break;
	      }
	      if (t==alg_count) put_error(string("Algorithums nicht bekannt :") + 
					  string(opt_param[ealgorithmus]));
	    }
	    algorithmus_id=(talgorithmus)t;
	    if (t==pivotiere) {
	      if (argc-1-x<2) put_error(string("Pivotieren braucht Eingabe der Zeilen- und Spaltennummer"));
	      if (!sscanf(argv[x+1],"%i",&pzeile) || !sscanf(argv[x+2],"%i",&pspalte))
		put_error(string("Kann '")+string(argv[x+1])+string("' '")+string(argv[x+2])+
			  string("' als Zeilen- und Spaltennummer fr Pivotieren nicht lesen"));
	      x+=2;
	    }
	  }
	}
      } else
	opt_switch[i-opt_par_count]=true;
    } else {
      inputlist.push_back(argv[x]);
    }
  }
  if (opt_switch[ehelp]) {
    cout<<help_text;
    exit(EXIT_SUCCESS);
  }
  // ï¿½erprfung der semantik
  if (opt_switch[estdin] && !inputlist.empty())  // input aus stdin und trotzdem 
    // noch inputsdateien
    put_error("Bei -s --stdin keine Inputsdateien");
  // Welcher Algorithmus und ob Eingegeben
  if (!opt_param[ealgorithmus])
    put_error("Algorithums muß mit -a festgestellt werden");
}
void interaktive() {
  int com=25,x,y;
  MatrixObject *matrix=0;
  MatrixObject *ergebniss=0;
  char buff[10];
  while (!cin.eof() && !schonende(com)) {
    switch(com) {
    case 1:
      if (matrix) {delete &matrix->prot; delete matrix; matrix=0;} 
      if (ergebniss) { delete ergebniss; ergebniss=0; };
      matrix=loadmatrix(); break;
    case 2: savematrix(matrix); break;
    case 3: printmatrix(matrix); break;
    case 4: savematrix(ergebniss); break;
    case 5: printmatrix(ergebniss); break;
    case 6: openprotokol(matrix); break;
    case 7: doalgorithmus(matrix,ergebniss,gauss); break;
    case 8: doalgorithmus(matrix,ergebniss,jordan); break; 
    case 9: doalgorithmus(matrix,ergebniss,loesung); break; 
    case 10: doalgorithmus(matrix,ergebniss,kern); break;
    case 11: doalgorithmus(matrix,ergebniss,bild); break;
    case 12: doalgorithmus(matrix,ergebniss,inverse); break;
    case 13: doalgorithmus(matrix,ergebniss,determinante); break;
    case 14: doalgorithmus(matrix,ergebniss,naehrung); break;
    case 15: doalgorithmus(matrix,ergebniss,pivotiere); break; 
    case 16: doalgorithmus(matrix,ergebniss,eckenfindung); break;
    case 17: doalgorithmus(matrix,ergebniss,simplex); break;
    case 18: doalgorithmus(matrix,ergebniss,ch_polynom); break;
    case 19: doalgorithmus(matrix,ergebniss,matrixspiel); break;
    case 20: if (ergebniss) { 
      VorbereiteAlgorithmus(ergebniss,matrix);
      delete ergebniss; ergebniss=0;
    } break;
    case 21:
    default:
      cout<<komunikat;
      if (matrix) cout<<"Matrix "<<matrix->mhoehe()<<" x "<<matrix->nbreite()<<'\n';
      if (ergebniss) cout<<"Ergebnis "<<ergebniss->mhoehe()<<" x "<<ergebniss->nbreite()<<'\n';
    }
    cout<<"? ";
    //  cin>>com; verursachte endlosschleife beim eingabe von nicht Ziffern
    cin>>setw(9)>>buff;
    com=0;
    sscanf(buff,"%i",&com);
  }
}
void konvert_tofloat(MatrixObject *&m) {
  if (!m->whoamI()==MatrixObject::tfloat) {
    matrix<rational> *old=(matrix<rational> *)m;
    m=new matrix<double>(old->mhoehe(),old->nbreite(),* new protstream());
    for (int x=0;x<old->nbreite();x++)
      for (int y=0;y<old->mhoehe();y++) {
	((matrix<double> *)m)->operator()(y,x)=old->operator()(y,x).todouble();
      }
    delete &(old->prot);
    delete old;
  }
}
// Ich muï¿½ es definieren sonst sehe #1
template class polynom<rational>;
template class polynom<double>;

typedef matrix<rational> mrational;
typedef matrix<double> mdouble;

void one_algorithmus(MatrixObject *&matrix) {
  MatrixObject *ergebniss=0;
  if (opt_switch[ekonvert]) konvert_tofloat(matrix);
  doalgorithmus(matrix,ergebniss,algorithmus_id);
  pformat->openmain(cout);
  if (opt_switch[eprotokoll]) {
    matrix->prot.PrintOn(cout,*pformat);
  } else {
    if (ergebniss) {
      switch(ergebniss->whoamI()) {
      case MatrixObject::trational:
	// #1 taj mit matrix<rational> staat mrational geht es nicht
	{ 
	  matrixkx<rational>  mrkx((*((mrational *)(ergebniss))),MatrixObject::tmatrix);
	  mrkx.PrintOn(cout,*pformat);
	}
	break;
      case MatrixObject::tfloat:
	{
	  matrixkx<double> mfkx(*((mdouble *)(ergebniss)));
	  mfkx.PrintOn(cout,*pformat);
	}
	break;
      }
    }
  }

  pformat->closemain(cout);
  if (opt_param[eergebniss]!=0 && ergebniss) {
    ofstream erg(opt_param[eergebniss],ios::out);
    if (erg) writeMatrix(ergebniss,erg);
    else 
      put_error(string("Die Ergebnissdatei kann nicht geöffnet werden :")+
		string(opt_param[eergebniss]));
  }
  delete ergebniss;
}
void one_ifile(const char*& file) {
  ifstream we(file,ios::in);
  if (we) {
    MatrixObject *matrix=0;
    matrix=readMatrix(we);
    if (matrix) {
      one_algorithmus(matrix);
      delete &(matrix->prot);
      delete matrix;
    }
  } else {
    put_error(string("Kann ") + string(file) + string(" nicht öffnen"));
  }
}
int main (int argc,char *argv[]) {
  if (argc>1) {
    interactiv=false;
    parse_param(argc,argv);
    ofstream progout;
    MatrixObject *matrix=0;
    if (opt_param[eoutput]!=0) { // Output zu Datei
      progout.open(opt_param[eoutput],ios::out);
      if (!progout) {
	put_error(string("Output zu Datei ") +string(opt_param[eoutput]) +
		  string(" nicht möglich")); 
      }
      // TODO !!!
      // cout=progout;
    }
    try {
      if (opt_param[etypeoutput]!=0)
	pformat=new protformat(opt_param[evorlagen],argv[0],opt_switch[ebruch],opt_param[etypeoutput]);
      else
	pformat=new protformat(opt_param[evorlagen],argv[0],opt_switch[ebruch]);
    } 
    catch(badformat error) {
	  put_error(error.givestring());
    }
    if (opt_switch[estdin]) {
      matrix=readMatrix(cin);
      if (matrix) {
	one_algorithmus(matrix);
      } else {
	put_error(string("Schlechter Input"));
      }
    } else {
      for_each(inputlist.begin(),inputlist.end(),one_ifile);
    }
  } else {
    interactiv=true;
    interaktive();
  }
  return(EXIT_SUCCESS);
}
