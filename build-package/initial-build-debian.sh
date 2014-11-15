#!/usr/bin/env bash

## readme - http://wiki.debian.org/IntroDebianPackaging

cd ../; ./clear; cd build-package;
set -e
set -x
export EDITOR="/bin/echo"

name="php5-cm"
version="1.0-1"
rversion="1.0"
src="phpcm"

cd ../..

rm -rf ./$name
cp -R ./$src ./$name
cd ./$name; cmake .;
mkdir debian
dch --create -v $version --package $name

echo -n 8 > debian/compat

echo "Source: $name
Maintainer: Perfect Solutions Builder <lynx.programmers.group.piroman@gmail.com>
Section: misc
Priority: optional
Standards-Version: 3.9.2
Build-Depends: debhelper (>= 8)

Package: $name
Architecture: amd64" > debian/control
echo 'Depends: ${shlibs:Depends}, ${misc:Depends}, libmemcached11 (>= 1.0.18), phpapi-20131226' >> debian/control
echo "Description: PHP Cache Manager.
 <PHP Cache Manager for highload projects>" >> debian/control


echo '#!/bin/sh -e' > debian/postinst
chmod +x debian/postinst
echo '' >> debian/postinst
echo 'ldconfig' >> debian/postinst

echo '#!/usr/bin/make -f' > debian/rules
echo '' >> debian/rules
echo "%:" >> debian/rules
echo '	dh $@' >> debian/rules


echo "Copyright 2014 Mikhail Konyukhov <lynx.programmers.group.piroman@gmail.com>" > debian/copyright
cat ../phpcm/LICENCE.md >>  debian/copyright


mkdir debian/source/
echo '3.0 (quilt)' > debian/source/format

cd ..
tar czvf ${name}_${rversion}.orig.tar.gz $name
cd $name

debuild -us -uc

#cd ..
#cp $nameWithVersion-1_amd64.deb /var/www/dists/wheezy/main/binary/
#cd /var/www/
#dpkg-scanpackages dists/ | gzip -9c > dists/Packages.gz
