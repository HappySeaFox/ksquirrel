#!/bin/sh

par="--without-arts --prefix=`kde-config --prefix`"

echo "*** Doing configure $* $par ..."

./configure $* $par