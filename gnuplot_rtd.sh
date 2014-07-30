#! /bin/bash

#NCHAN=${1:-4}

#PRTD="prtd"
#if [ $1 ]; then PRTD="prtd"; else PRTD="prtd_2ch"; fi
#echo "Using ${PRTD}..."

#/usr/src/rtdgui/rtdgui -g 640x480+0+0 /usr/src/rtdgui/hf2_config.input&
#/usr/src/rtdgui/rtdgui /usr/src/rtdgui/hf2_config.input&
#/usr/src/prtd/${PRTD} -c /usr/src/prtd/hf2_config.input -n ${NCHAN} -w


gnuplot -persist -e "plot '/tmp/rtd/rtd.data' binary format=\"%int16\" using 1 every::0:0:200:0" gnuplot_rtd.loop
