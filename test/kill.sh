#!/bin/sh

ps ax|grep 'python simpleclient.py'|awk '{print $1}'|xargs kill -9
ps ax|grep 'python simpleclient2.py'|awk '{print $1}'|xargs kill -9
ps ax|grep 'python dbtest.py'|awk '{print $1}'|xargs kill -9