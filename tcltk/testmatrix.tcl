load [file join . matrixtcl.so]

set mhandle [matrixtcl create 3 4 -rational]
puts "mhandle $mhandle handles [matrixtcl info]"
matrixtcl configure
puts "size [matrixtcl $mhandle info size] type [matrixtcl $mhandle info type] issolution [matrixtcl $mhandle info issolution]"
puts "tclform [matrixtcl $mhandle getform tcl]"
for {set x 0} {$x<3} {incr x} {
    for {set y 0} {$y<4} {incr y} {
        matrixtcl $mhandle setelem $x $y [list 3 [expr {$x+1}]]
    }
}
puts "tclform [matrixtcl $mhandle getform tcl]"
matrixtcl $mhandle algorithmus gauss
puts "issolution [matrixtcl $mhandle info issolution]"
puts "tclform [matrixtcl $mhandle getform tcl]"

matrixtcl destroy $mhandle

set mhandle [matrixtcl load inverse.mat]
puts "tclform $mhandle [matrixtcl $mhandle getform tcl]"
puts "inverse [matrixtcl $mhandle algorithmus inverse]"
puts "issolution [matrixtcl $mhandle info issolution]"
puts "tclform [matrixtcl $mhandle getform tcl]"
