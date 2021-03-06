#!/bin/bash -x
##
## BAW: http://wiki.cs.unm.edu/ssl/doku.php/kurt:bochs
## mkldsym: based on linux mksysmap, we just strip a field
## 
## usage:
## 
## mkldsym <ELF EXE> <output sym file>
nm -n $1 | grep -v '\( [aUw] \)\|\(__crc_\)\|\( \$[adt]\)' | awk '{print $1, $3}' > $2
