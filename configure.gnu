#!/bin/sh

par="--prefix=`kde-config --prefix`"

echo "*** Doing configure $* $par ..."

./configure $* $par