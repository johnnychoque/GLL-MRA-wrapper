#!/bin/sh
echo "Starting MRA wrapper ..."
cd ../mra-wrapper
bin/wrapper_main configs/MRAWrapper_config.cfg &
echo "Starting GLL FE ..."
cd ..
bin/GLL_main configs/GLL_config.cfg &
