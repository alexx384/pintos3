#!/bin/bash
cd build
rm -vf filesys.dsk
pintos-mkdisk filesys.dsk --filesys-size=2
pintos --qemu --disk=filesys.dsk -- -f -q
pintos --qemu --disk=filesys.dsk -p ../../examples/memstat -a memstat -- -q
pintos --qemu --disk=filesys.dsk -- -q run 'memstat'