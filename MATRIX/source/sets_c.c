#include "sets_class.c"
int hmint(int a) {
	return (a+31)>>5;
}
sets::sets(int lenght) {
	int x,lint=hmint(lenght);
	leng=lenght;
	adr=new int[lint];
	for (x=0;x<lint;x++) adr[x]=0;
}
sets::sets(const sets & z) {
	pos=z.pos;
	leng=z.leng;
	int x,lint=hmint(leng);
	adr=new int[lint];
	for(x=0;x<lint;x++) adr[x]=z.adr[x];
}
sets & sets::operator = (const sets & z) {
	if (this!=&z) {
		delete adr;
		int x,lint=hmint(z.leng);
		adr=new int[lint];
		leng=z.leng;
		pos=z.pos;
		for (x=0;x<lint;x++) adr[x]=z.adr[x];
	}
	return *this;
}
sets::~sets() {
	delete adr;
}
void sets::clear() {
	int x,l=hmint(leng);
	for(x=0;x<l;x++) adr[x]=0;
}
void sets::add(int x) {
	if (x<leng)
	adr [x>>5]|=1<<(x&0x1F);
}
void sets::del(int x) {
	if (x<leng)
	adr[x>>5]&=0xFFFFFFFF^(1<<(x&0x1F)); // adr=adr AND ($FFFFFF XOR bit)
}
void sets::s_xor(int x) {
	if (x<leng)
	adr[x>>5]^=1<<(x&0x1F); // adr=adr XOR bit
}
int sets::exists(int x) const {
	return adr[x>>5]&(1<<(x&0x1F));
}
void sets::init() {
	pos=0;
}
int sets::next() {
	return exists(pos++);
}
int sets::next_full() {
	while (!exists(pos) && pos<leng) pos++;
	if (pos<leng)
		return pos++;
	else
		return -1;
}
int sets::all() {
	return pos>=leng;
}
int sets::howmany() const {
	int x,erg=0;
	for (x=0;x<leng;x++) {
		if (exists(x)) erg++;
	}
    return erg;
}
