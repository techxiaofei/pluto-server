#!/bin/sh
nohup python -u simpleclient.py 2 > log2 2>&1 &
sleep 1
nohup python -u simpleclient.py 3 > log3 2>&1 &
sleep 1
nohup python -u simpleclient.py 22 > log22 2>&1 &
sleep 1
nohup python -u simpleclient.py 100 > log100 2>&1 &
 
