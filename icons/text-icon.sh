#! /bin/bash

# TEXTFONT=/usr/share/fonts/truetype/ttf-liberation/LiberationSans-Regular.ttf
TEXTFONT=/usr/share/fonts/truetype/msttcorefonts/Arial.ttf

convert -background black -fill white -size 64x32 -pointsize 14 -gravity center  \
	-font $TEXTFONT \
	label:"$1" icon-$2.png

