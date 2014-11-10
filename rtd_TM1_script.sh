#! /bin/bash

NCHAN=4

OPREFIX="test"

PRTD="prtd"

RTD_DIR="/tmp/rtd"
RTDF="rtd.data"

RTD_CONFIG_DIR="/usr/src/rtdgui"

RTD_CONFIG_FILE="usr/src/rtdgui/hf2_config.input"

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

#Handle specialized display
if [ $3 ];
then 
    RTD_CONFIG_FILE="${RTD_CONFIG_DIR}/Andoya_configs/${3}-hf2_config.input"
fi

echo "Using ${PRTD} for \"${OPREFIX}\" display with rtd file ${RTD_DIRF}..."

/usr/src/rtdgui/rtdgui_TM1_HF ${RTD_CONFIG_FILE} ${OPREFIX}&
/usr/src/prtd/${PRTD} -c /usr/src/prtd/hf2_config.input -n ${NCHAN} -o ${OPREFIX}
