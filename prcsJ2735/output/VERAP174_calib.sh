#!/bin/sh

# Shell script for calibration

llc powerdet 1 2500 -42.961 0.0132 3344 -10.862 0.0036 2500 -42.961 0.0132 3344 -10.862 0.0036 38 0 4000000 
llc temp cfg 2 112 113 0.0685 0.2604 0.0685 0.2604 105 20 
llc reg w 1 8 0x100000 
llc reg w 1 9 0x200000 
llc reg w 0 40 -150 
llc reg w 0 41 -200 
llc chconfig -s -w CCH -c 178 -r a -a 2 
sleep 1
llc cal 
llc chconfig -x -w CCH -c 178 -r a