#!/bin/bash

if [ "$#" -lt 4 ]
then
    echo "usage: color-by-number.sh <input-image> <output-name> <color> <size> <options>"
    exit
fi

input=$1
output=$2
color=$3
size=$4

width=$(identify -format '%w' $input)
height=$(identify -format '%h' $input)

total=$((width * height)) 

if [ "$size" = "micro" ]
then
    divisor=250
    fontsize=7
elif [ "$size" = "tiny" ]
then
    divisor=1500
    fontsize=3
elif [ "$size" = "small" ]
then    
    divisor=10000
    fontsize=1
elif [ "$size" = "medium" ]
then
    divisor=20000
    fontsize=0.75
elif [ "$size" = "large" ]
then 
    divisor=30000
    fontsize=0.5
fi

if [ "$total" -lt 1000 ]
then
    fontsize=7
fi

if [ $total -gt $divisor ]
then
    quotient=$((total / $divisor))
    percent=$(awk -v x=$quotient 'BEGIN{print 100 / sqrt(x)}')
    convert $input -resize $percent% -compress none input.ppm
else
    convert $input -compress none input.ppm
fi

shift 4

./bin/color_by_number $color $fontsize $@ < input.ppm > $output

rm input.ppm

jgraph -P jgraph.txt | ps2pdf - output.pdf
