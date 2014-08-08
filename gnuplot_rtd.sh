#! /bin/bash

#plot '/tmp/rtd/rtd.data' binary skip = 72 format="%int16%int16" using 2 every ::100:0:200:0 with lines, '/tmp/rtd/rtd.data' binary skip = 72 format="%int16%int16" using 1 every ::100:0:200:0 with lines

NCHAN=${1:-2}

BYTESKIP=72
DEFCHANFORMAT="%uint16"
CHANFORMAT=${DEFCHANFORMAT}

OFFSET=000
NUMPOINTS=800

#WITHLINES=""
WITHLINES="with lines"


RTDFILE="/tmp/rtd/rtd.data"

#GNUPLOT_OPTSTR="set terminal wxt size 800,600"


for((i = 1; i < NCHAN; i++))
do CHANFORMAT+=${DEFCHANFORMAT}
done


PLOTSTR="plot '${RTDFILE}' binary skip=${BYTESKIP} format=\"${CHANFORMAT}\" using 1 every ::${OFFSET}:0:$((OFFSET + NUMPOINTS)):0 ${WITHLINES} title \"Channel 1\""


#PRTD="prtd"
#if [ $1 ]; then PRTD="prtd"; else PRTD="prtd_2ch"; fi
#echo "Using ${PRTD}..."

#/usr/src/rtdgui/rtdgui -g 640x480+0+0 /usr/src/rtdgui/hf2_config.input&
#/usr/src/rtdgui/rtdgui /usr/src/rtdgui/hf2_config.input&
#/usr/src/prtd/${PRTD} -c /usr/src/prtd/hf2_config.input -n ${NCHAN} -w

for((i = 1; i < NCHAN; i++))
do PLOTSTR+=", '${RTDFILE}' binary skip=${BYTESKIP} format=\"${CHANFORMAT}\" using $((i+1)) every ::${OFFSET}:0:$((OFFSET + NUMPOINTS)):0 ${WITHLINES} title \"Channel $((i+1))\"" 
done

echo ${PLOTSTR}

echo "Gnuplotting ${RTDFILE}.."
echo "(Gnuplot) Number of channels in ${RTDFILE}: ${NCHAN}"

#gnuplot -persist -e "${GNUPLOT_OPTSTR}" -e "${PLOTSTR}" rtd_loop.gnu
gnuplot -persist plot_opts.gnu -e "${PLOTSTR}" rtd_loop.gnu
