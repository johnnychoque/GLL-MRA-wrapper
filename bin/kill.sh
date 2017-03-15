#!/bin/sh
echo "Stopping GLL ..."
killall -9 GLL_main
echo "Stopping MRA wrapper ..."
killall -9 wrapper_main
