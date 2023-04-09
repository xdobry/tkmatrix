#ifndef sets_class
#define sets_class
class sets {
	int *adr;
	int leng,pos;
public:
	sets(int=32);  // initialiesierung für x elemente; vorsicht elemente
	sets(const sets &);	// Kopierconstruktor
	sets & operator = (const sets &);	// operator des Zuschreibens
	~sets();		// werden von 0 nummeriert ähn wie matrizen bei C
	void clear();  // alle löschen
	void add(int);
	void del(int);
	void s_xor(int);
	int exists(int) const;
	void init();		// initialiesirung des Iterationsverfahren
	int next();		// ob der element existiert
	int next_full();	// nr. der nächsten existierenden Elementen, -1 keiner
	int all();			// ? alle Elemente wurden nachgefragt
	int howmany() const;		// wieviel Elemente
};
#endif
