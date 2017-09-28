#!/bin/bash

set -e

for c in course*;do
    cd $c && make test && cd -
done

