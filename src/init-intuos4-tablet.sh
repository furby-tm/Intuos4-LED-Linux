#! /bin/bash

DEVICE="Wacom Intuos4 6x9 pad"
LEDCMD=./intuos4-led-config

sudo modprobe -r wacom
sudo $LEDCMD --button 1 --icon Undo
sudo $LEDCMD --button 2 --icon Redo
sudo $LEDCMD --button 3 --icon ZoomIn
sudo $LEDCMD --button 4 --icon ZoomOut
sudo $LEDCMD --button 5 --icon Save
sudo $LEDCMD --button 6 --icon Alt
sudo $LEDCMD --button 7 --icon Ctrl
sudo $LEDCMD --button 8 --icon ArrowUp
sudo modprobe wacom

sleep 1

xsetwacom --set "$DEVICE" Button 2 "key ctrl y"
xsetwacom --set "$DEVICE" Button 3 "key ctrl z"
xsetwacom --set "$DEVICE" Button 8 "key plus"
xsetwacom --set "$DEVICE" Button 9 "key minus"
xsetwacom --set "$DEVICE" Button 10  "key ctrl s"
xsetwacom --set "$DEVICE" Button 11 "key alt"
xsetwacom --set "$DEVICE" Button 12 "key ctrl"
xsetwacom --set "$DEVICE" Button 13 "key shift"
