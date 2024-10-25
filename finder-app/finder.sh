#!/bin/bash


if [ -z "$1" ]; then
	echo "First argument not provided"
	exit 1
elif [ -z "$2" ]; then
	echo "Second argument not provided"
	exit 1
fi



pathDir=$1

wordName=$2

if [ ! -d "$pathDir" ]; then 
	echo "Path not exists"
	exit 1
fi


numberLines=$(grep -R $wordName $pathDir | wc -l)

numberFiles=$(grep -R -c $wordName $pathDir | wc -l)



echo "The number of files are $numberFiles and the number of matching lines are $numberLines"