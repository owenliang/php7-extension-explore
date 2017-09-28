#!/bin/bash

# 只编译不安装, 若需安装某个course需自己执行make install

set -e

for c in course*;do
        cd $c && make && make && cd -
done

sudo echo "extension=myext.so" >> `php --ini|grep Loaded| awk '{print $NF}'`
