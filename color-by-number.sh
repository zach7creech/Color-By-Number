#!/bin/bash

if [ "$#" -lt 4 ]
then
    echo "usage: color-by-number.sh <input-image> <output-name> <color> <size> <landscape> <options>"
    exit
fi

input=$1
output=$2
color=$3
size=$4
landscape=$5

width=$(identify -format '%w' $input)
height=$(identify -format '%h' $input)

total=$((width * height)) 

if [ "$size" = "micro" ]
then
    divisor=250
    fontsize=10
elif [ "$size" = "tiny" ]
then
    divisor=1500
    fontsize=6
elif [ "$size" = "small" ]
then    
    divisor=10000
    fontsize=2.75
elif [ "$size" = "medium" ]
then
    divisor=20000
    fontsize=1.75
elif [ "$size" = "large" ]
then 
    divisor=30000
    fontsize=1.5
elif [ "$size" = "full" ]
then
    divisor=$total
    fontsize=0.5
fi

if [ "$total" -lt 1000 ]
then
    fontsize=14
fi

if [ $total -gt $divisor ]
then
    quotient=$((total / $divisor))
    percent=$(awk -v x=$quotient 'BEGIN{print 100 / sqrt(x)}')
    convert $input -auto-orient -resize $percent% -compress none input.ppm
else
    convert $input -auto-orient -compress none input.ppm
fi

shift 4

sed -i '/^#/d' input.ppm

./bin/color_by_number $color $fontsize $@ < input.ppm

if [ "$landscape" != "-L" ]
then
    landscape=""
fi

if [ "${output##*.}" = "pdf" ]
then  
    jgraph -P $landscape jgraph.txt | ps2pdf - $output
else
    jgraph -P $landscape jgraph.txt | ps2pdf - | convert -density 300 - -quality 100 $output
fi

rm input.ppm
rm jgraph.txt
