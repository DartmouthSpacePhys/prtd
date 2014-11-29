#! /bin/bash

NCHAN=4

OPREFIX="test"

PRTD="prtd_8bit"

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

RTD_DIRF=${RTD_DIR}/${RTDF}

echo "Using ${PRTD} for \"${OPREFIX}\" display with rtd file ${RTD_DIRF}..."

/usr/src/rtdgui/rtdgui /usr/src/rtdgui/hf2_config.input ${OPREFIX}&
/usr/src/prtd/${PRTD} -c /usr/src/prtd/hf2_config.input -n ${NCHAN} -o ${OPREFIX}
