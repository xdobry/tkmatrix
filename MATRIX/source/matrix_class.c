#ifndef matrix_class
#define matrix_class 1
#include "reel_class.c"
#include "sets_class.c"
class matrix
{
	int hoehe,breite;
	reel  *adrmatrix;
public:
	matrix(int=5,int=5);		// hoehe * breite
	matrix(const matrix &);
	matrix & operator = (const matrix &); // mein Compiler wollte das nicht schlucken
	void load(const matrix &); // Ersatz für Obengennanten
	~matrix();
	void gauss();				// primitiver Gaußalgorithmus
	void gauss_jordan();		// Gaußjordan Algorithmus
	void m_gauss(int);			//  ein gauss schritt ab zeile=spalte=x
	void tausche(int,int);		// tausche a mit b zeile
	const sets sprungstellen();		// Sets mit Sprugstellen
	int sstellencount();		// gibt die zahl der sprungstellen zurück
	int sstellencount(const sets &);	// gibt die zahl der sprungstellen zurück
								// int=bitmuster aus sprungstellen
	int nichtnull(int,int=0);   // gibt nr der ersten nicht 0 zeile 
								// in x spalte ab y zeile oder -1
	int mhoehe();				// gibt hohe
	int nbreite();				// breite der matrix
	reel & operator () (int,int); // gibt wert=?.matrix(zeile,spalte)
	matrix loesung();			 // gibt die matrixloesung von
								 // der matrix nach GaußJordan algorithmus
								// x=x1+a*x2+b*x3...
								// x1 - speziele Lösung 
								// x2,x3 Basis des Kerns von Koefizienten matrix
	matrix inverse();			// gibt die Inverse der matrix zurück
								// wenn es keine gibt matrix(0,0)
	matrix bild();				// Gibt das Bild von Matrix
	matrix kern();				// Berechne Kern von Matrix
// proceduren zu Optimierungsverfahren
	void pivotiere (int,int);	// pivotiere an der Stelle hoehe,breite
	int maxkriterium(sets &);	// gibt den spaltennummer nach maximum
			// Kriterium; wenn -1 keine Spaltein; sets=markierte spalten
	int max2kriterium();		// benötigt bei eckenaustauschal.
	int quotkriterium(int);		// gibt	Zeile nach quotentien
			// Kriterium; wenn -1 keine Zeile int=spalte	
	int eckenfindung(); 		// eckenfindungsalgorithmus
			// 0-keine Zeile 1-keine Spalte (also gibt es ein Optimum)
	int eckenaustausch();		// eckenaustauschalgorithmus
			// 0-all b negativ (OK) 1-keine Spalte 2-keine Zeile
			// 3-Zyklus; keine gute Zyklus bedingung
	int sucheecke();			// eckenfindung und eckenaustausch zusammen
			// 1- OK 0-Kein Optimum
	sets einheitszeilen(); 		// gibt den sets mit einheitszeilen
			// von Matrix nach Eckenfindung und Austausch
	matrix rechneloesung(sets &); //Berechnet die Lösung aus Optimierungs
			// matrix nach gefundener Ecke mit Einheitszeilen in sets
	matrix sucheloesung(); // sucht eine Lösung für Optimierung Ver.
	matrix optimiere();	  // Optimierungs verfahren zu Matrix in Form
		// a00*x1+a01*x2 >= b0
		// a10*x1+a11*x2 >= b1
		// Gewinnfunktion (zu Maximieren) g(x)=g0+g1*x1+g2*x2
		// Aussehen der dazugehöriger Matrix 3*3
		// a00 a01 b0
		// a10 a11 b1
		// g1  g2  g0
// Weitere Proceduren
	matrix naehrungsloesung(); // gibt die NL nach (At*A)*x=At*b
// Output
	friend ostream & operator << (ostream &,matrix &);
	void printmatrix();			// schreibt die matrix über stout
};
#endif
