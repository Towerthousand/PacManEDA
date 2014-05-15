#!/bin/bash


command_exists () {
    type "$1" &> /dev/null ;
}

UNAME=`uname`

if [  $UNAME = "Linux" ] ; then 
    if command_exists chromium-browser ; then
        chromium-browser --allow-file-access-from-files file://`pwd`/viewer.html?game=$1
    elif command_exists chrome-browser ; then
        chrome-browser --allow-file-access-from-files file://`pwd`/viewer.html?game=$1
    elif command_exists /opt/google/chrome/google-chrome ; then
        /opt/google/chrome/google-chrome --allow-file-access-from-files file://`pwd`/viewer.html?game=$1
    elif command_exists firefox ; then
        firefox file://`pwd`/viewer.html?game=$1
    else
        echo "Cannot find a browser"
    fi
elif [ $UNAME = "Darwin" ] ; then
    open -a "Google Chrome" --args --allow-file-access-from-files file://`pwd`/viewer.html?game=$1
else
    echo "Unrecognized system"
fi





