#if !defined(protformat_class)
#define protformat_class

//#include <strclass.h>
#include <string>
#include <list>
#include <fstream>

using namespace std;

// Formatdateien Nammen Aufbau
// objectname.formatname."mform" z.B. matrix.ascii.mform
// m�liche Objecte
// main : er muss existieren; rahmen fr jede ausgabe; fr html z.b <HTML>...</HTML>
// matrix : Matrix mu�existieren
// string : Kommentar fr Protokolle mu�auch existieren
// eqnarray: Lineares Gleichung System (LGS)
// solution: L�ung des LGS als Vektor x1=.... 
// determinante: Determinante det=...
// simplexarray: Ungleichungs System + zu Maximirende Funktion
// werden die Datein nicht gefunden kommt es begrenzten Ausgabe (auch keiner)
/* Format Aufbau
allgemein: 
%feldname feldinhalt
.feldinhalt kann mehrere Zeilen umfassen und gilt bis Zeile mit Anfang %
.Zeilenumbrche werden au�r der Zeile mit %feldname bercksichtig
.feldinhald kann printf steuerung Zeichen enthalten wie %s %d 
.feldinhalt wird in printf Prozedur benutzt
.Es wird ein Zeichen (tab,space,new Line) zwischen feldname und feldinhalt vorausgesetz
Alle Felder mssen fr jedes Objectformatdatei enthalten sein, die Reihenfolge mu�stimmen
Die Struktur ist so geschaffan um Latex formate liefern zu k�nen fr Html
k�nte sie einfacher sein
fr string speziel translate Zeichen (z.b �kann in Html als &auml; transferiert werden)
>�&auml;
-fr main
%head
%tail 
-fr matrix
%head
%headflex : wiederholt n mal; n=breite des Matrix
%headtail
%rowhead
%bruch1  %s
%bruch2  %s,%s
%bruch3  %s,%s,%s
%double  %d
%cellseparator
%rowtail
%tail
- fr string (Vorsicht translate Zeichen)
>�&ouml;
>�&auml;
%head
%tail
- fr eqnarray
%head
%rowhead
%bruch1  %s,%i
%bruch2  %s,%s,%i
%bruch3  %s,%s,%s,%i
%double  %d,%i
%cellseparator
%equal
%rbruch1  %s        ;r wier recht Seite
%rbruch2  %s,%s
%rbruch3  %s,%s,%s
%rdouble  %d
%rowtail
%tail
- fr solution
%head 
%arrayhead
%bruch1 %s
%bruch2 %s,%s
%bruch3 %s,%s,%s
%double %d
%separator
%index + a%i *
%arraytail
%tailmore
%tail
-Vobei ai freiw�lbar
%tail
- fr determinante
%head
%bruch1  %s
%bruch2  %s,%s
%bruch3  %s,%s,%s
%double  %d
%tail
- fr simplex
%head
%headflex
%headtail
%rowhead
%bruch1  %s,%i
%bruch2  %s,%s,%i
%bruch3  %s,%s,%s,%i
%double  %d,%i
%cellseparator
%rowtail
%fhead    : f wie Funktion
%fbruch1  %s,%i
%fbruch2  %s,%s,%i
%fbruch3  %s,%s,%s,%i
%fdouble  %d,%i
%tail
- fr polynom
%head
%bruch1  %s,%i
%bruch2  %s,%s,%i
%bruch3  %s,%s,%s,%i
%double  %d,%i
%obruch1  %s        ;r wier recht Seite
%obruch2  %s,%s
%obruch3  %s,%s,%s
%odouble  %d
%tail
- fr scalar
%bruch1  %s,%i
%bruch2  %s,%s,%i
%bruch3  %s,%s,%s,%i
%double  %d,%i
*/
#define formmustbe 3
#define objectcount 9
// Beim Einfgen von neuen protokoltypen
// 1. objectcont erh�en#
// 2. neues static char *neuertyp[]; in protformat Deklaration
// 3. *protformat::objects[] in protformat.cc erg�zen
// 4. char *neuertyp[] in protformat.cc kreieren
// 5. int protformat::feldcount[objectcount] in protformat.cc erg�zen
// 6. z.B feldtab[fscalar]=sscalar in protformat::protformat erg�zen
class badformat {
private:
// Die Fehlerklasse wurde mehrmal ge�dert
// Mein Compiler konnte keine destruktoren zu Memberelementen aufrufen und
// selbst kein Destruktor bilden
// Die Klasse war zuerst als string * errostring definiert das verursachte 
// memoryleaks. Jetzt char errorstring[200];
// Tja warum den ein Zeiger; mit normallen string spuckt mein gcc compiler
// matrix_p.cc:403: Internal compiler error.
// matrix_p.cc:403: Please submit a full bug report to `bug-g++@prep.ai.mit.edu'.
 char errorstring[200];
public:
 badformat(string s);
// ~badformat();
// Destruktor geht auch nicht; ich setze voraus, da�nach diesem throw & catch
// das Program unterbrochen wird, sonst memory leaks (!! nur beim alten L�ung)
 char *givestring();
};
// inline badformat::badformat(string s) { errorstring=new string(s); }
// inline badformat::~badformat() { delete errorstring; }       
class protformat {
public:
  protformat(const char *dirpath,char *argv0,bool nurbruch,const char *form="ascii");
  ~protformat();
  enum tformobject {fmatrix,fstring,fmain,feqnarray,fsolution,fdeterminante,
		    fsimplex,fpolynom,fscalar };
// objectcount wird als eine konstante benutzt um Zahl der Objecte festzustellen
private:
// Vorsicht beim �derungen; Es bestehen Ubh�gigkeiten zwichen feldcount,formmustbe
// objects ,smain(und s*)
  static char *objects[];
  static int feldcount[objectcount];
  static char *smain[];
  static char *smatrix[];
  static char *sstring[];
  static char *seqnarray[];
  static char *ssolution[];
  static char *sdeterminante[];
  static char *ssimplex[];
  static char *spolynom[];
  static char *sscalar[];

  string **sgraph[objectcount];
  struct translate {
    char from;
    string to;
  };
  list<translate> translist;
public:
  // Optionnen fr Formatierung der Ausgabe 
  static bool nurbruch,widefloat_reformat;
  static int widefloat_showprec,widefloat_maxnum,widefloat_maxzeros;

  void openmain(ostream &);
  void closemain(ostream &);
  const char *givefstring(tformobject,int);
  bool form_exist(tformobject);
  string translate(const string &);
private:
  void suchedatei(ifstream &,string,string &,char *);
};
#endif








