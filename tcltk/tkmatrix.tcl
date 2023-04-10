#!/usr/bin/wish -f
# to proper work of this programm
# set this varibale "progdir" to path of the dir whith this programm 
# !! whithout ending / and not including ~
# and variable "helpdir" to the dir with html help files
# set progdir dummy

package require Tk

set tbimageinited 0

set sname [info script]
if {$sname==""} {
  # Run interactive for develop purposes
  set progdir /home/artur/tkmatrix
} else {
  file lstat $sname stats
  # follow sym links
  if {$stats(type)=="link"} {
	 set sname [file readlink $sname]
  }
  set progdir [file dirname $sname]
}

encoding system utf-8
source [file join $progdir tkmatrix.xotcl]

startTkmatrix $progdir {} $progdir
