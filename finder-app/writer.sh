#!/bin/bash


if [ -z "$1" ]; then
	echo "First argument not provided"
	exit 1
elif [ -z "$2" ]; then
	echo "Second argument not provided"
	exit 1
fi

if [ ! -d "$pathDir" ]; then 
	mkdir -r 
fi

pathDirName=$(dirname $1)

fileName=$(basename $1)

wordName=$2

mkdir -p $pathDirName 

echo $wordName > $1