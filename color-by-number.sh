#!/bin/bash

if [ "$#" -lt 3 ]; then
    echo "usage: color-by-number.sh <input-image> <output-name> <color> <options>"
    exit
fi

input=$1
output=$2
color=$3

convert $input -compress none input.ppm 

./bin/color_by_number $color < input.ppm > $output

rm input.ppm
