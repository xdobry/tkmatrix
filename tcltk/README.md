1. What is tkmatrix?
2. Compile and Install
3. Documentation
4. tkmatrix Homepage, Author and License

== 1. What is tkmatrix? ==

tkmatrix is an education mathematical program to manipulate matrices.
Matrices are part of linear algebra. It was developed for students to learn 
algoritms and test own calculation. Therefore tkmatrix can give
not even a solution but all algorithm steps as a protocol. It can calculate
with floatnumbers (and multiple precision float) and fractions. 
Solutions and protocols can be exported to ascii, html and latex files.
(other files are possible). The math-part of tkmatrix is
programmed in C++ as tcl-library and graphical interface in Tk/Tcl. 
The distribution includes the program matrix too. 
It is pure shellprogram tkmatrix without GUI.

== Prerequisits ==

install compiler

    apt get intall g++

install tcl tk

    apt-get install tcl tk tcl-dev tk-dev

install gnu multiple precision library

    apt-get install -y libgmp-dev

== 2. Compile and Install ==

-compile configuration
old linux STL Library use string::erase not string::remove. 
If your Library (RedHat<5.1) know string::remove you must edit
makefile and uncommnet one line

-to compile change in tkmatrix-0.6 directory and do
   make

== Installation ==

You can use the program without full install (when you have no
root). To run start tkmatrix.tcl. The program expect wish in /usr/lib/wish

To full install do

     make install

it copy all necessary files in /usr/lib/tkmatrix and make 2
symbolic links
in /usr/bin. Documentation in dir docu will be not copied.

== 3 Other Documentation ==

Full documentation is available only in german language
docus/matrix.html
english short-version is docus/tkmatrix_en.html


  * :mathe_en.pdf  Acrobat Reader File mathematical short documentation in English
  * :mathe_en.tex  Latex File
  * :mathe1.pdf mathe1.ps full mathematical documentation in German

== 4 tkmatrix Homepage, Author and License ==

English tkmatrix Homepage
http://www.xdobry.de/tkmatrix/tkmatix_en.html
German Pages
http://www.xdobry.de/tkmatrix

tkmatrix was written by Artur Trzewik

tkmatrix is free software. It was created for education and test
purposes. 
This program is distributed in the hope that it will be useful,
but 
WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Email: mail@xdobry.de
