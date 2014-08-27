#! /bin/bash

#plot '/tmp/rtd/rtd.data' binary skip = 72 format="%int16%int16" using 2 every ::100:0:200:0 with lines, '/tmp/rtd/rtd.data' binary skip = 72 format="%int16%int16" using 1 every ::100:0:200:0 with lines

NCHAN=4

OPREFIX="test"

RTD_DIR="/tmp/rtd"
RTDF="rtd.data"

if [ $1 ] 
then if [ $1 -le 4 ]
    then NCHAN=${1} 
    else echo "$0 <NUM CHANS (MAX 4)> <d|f|t (\"digitizer\"|\"fileplayer\"|\"tcp\")> "
	exit
    fi
fi

if [ $2 ];
then if [ `expr match ${2} "f"` -eq 1 ]; then 
	RTDF="rtd_${2}.data"; OPREFIX=${2}
    elif [ `expr match ${2} "t"` -eq 1 ]; then
	RTDF="rtd_${2}.data"; OPREFIX=${2}
    elif [ `expr match ${2} "d"` -eq 1 ]; then
	RTDF="rtd_${2}.data"; OPREFIX=${2}
    else echo "$0 <NUM CHANS (MAX 4)> <d|f|t (\"digitizer\"|\"fileplayer\"|\"tcp\")> "
	exit
    fi
fi

RTDFILE=${RTD_DIR}/${RTDF}

BYTESKIP=72
DEFCHANFORMAT="%uint16"
CHANFORMAT=${DEFCHANFORMAT}

OFFSET=000
NUMPOINTS=800

#WITHLINES=""
WITHLINES="with lines"

for((i = 1; i < NCHAN; i++))
do CHANFORMAT+=${DEFCHANFORMAT}
done

PLOTSTR="plot '${RTDFILE}' binary skip=${BYTESKIP} format=\"${CHANFORMAT}\" using 1 every ::${OFFSET}:0:$((OFFSET + NUMPOINTS)):0 ${WITHLINES} title \"Channel 1\""

for((i = 1; i < NCHAN; i++))
do PLOTSTR+=", '${RTDFILE}' binary skip=${BYTESKIP} format=\"${CHANFORMAT}\" using $((i+1)) every ::${OFFSET}:0:$((OFFSET + NUMPOINTS)):0 ${WITHLINES} title \"Channel $((i+1))\"" 
done

echo ${PLOTSTR}

echo "Gnuplotting ${RTDFILE}.."
echo "(Gnuplot) Number of channels in ${RTDFILE}: ${NCHAN}"

#gnuplot -persist -e "${GNUPLOT_OPTSTR}" -e "${PLOTSTR}" rtd_loop.gnu
gnuplot -persist plot_opts.gnu -e "${PLOTSTR}" rtd_loop.gnu
