#! /bin/bash

#LEFT-HANDED="--lefthanded"

modprobe -r wacom

./intuos4-led-config $LEFTHANDED -button 1 --icon Undo
./intuos4-led-config $LEFTHANDED -button 2 --icon Redo
./intuos4-led-config $LEFTHANDED -button 3 --icon ZoomIn
./intuos4-led-config $LEFTHANDED -button 4 --icon ZoomOut
./intuos4-led-config $LEFTHANDED -button 5 --icon Save
./intuos4-led-config $LEFTHANDED -button 6 --icon Alt
./intuos4-led-config $LEFTHANDED -button 7 --icon Ctrl
./intuos4-led-config $LEFTHANDED -button 8 --icon ArrowUp

modprobe wacom

