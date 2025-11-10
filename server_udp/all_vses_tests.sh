#!/bin/sh

./vses_test.sh solo
sleep 30
./vses_test.sh membomb
sleep 30
./vses_test.sh solo col
sleep 30
./vses_test.sh membomb col
sleep 30
./vses_test.sh solo bwreg
sleep 30
./vses_test.sh membomb bwreg
sleep 30
./vses_test.sh solo col bwreg
sleep 30
./vses_test.sh membomb col bwreg
