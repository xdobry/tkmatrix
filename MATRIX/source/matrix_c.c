#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

#include "reel_class.c"
#include "sets_class.c"
#include "matrix_class.c"

extern ofstream prot;
// Filestuktur, wenn !=0 dann wird es zu den jedes schritt von Algorithmen
// geschrieben in Form von Protokol

matrix::matrix (int h,int b)
{
	hoehe=h; breite=b;
	adrmatrix = new reel [h*b];
}
matrix::matrix(const matrix & m)
{
	hoehe=m.hoehe;
	breite=m.breite;
	adrmatrix = new reel [hoehe*breite];
	int i;
	for(i=0;i<hoehe*breite;i++) adrmatrix[i]=m.adrmatrix[i];
}
// matrix & matrix::operator = (matrix & z) {
void matrix::load(const matrix &z) {
	if (this!=&z) {
		delete adrmatrix;
		hoehe=z.hoehe;
		breite=z.breite;
		adrmatrix=new reel[hoehe*breite];
		reel *adr1=adrmatrix,*adr2=z.adrmatrix;
		int i;
		for(i=0;i<hoehe*breite;i++) *adr1++=*adr2++;
	}
//	return *this; // nur bei definition mit operator =
}
matrix::~matrix() {
	delete adrmatrix;
}
int matrix::mhoehe () {
	return hoehe;
}
int matrix::nbreite () {
	return breite;
}
reel & matrix::operator () (int i,int j) {
	if ((i<0) || (i>=hoehe)) i=0;
	if ((j<0) || (j>=breite)) j=0;
	return adrmatrix[i*breite+j];
}
int matrix::nichtnull(int kol,int first) {
	int i;
	for (i=first;i<hoehe;i++)
		if (adrmatrix[i*breite+kol][0]) return i;
	return -1;
}
void matrix::tausche(int a,int b) {
	reel x;
	int i;
	if (a<0 || b<0 || a>=hoehe || b>=hoehe || b==a) return;
	for (i=0;i<breite;i++) {
		x=adrmatrix[a*breite+i];
		adrmatrix[a*breite+i]=adrmatrix[b*breite+i];
		adrmatrix[b*breite+i]=x;
	}
}
void matrix::m_gauss(int i) {
	int x,y; // x - spalten y - zeilen Zöhler
	reel *adr,*adr2; // nur für Beschleunigung
					 // und für Zeiger Spezielisten
	reel a,b;
	x=nichtnull(i,i);
	if (x>=0) { tausche(x,i);
		// (i,i) Antrag auf 1 skalieren zeile durch (i,i) teilen
		adr=adrmatrix+breite*i+i;
		a=*adr; *adr++=1;
		for (x=i+1;x<breite;x++) { *adr=*adr/a; adr++; }
		// durch Zeilen addierung Antrag (y,i) auf Null bringen
		// x.Zeile=x.Zeile-x.Zeile(y,i)*operation.Zeile
		for (y=i+1;y<hoehe;y++) {
			adr2=adrmatrix+breite*y+i;
			if ((*adr2)[0]) {
				a=*adr2;
				*adr2++=0;	// ersten Antrag anmerken und sofort auf null setzen
							// addition ab nöchster spalte
				adr=adrmatrix+breite*i+(i+1);
				for (x=i+1;x<breite;x++) {
					b=a*(*adr);
					*adr2=(*adr2)-b;
					adr2++; adr++;
				}
			}
		}
	}
}
void matrix::gauss() {
	int i;
	if (prot) {
		prot<<"gauss von\n";
		prot<<*this;
	}
	for (i=0;i<breite && i<hoehe ;i++) { 
		m_gauss(i);
		if (prot) {
			prot<<"gauss nr."<<i<<'\n';
			prot<<*this;
		}
	}
}
void matrix::gauss_jordan() {
	int i,j,k; 
	reel a,b;
	reel *adr,*adr2;
	if (prot) {
		prot<<"gauss von";
		prot<<*this;
	}
	for (i=0;i<breite && i<hoehe ;i++) {
		m_gauss(i);
		if (adrmatrix[i*breite+i][0]) {
		// dieses Teil ist mit der aus m_gauss sehr öhnlich
			for (j=0;j<i;j++) {
				adr2=adrmatrix+breite*j+i;
				if ((*adr2)[0]) {
					a=*adr2;
					*adr2++=0;	// ersten Antrag anmerken und sofort auf null setzen
								// addition ab nöchster spalte
					adr=adrmatrix+breite*i+(i+1);
					for (k=i+1;k<breite;k++) {
						b=a*(*adr);
						*adr2=(*adr2)-b;
						adr2++; adr++;
					}
				}
			}
		}
		if (prot) {
			prot<<"gauss_jordan Nr."<<i<<'\n';
			prot<<*this;
		}
	}
}
const sets matrix::sprungstellen() {
	int x,y,last;
	sets s(breite);
	last=breite-1;
	for (y=hoehe-1;y>=0 && last!=-1;y--)
		for (x=0;x<=last;x++)
			if (adrmatrix[y*breite+x][0]) {
				last=x-1; 
				s.add(x);
			}
	if (y!=-1) s.clear();
	return s;
}
int matrix::sstellencount(const sets & a) {
	return a.howmany();
}
int matrix::sstellencount() {
	return sstellencount(sprungstellen());
}
matrix matrix::loesung() {
	sets s=sprungstellen();
	int c=s.howmany();
	if (c==breite || s.exists(breite-1)) { matrix l(0,0); return l; }
	matrix l(breite-1,breite-c);
	int x,y,z,v;
	y=0;
	for (x=0;x<breite-1;x++) {
		if (s.exists(x)) { l(x,0)=adrmatrix[y*breite+breite-1]; y++; }
			else l(x,0)=0;
	}
	y=1;
	for (x=0;x<breite-1;x++) {
		if (!s.exists(x)) {
			v=0;
			for (z=0;z<breite-1;z++) {
				if (s.exists(z)) { l(z,y)=-adrmatrix[v*breite+x]; v++; }
					else l(z,y)=0;
			}
			l(x,y)=1;
			y++;
		}
	}
	if (prot) {
		prot<<"Lösung\n";
		prot<<l;
	}
	return l;
}
matrix matrix::inverse () {
	if (breite!=hoehe) { 
		cout<<"Man kann nur eine Inverse von n*n Matrix ausrechnen\n";
		matrix s(0,0); return s;
	}
	matrix s(breite,breite*2);
	int x,y;
	reel a;
	for (y=0;y<hoehe;y++)
		for (x=0;x<breite;x++)
			s(y,x)=adrmatrix[y*breite+x];
	a=0;
	for (y=0;y<hoehe;y++)
		for (x=breite;x<breite*2;x++)
			s(y,x)=a;
	a=1;
	for(x=0;x<breite;x++)
		s(x,x+breite)=a;
	s.gauss_jordan();
	if (s.sstellencount()!=breite) { matrix s(0,0); return s; }
	 // matrix nicht inventierbar spungstellen liegen nicht nacheinander
	matrix i(hoehe,breite);
	for (y=0;y<hoehe;y++)
		for (x=0;x<breite;x++)
			i(y,x)=s(y,x+breite);
	if (prot) {
		prot<<"Inverse\n";
		prot<<i;
	}
return i;
}
matrix matrix::bild() {
	matrix b(*this);
	b.gauss_jordan();
	sets s=b.sprungstellen();
	int x,y,spalte,dim=s.howmany();
	matrix l(hoehe,dim);
	s.init();
	for (x=0;x<dim;x++) {
		spalte=s.next_full();
		for(y=0;y<hoehe;y++)
			l.adrmatrix[y*l.breite+x]=adrmatrix[y*breite+spalte];
	}
	if (prot) {
		prot<<"Bild von Matrix";
		prot<<l;
	}
return b;
}
matrix matrix::kern() {
	int x,y;
	// Bilde eine matrix mit zusötzlicher Nullspalte
	matrix b(hoehe,breite+1);
	for (y=0;y<hoehe;y++)
		for(x=0;x<breite;x++)
			b.adrmatrix[y*b.breite+x]=adrmatrix[y*breite+x];
	b.gauss_jordan();
	b.load(b.loesung());
	if (b.breite==1) {
		if (prot) {
			prot<<"Kern von Matrix\n";
			prot<<b;
		}
		return b;
	}
	matrix l(b.hoehe,b.breite-1);
	for (y=0;y<l.hoehe;y++)
		for(x=0;x<l.breite;x++)
			l.adrmatrix[y*l.breite+x]=b.adrmatrix[y*b.breite+x+1];
	if (prot) {
		prot<<"Kern von Matrix\n";
		prot<<l;
	}
return l;
}
void matrix::pivotiere (int zeile,int spalte) {
	reel a=adrmatrix[zeile*breite+spalte];
	reel b;
	if (a[0] && zeile<hoehe && spalte<breite) { // Wenn Null dann 
												// keine Pivotierung
		reel *adr,*adr2;
		int  x,y;
		if (!(a[0]==1 && a[1]==1)) {// Wenn 1 dann Teilung unnötig 
			adr=adrmatrix+spalte;
			for (y=0;y<hoehe;y++) {
				*adr=*adr/a;
				adr+=breite;
			}
		}
		for (x=0;x<breite;x++) {
			if (x!=spalte) {
				adr2=adrmatrix+spalte;
				adr=adrmatrix+x;
				a=adrmatrix[zeile*breite+x];
				for (y=0;y<hoehe;y++) {
					b=a*(*adr2);
					*adr=*adr-b;
					adr+=breite;
					adr2+=breite;
				}
			}
		}
	}
}
int matrix::maxkriterium(sets &mark) {
	int erg=-1,x,y;
	reel *adr=adrmatrix+(hoehe-1)*breite; // an Anfang der g zeile setzen
	reel hoechst,a;
	for (x=0;x<breite-1;x++) {
		if (!mark.exists(x))  { // Keine spalte markiert
			if (erg<0) {
				for (y=0;(y<hoehe && (adrmatrix[y*hoehe+x])[0]==0);y++);
					// ob null spalte
				if (y<hoehe) {
					erg=x;
					hoechst=adr[x].abs();
				}
			} else {
				a=adr[x].abs();
				if (a>hoechst) {
					erg=x;
					hoechst=a;
				}
			}
		}
	}
return erg;
}
int matrix::max2kriterium() {
	int erg=-1,x,y;
	reel *adr=adrmatrix+(hoehe-1)*breite; // an Anfang der g zeile setzen
	reel hoechst;
	for (x=0;x<breite-1;x++) {
		if (erg<0) {
			for (y=0;(y<hoehe && (adrmatrix[y*hoehe+x])[0]==0);y++);
				// ob null spalte
			if (y<hoehe) {
				erg=x;
				hoechst=adr[x];
			}
		} else {
			if (adr[x]>hoechst) {
				erg=x;
				hoechst=adr[x];
			}
		}
	}
return erg;
}
int matrix::quotkriterium(int spalte) {
	int erg=-1,y;
	reel min,a;
	for (y=0;y<hoehe-1;y++) {
		a=adrmatrix[breite*(hoehe-1)+spalte]*adrmatrix[breite*y+spalte];
		if ((reel)0>a) {
			if (erg<0) {
				erg=y;
				a=(adrmatrix[breite*y+spalte].abs());
				min=adrmatrix[breite*y+breite-1]/a;
			} else {
				a=(adrmatrix[breite*y+spalte].abs());
				a=adrmatrix[breite*y+breite-1]/a;
				if (min>a) {
					erg=y;
					min=a;
				}
			}
		}
	}
return erg;
}
int matrix::eckenfindung() {
	int s,z; 
	sets mark(breite-1);
	if (prot) {
		prot<<"Eckenfindung von\n";
		prot<<*this;
	}
	while (1) {
		s=maxkriterium(mark);
		if (s<0) return 1;
		z=quotkriterium(s);
		if (z<0) return 0;
		pivotiere(z,s);
		if (prot) {
			prot<<"pivotiere ("<<z+1<<','<<s+1<<")\n";
			prot<<*this;
		}
		mark.add(s);
	}
}
int matrix::eckenaustausch() {
	int x,z,s,ind=0;
	for (x=0;x<breite-1;x++) { // es gibt breite-1 ecken
		for (z=0;(z<breite-1 && (reel)0>=adrmatrix[breite*(hoehe-1)+z]);z++);
		if (z==breite-1) return 0;
		if (prot && !ind) {
			prot<<"Eckenaustausch\n";
			ind=1;
		}
		s=max2kriterium();
		if(s<0) return 1;
		z=quotkriterium(s);
		if (z<0) return 2;
		pivotiere(z,s);
		if (prot) {
			prot<<"pivotiere ("<<z+1<<','<<s+1<<")\n";
			prot<<*this;
		}
	}
return 3;
}
int matrix::sucheecke() {
	if(!eckenfindung()) {
		cout<<"Es gibt keine optimale Lösung\n";
		if (prot) {
			prot<<"Es gibt keine optimale Lösung";
		}
		return 0;
	}
	if(!eckenaustausch()) return 1;
	else return 0;
}
sets matrix::einheitszeilen() {
	sets s(hoehe-1);
	int x,y;
	for(y=0;y<hoehe-1;y++) {
		for(x=0;x<breite && adrmatrix[y*breite+x]==0;x++);
		if (adrmatrix[y*breite+x]==1) {
			for(x=x+1;x<breite && adrmatrix[y*breite+x]==0;x++);
			if (x==breite) s.add(y); 
		}
	}
return s;
}
matrix matrix::rechneloesung (sets &mark) {
	int z,x,y,h=mark.howmany();
	if (h==0) { matrix l(0,0); return l; }
	matrix l(h,breite);
	mark.init();
	for(y=0;y<h;y++) {
		z=mark.next_full();
		for(x=0;x<breite;x++)
			l.adrmatrix[breite*y+x]=adrmatrix[breite*z+x];
	}
	l.gauss_jordan();
	return (l.loesung());
}
matrix matrix::sucheloesung() {
	int x,y;
	// ob nullvektor die Lösung ist
	cout<<"Ich suche eine Lösung\n";
	for (y=0;y<hoehe-1 && 0>=adrmatrix[y*breite+breite-1];y++);
	if (y==hoehe-1) { matrix l(breite-1,1); return l; }
	matrix b(hoehe,breite+1);
	reel a=adrmatrix[breite-1]; // auf erstes be setzen 
	// kopiern zu B_dach matrix und suche gröötes b gleichzeitig
	for (y=0;y<hoehe-1;y++) {
		if (adrmatrix[y*breite+breite-1]>a) a=adrmatrix[y*breite+breite-1];
		for (x=0;x<breite-1;x++)
			b.adrmatrix[y*b.breite+x]=adrmatrix[breite*y+x];
		b.adrmatrix[y*b.breite+b.breite-2]=-1;
	}
	// Letze Zeile in B_dach Einrichten b_max-b
	// Ausgang matrix B_dach sichern
	matrix bausgang(b);
	for (y=0;y<hoehe-1;y++) {
		x=y*b.breite+b.breite-1;
		b.adrmatrix[x]=a-adrmatrix[y*breite+breite-1];
		bausgang.adrmatrix[x]=adrmatrix[y*breite+breite-1];
	}
	x=(b.hoehe-1)*b.breite+b.breite-1;
	// x auf letztes Antrag in der M. setzen
	b.adrmatrix[x]=-a;
	b.adrmatrix[x-1]=1;
	if (prot) {
		prot<<"Ausgangsmatrix zur Suche nach einer Lösung\n";
	}
	if (!b.sucheecke()) {
		a=b.adrmatrix[x];
		if (a>=0) {
			// Kommt nur vor bei den Ungleichungen ohne Beschränkung
			// Suche lösung wenn der Gewinn >=0
			// Lösung von Ursprung b_dach ohne gewinnzeile
			// Letzte Splate=Gewinn+(letzte splate)Ursprung B_dach+Letztes B_Dach
			reel hr;
			matrix bhilf(hoehe-1,breite+1);
			for (y=0;y<bhilf.hoehe;y++) {
				for(x=0;x<bhilf.breite-2;x++)
					bhilf.adrmatrix[y*bhilf.breite+x]=adrmatrix[y*breite+x];
				bhilf.adrmatrix[y*bhilf.breite+x]=-1;
				hr=0;
				x++; // x auf letzte spalte setzen
				hr=a+b.adrmatrix[y*b.breite+x];
				hr=hr+adrmatrix[y*breite+x-1]; // matrix um eine spalte kürzer
				bhilf.adrmatrix[y*bhilf.breite+x]=hr;
			}
			if (prot) 
				prot<<"Gewinn positiv es gibt also eine Lösung\n";
			bhilf.gauss_jordan();
			bhilf.load(bhilf.loesung());
			matrix l(breite-1,1);
			for(y=0;y<breite-1;y++)
				l.adrmatrix[y]=bhilf.adrmatrix[bhilf.breite*y];
			return l;
		} else {
			cout<<"Keine Lösung\n";
			if (prot) 
				prot<<"Keine Lösung\n";
			matrix l(0,0); return l;
		}
	}
	sets ls(b.einheitszeilen());
	matrix lb(bausgang.rechneloesung(ls));
	if (lb.hoehe==0) {
		matrix l(0,0); return l;
	}
	matrix l(breite-1,1);
	for(y=0;y<breite-1;y++)
		l.adrmatrix[y]=lb.adrmatrix[lb.breite*y];
	return l;
}
matrix matrix::optimiere() {
	if(prot) {
		prot<<"optimiere\n";
		prot<<*this;
	}
	matrix l(sucheloesung());
	if (l.hoehe==0) {
		matrix l(0,0); return l;
	}
	if (prot) {
		prot<<"Eine Lösung\n";
		prot<<l;
	}
	matrix b_(*this);
	reel a,b;
	int x,y;
	for (y=0;y<hoehe;y++) {
		a=0;
		for (x=0;x<breite-1;x++) {
			b=adrmatrix[y*breite+x]*l.adrmatrix[x*l.breite];
			a=a+b;
		}
		b=adrmatrix[y*breite+breite-1];
		b_.adrmatrix[y*breite+breite-1]=a-b;
	}
	if (prot) {
		prot<<"Ausgangsmatrix zu Eckensuche\n";
	}
	if (!b_.sucheecke()) {
				matrix l(0,0); return l;
	}
	sets ls=b_.einheitszeilen();
	matrix l2(rechneloesung(ls));
	if (l2.mhoehe()) {
		cout<<"Lösung gefunden\n";
	}
	return l2;
}
matrix matrix::naehrungsloesung() {
	if (hoehe<=breite-1) { 
		gauss_jordan(); 
		return loesung();
	}
	matrix b(breite-1,breite);
	int x,y,z;
	reel a,c;
	for (y=0;y<breite-1;y++) {
		for (x=0;x<breite-1;x++) {
			a=0;
			for (z=0;z<hoehe;z++) {
				c=adrmatrix[z*breite+x]*adrmatrix[z*breite+y];
				a=a+c;
			}
			b.adrmatrix[y*b.breite+x]=a;
		}
		a=0;
		for(z=0;z<hoehe;z++) {
			c=adrmatrix[z*breite+y]*adrmatrix[z*breite+breite-1];
			a=a+c;
		}
		b.adrmatrix[y*breite+breite-1]=a;
	}
	if (prot) {
		prot<<"Ausgangmatrix (At*A)x=At*b"<<std::endl;
	}
	b.gauss_jordan();
	return (b.loesung());
}
ostream & operator << (ostream & wy,matrix & a) {
	int m1,n1;
	for (m1=0;m1<a.hoehe;m1++) {
		for (n1=0;n1<a.breite;n1++) wy<<a.adrmatrix[m1*a.breite+n1]<<'\t';
	wy<<'\n';
	}
return wy;
}
void matrix::printmatrix() {
	cout<<*this;
}
