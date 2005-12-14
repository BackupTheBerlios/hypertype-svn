#!/bin/sh

VERSION="0.1"
RELEASE="1"

# Retrieve latest source
svn update && make -f Makefile.cvs
if [ $? != 0 ] ; then
   exit 
fi

# Build packages
make dist && cp -v hypertype-$VERSION.tar.gz /usr/src/packages/SOURCES && rpmbuild -ba hypertype.spec && cp -v /usr/src/packages/RPMS/i586/hypertype-$VERSION-$RELEASE.i586.rpm ./
if [ $? != 0 ] ; then
   exit 
fi

# Upload packages
echo "Enter ftp password"
read PASSWORD
ftp -u ftp://u37614859:$PASSWORD@ftp.joshnisly.com/blog/files/ hypertype-$VERSION.tar.gz hypertype-$VERSION-$RELEASE.i586.rpm

# Clean up
rm hypertype-$VERSION.tar.gz
rm hypertype-$VERSION-$RELEASE.i586.rpm

