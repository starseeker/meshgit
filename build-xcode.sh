#!/bin/bash -e

if [ ! -d obj ]; then
    mkdir obj
fi
if [ ! -d obj/xcode ]; then
    mkdir obj/xcode
fi
cd obj/xcode

cmake -Wdev -G "Xcode" ../..
sleep 1
open *.xcodeproj

cd ../..
