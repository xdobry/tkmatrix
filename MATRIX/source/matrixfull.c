#include <iostream.h>
#include <stdio.h>

#include "matrix_class.c"

char komunikat[]="MatrixReel geschrieben von Artur Trzewik 07.1996\n"
" 1 Matrix Laden              11 Kern von Matrix\n"
" 2 Matrix Sichern            12 Bild von Matrix\n"
" 3 Matrix Ausgeben           13 Inverse von Matrix\n"
" 4 Ergebnis Sichern          14 Nährungslösung\n"
" 5 Ergebnis Ausgeben         15 Pivotiere\n"
" 6 Protokoll Öffnen          16 Eckenfindung\n"
" 7 Protokoll Schließen       17 Eckenaustausch\n"
" 8 Gauß-algorithmus          18 Optiemiere\n"
" 9 Gauß-Jordan-algorithmus   19 Matrix = Ergebnis\n"
" 10 Lösung Ausrechnen        20 Programm beenden\n"
" (jede andere Taste dises Text)\n";
char welcherpro[]=" Die Protokollmenu\n"
"  1 Ausgabe auf den Bildschrimm\n"
"  2 Ausgabe zu einer Datei\n"
"  3 Kein Protokoll\n"
"? PM ";

int full(matrix &x) {
	if (x.mhoehe()==0 || x.nbreite()==0) { cout<<"Zuerst Matrix Laden\n";
		return 0;
	} else return 1;
}
int schonende(int a) {
	char b;
	if (a==20) {
		cout<<"Programm beenden, bist du sicher ? (j/n) ";
			cin>>b;
			if (b=='j' || b=='J') return 1;
	}
return 0;
}
#define DLMAX 79
matrix loadmatrix() {
	char name[DLMAX+1];
	FILE *we;
	int m,n,m1,n1;
	cout<<"Gibt die Name des Datei: ";
//	cin>>setw(DLMAX)>>name;		// mein compiler nimmt das nicht an
	cin>>name;
	we=fopen(name,"r");
	if (we) {
		we>>m; // matrix m x n m-hoehe n-breite
		we>>n;
		matrix a(m,n);
		for (m1=0;m1<m;m1++) 
			for (n1=0;n1<n;n1++) we>>a(m1,n1);
		fclose(we);
		cout<<"Geladen wurde Matrix "<<m<<" x "<<n<<'\n';
		return a;
	} else { cout<<"Ich konnte die datei nicht finden\n";
		matrix a(0,0);
		return a;
	}
}
void savematrix(matrix &a) {
	char name[DLMAX];
	FILE *wy;
	if (a.mhoehe()!=0 && a.nbreite()!=0) {
		cout<<"Speichert unter: ";
//		cin>>setw(DLMAX)>>name;		// mein compiler nimmt das nicht an
		cin>>name;
		wy=fopen(name,"w");
		if (wy) {
			wy<<a.mhoehe()<<' '; // matrix m x n m-hoehe n-breite
			wy<<a.nbreite()<<'\n';
			wy<<a;
			fclose(wy);
		} else cout<<"Datei konnte nicht geöffnet werden\n";
	} else cout<<"Es gibt nichts zu speichern\n";
}
void printmatrix(matrix &a) {
	if (a.mhoehe()!=0 && a.nbreite()!=0) {
		cout<<"Matrix "<<a.mhoehe()<<" x "<<a.nbreite()<<'\n';
		cout<<a;
	} else cout<<"Nichts auszugeben\n";
}
FILE *prot=0;
void closeprotokol() {
	if (prot) {
		if (prot==stdout) prot=0;
		else {
			fclose(prot);
			prot=0;
			cout<<"Protokoll wurde geschlossen\n";
		}
	}
}
void openprotokol() {
	char name[DLMAX];
	int com;
	cout<<welcherpro;
	cin>>com;
	switch(com) {
	case 1:
		closeprotokol();
		prot=stdout;
		break;
	case 2:
		closeprotokol();
		cout<<"Protokoll speichern unter: ";
	//	cin>>setw(DLMAX)>>name;		// mein compiler nimmt das nicht an
		cin>>name;
		prot=fopen(name,"w");
		if (!prot) cout<<"Protokoll konnte nicht geöffnet werden\n";
		break;
	case 3: break;
	default:
		cout<<"Falsche Eingabe\n";
	}
}
// a.load sollte man durch a= ersetzen mein compiler wollte das aber nicht
main () {
	int com=25,x,y;
	matrix a(0,0),b(0,0);
	while (!schonende(com)) {
	switch(com) {
		case 1: a.load(loadmatrix()); break;
		case 2: savematrix(a); break;
		case 3: printmatrix(a); break;
		case 4: savematrix(b); break;
		case 5: printmatrix(b); break;
		case 6: openprotokol(); break;
		case 7: closeprotokol(); break;
		case 8: if (full(a)) { b.load(a); b.gauss(); } break;
		case 9: if (full(a)) { b.load(a); b.gauss_jordan(); } break;
		case 10: if (full(a)) { b.load(a); 
					b.gauss_jordan(); b.load(b.loesung()); } break;
		case 11: if (full(a)) { b.load(a.kern()); } break;
		case 12: if (full(a)) { b.load(a.bild()); } break;
		case 13: if (full(a)) { b.load(a.inverse()); } break;
		case 14: if (full(a)) { b.load(a.naehrungsloesung()); } break;
		case 15: if (full(a)) { cout<<"Gibt die Zeilen Nr 1-"<<a.mhoehe()<<": ";
				cin>>y; 
				cout<<"Gibt die Spalten Nr 1-"<<a.nbreite()<<":  ";
				cin>>x;
				if (y<=a.mhoehe() && x<=a.nbreite()) {
					b.load(a); b.pivotiere(y-1,x-1);
					if (prot) {
						prot<<a;
						prot<<"pivotiere an der Stelle ("<<y<<','<<x<<")\n";
						prot<<b;
					}
				} } break;
		case 16: if (full(a)) { b.load(a); b.eckenfindung(); } break;
		case 17: if (full(a)) { b.load(a); b.eckenaustausch(); } break;
		case 18: if (full(a)) { b.load(a.optimiere()); } break;
		case 19: if (b.mhoehe()!=0) a.load(b); break;
		case 20: break;
		default:
			cout<<komunikat;
			cout<<"Matrix "<<a.mhoehe()<<" x "<<a.nbreite()<<'\n';
			cout<<"Ergebnis "<<b.mhoehe()<<" x "<<b.nbreite()<<'\n';
	}
	cout<<"? ";
	cin>>com;
	}
	closeprotokol();
}
