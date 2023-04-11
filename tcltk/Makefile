VERSION = 0.7.0
RELASE = 0
INSTALLDIR = /usr/lib/tkmatrix
LINKDIR = /usr/bin
TCLPATH = /opt/tcl8412

objects = matrix3.o protstream.o protformat.o rational.o widefloat.o
outfile = matrix
tcllib = matrixtcl
floatlib = float
winobjects = winsupport.o float.o myappinit.o

# Warning some Linux Distribution do not include tcl and tk header file
# as (tcl.h tk.h) in tcl und tk package as RedHat (location /usr/include)
# So add an Include Path to this header flies tcl.h tk.h
# hier the tcl.h and tk.h are in working-dictionary
TCLTKHEADER = -I/usr/include/tcl
CFLAGS = $(TCLTKHEADER) -fpic
# specially for cygnus linux-utils package
# to compile Windows Version
# make tkmatrix.exe
WINLIBS = -lstdc++ -ltk80 -ltcl80
# for old gcc uncomment next line
#CPPFLAGS =  -fhandle-exceptions
#commend next line if you use old STL-Library with string::remove
#Add -DNO_GMP to compile tkmatrix without gmp library
#CXXFLAGS = -Dstd_noremove -DNO_GMP
CXXFLAGS = -Dstd_noremove $(TCLTKHEADER) -fpic
# Set gmp Libs GNU GNU multiple precision arithmetic library
#comment it out if you don't use it
MLIBS = -lgmp
#CXXFLAGS = -g -fhandle-exceptions
# CXXFLAGS = -fhandle-exceptions -frtti
#CXXFLAGS = -frepo
# mit letzen Option wollte der Compiler die streambuf.h nicht schlucken
# darum sind alle dynamic_cast auskommentiert und durch normalle ersetzt
ALL: $(outfile) $(tcllib).so $(floatlib).so
$(outfile) : $(objects) matrix_p.o polynom.h
	$(CXX) $(objects) matrix_p.o $(MLIBS) -o $(outfile)
	strip $(outfile)
$(tcllib).so : $(objects) matrixtcl.o matrixtcl_object.o
	$(CXX) $(objects) matrixtcl.o  matrixtcl_object.o $(MLIBS) \
	-pipe -shared -ltclstub -o $(tcllib).so
$(floatlib).so : float.o
	$(CC) float.c -pipe -shared -fpic -ltclstub -ltkstub -o $(floatlib).so $(TCLTKHEADER)

# Static Version of tkmatrix 
#myappinit.o : myappinit.c

tkmatrixstat : float.o $(objects) matrixtcl.cc matrixtcl_object.o myappinit.o
	$(CXX) float.o $(objects) matrixtcl.o  matrixtcl_object.o \
	myappinit.o -ltk -ltcl -ldl -L/usr/X11R6/lib/ -lX11 $(MLIBS) \
	-o tkmatrixstat

# Windows Version compiled by cygwin - Cygnus Solution Windows egcs Compiler
#winsupport.o : winsupport.c

resources.res : resources.rc
	windres -O coff resources.res resources.rc
tkmatrix.exe : $(objects) matrixtcl.o matrixtcl_object.o $(winobjects)
	$(CXX) -mwindows -e_mainCRTStartup resources.res $(objects) $(winobjects) \
   matrixtcl.o matrixtcl_object.o -L. $(MLIBS) $(WINLIBS) -o tkmatrix.exe

testAppInit.o : testAppInit.c
testwish.exe : $(objects) matrixtcl.o matrixtcl_object.o winsupport.o float.o testAppInit.o
	$(CXX) $(objects) winsupport.o float.o testAppInit.o matrixtcl.o matrixtcl_object.o -L. $(MLIBS) $(WINLIBS) -o testwish.exe

widefloat.o : widefloat.h
rational.o : rational.h
$(objects) : matrix3.h polynom.h
$(tcllib).o : matrix3.h polynom.h
protstream.o : protstream.h protformat.h
protformat.o : protformat.h
matrix_p.o : protstream.h
matrixtcl.o matrixtcl_object.o : matrixtcl.h

.PHONY : clean tar zip zipupdate install uninstall
clean:
	rm $(objects) matrix_p.o $(tcllib).o  matrixtcl_object.o  $(winobjects)
tar:
	mkdir -p tkmatrix-$(VERSION)
	cp *.rc *.c *.cc *.h tkmatrix.tcl tkmatrix.xotcl *.mform \
	*.mat *.lan *.tkm *.lsm Makefile README LIESMICH \
	LIESMICH_WIN.txt \
	tkmatrix.ico tkmatrix-$(VERSION)
	mkdir -p tkmatrix-$(VERSION)/docus
	cp docus/*.html docus/*.sgml docus/*.pdf docus/*.ps docus/*.tex \
	docus/ISOlat1.2html docus/mapping docus/*.gif tkmatrix-$(VERSION)/docus
	mkdir -p tkmatrix-$(VERSION)/images
	cp images/*.gif tkmatrix-$(VERSION)/images
	tar czvf tkmatrix-$(VERSION)-$(RELASE).tar.gz tkmatrix-$(VERSION)
	rm -f -r tkmatrix-$(VERSION)
zip:
	//c/Programme/WinZip/WinZip32.exe *.mform tkmatrix.exe *.lan *.mat *.tcl *.dll \
	tkmatrix.hlp tkmatrix.cnt tcl8.0 tk8.0 \
	images/*.gif docus/*.html docus/*.pdf

install:
	install -d $(INSTALLDIR)/images
	install -m 0644 *.tcl *.so matrix *.mat *.mform *.lan *.tkm $(INSTALLDIR)
	install tkmatrix.tcl *.so matrix $(INSTALLDIR)
	install -m 0644 images/*.gif $(INSTALLDIR)/images
	ln -s $(INSTALLDIR)/tkmatrix.tcl $(LINKDIR)/tkmatrix
	ln -s $(INSTALLDIR)/matrix $(LINKDIR)/matrix
uninstall:
	rm -r /usr/lib/tkmatrix
	rm /usr/bin/tkmatrix /usr/bin/matrix














