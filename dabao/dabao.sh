#!/bin/bash
cd `dirname $0`
mkdir -p build
cd extract
find opt -type f -print0 |xargs -0 md5sum >md5sums
mv md5sums DEBIAN/md5sums
cd ..
dpkg-deb -b extract/ build/