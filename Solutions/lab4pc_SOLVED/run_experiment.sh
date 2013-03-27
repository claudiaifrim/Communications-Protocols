#!/bin/bash

# Lab4: LOSS=0, CORRUPT=0
# Lab4: allowed values for SPEED: 0 - 20 Mb/s, DELAY: 0 - 1000 ms

SPEED=10
DELAY=10
LOSS=0
CORRUPT=0
BDP=$((SPEED * DELAY))

killall link 2> /dev/null
killall recv 2> /dev/null
killall send 2> /dev/null

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT &> /dev/null &
sleep 1
./recv &
sleep 1

./send $BDP
