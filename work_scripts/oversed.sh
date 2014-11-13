#!/bin/bash

# shellscript, to change a line in a file

if (($# != 2))
  then
  echo "Wrong number of parameters, 2 expected"
  echo "Syntax same as in sed"
  exit 1
fi

file=$2
bk=${file}.bk
opt=$1

mv $file $bk
sed "$opt" $bk >$file