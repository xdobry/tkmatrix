#ifndef sets_class
#define sets_class
class sets {
	int *adr;
	int leng,pos;
public:
	sets(int=32);  // initialiesierung f�r x elemente; vorsicht elemente
	sets(sets &);	// Kopierconstruktor
	sets & operator = (sets &);	// operator des Zuschreibens
	~sets();		// werden von 0 nummeriert �hn wie matrizen bei C
	void clear();  // alle l�schen
	void add(int);
	void del(int);
	void xor(int);
	int exists(int);
	void init();		// initialiesirung des Iterationsverfahren
	int next();		// ob der element existiert
	int next_full();	// nr. der n�chsten existierenden Elementen, -1 keiner
	int all();			// ? alle Elemente wurden nachgefragt
	int howmany();		// wieviel Elemente
};
#endif
