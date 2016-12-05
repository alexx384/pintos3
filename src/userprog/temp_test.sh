#!/bin/bash
cd build
pintos --qemu --disk=filesys.dsk -- -q run 'fsinfo'