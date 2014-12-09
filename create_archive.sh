#! /bin/bash

TEMP=libvata

cp -R api/ $TEMP/
mkdir $TEMP/build
cp -R cli/ $TEMP/
cp -R cmake/ $TEMP/
cp -R CMakeLists.txt $TEMP/
cp -R COPYING $TEMP/
cp -R Makefile $TEMP/
cp -R README $TEMP/
cp -R include $TEMP/
cp -R src $TEMP/
cp -R unit_tests $TEMP/
cp -r documentation.pdf $TEMP
mkdir $TEMP/tests
cp -R tests/*sh tests/*awk tests/*in  tests/aut_timbuk_smaller tests/fa_timbuk_armc $TEMP"/tests/"

tar -czf vata.tar.gz $TEMP

rm -rf $TEMP
