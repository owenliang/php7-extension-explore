#!/bin/bash

set -e

for c in course*;do
	cd $c 
	make
	sudo make install
	make test
	cd -
done

