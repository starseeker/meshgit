#!/bin/bash -e

if [ ! -d obj ]; then
    mkdir obj
fi
if [ ! -d obj/mk ]; then
    mkdir obj/mk
fi
cd obj/mk

echo "Uncomment the line that points to GCC and G++ compilers ver 4.7 or 4.8" && exit 1
#cmake -Wdev -G "Unix Makefiles" ../..
#CC="/opt/local/bin/gcc-mp-4.7" CXX="/opt/local/bin/g++-mp-4.7" cmake -Wdev -G "Unix Makefiles" ../..
#CC="/usr/bin/gcc-4.7" CXX="/usr/bin/g++-4.7" cmake -Wdev -G "Unix Makefiles" ../..

set +e
make all
RET=$?
set -e

cd ../..

exit ${RET}
