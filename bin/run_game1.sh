#!/bin/sh

echo "running process game1......"

path=`pwd`

${path}/gameserver --game1 1>../log/log.game1 2>&1 &