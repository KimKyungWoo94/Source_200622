#!/bin/sh
RTCM_CH=180 #채널번호 wave3번에 180번(현재로선 고정) wave0번(V2I 178번 채널 단말기-> 기지국 영상스트리밍) wave 1번2번 (I2I통신용 172,184채널) # 
PSID=17160 #RTCM PSID#
NETIFINDEX=3 #wsm송신인퍼테이스 설정 0~3 #
DATARATE=12 #데이타레이트#
TXPOWER=20 #송신파워#
INTERVAL=1000000 #주기 ms단위 1초#
UDP_IP=192.168.200.1 #기지국 1번 IP #
UDP_PORT=20001 #UDP 포트번호 only Client #
GPSD_PORT=99 #GPSD포트번호# 
INTERFACE_NAME=wave-ipv6-3  #웨이브통신 RF포트번호 설정#
OP=$1 #스크립트 실행할때 OpType Obu냐 RSU냐 #
SOCKET=$2 #스크립트 실행할때 Socket Type Server냐 Client냐 #
DEBUG=1 #DEBUG모드 1이면 디버깅 모드 0이면 안함 #
running=""
PIDLIST=./pidlist

usage() 
{
	echo ""
	echo "Usage:"
	echo "  ./runObu.sh <OP> <SOCK>"
	echo ""
	echo "Parameters:"
	echo "  OP   : obu or rsu "
	echo "  SOCK : server or client(only rsu)"
	echo ""
	exit
}

do_checkRunning()
{
    PID=`ps -ef | grep $1 | grep -v 'grep' | awk '{print $2}'` 
    if [ -z $PID ];then
        echo  $1 "is not running"
        running="no"
    else
        echo $1 "is already running"
        running="yes"
    fi
}

do_IPSET()
{
    ### Set Channel ###
    ./chan config --if $NETIFINDEX --ts0 $RTCM_CH --ts1 $RTCM_CH --rate $DATARATE --power $TXPOWER

    ### Set IP ###
    ifconfig $INTERFACE_NAME $UDP_IP
}

do_obu()
{
    ### gpsd ###
    do_checkRunning gpsd   
    if [ $running == "no" ];then
        ./gpsd /dev/ttyACM0 -n -S 99  #-n 연결 기다리지않음 -S 포트설정 #
    fi

    ### timeSync ###
    do_checkRunning timeSync   
    if [ $running == "no" ];then
        ./timeSync &
    fi

    ### prcsWSM ###
    do_checkRunning prcsWSM   
    if [ $running == "no" ];then
        ./prcsWSM -a rx -p $PSID -x $NETIFINDEX -n $RTCM_CH -r $DATARATE -b $DEBUG & #-a 동작방식 tx rx trx -p PSID설정 -x wsm송신인터페이스설정 -n wsm송신채널설정 -r datarate설정 -b 디버깅메시지출력 #
        echo PRCS_WSM_PID=$! >> ${PIDLIST}
    fi
    
    ### prcsJ2735 ###
    do_checkRunning prcsJ2735   
    if [ $running == "no" ];then
        ./prcsJ2735 --op=rx --port=$GPSD_PORT --debug=$DEBUG &
        echo PRCS_J2735_PID=$! >> ${PIDLIST}
    fi
}

do_rsu_server()
{
    ### gpsd ###
    do_checkRunning gpsd   
    if [ $running == "no" ];then
        ./gpsd /dev/ttyACM0 -n -S 99
    fi

    ### timeSync ###
    do_checkRunning timeSync   
    if [ $running == "no" ];then
        ./timeSync &
    fi

    ### Run prcsWSM ###
    do_checkRunning prcsWSM   
    if [ $running == "no" ];thenNETIFINDEX
        ./prcsWSM -a tx -p $PSID -x $NETIFINDEX -n $RTCM_CH -r $DATARATE -b $DEBUG &   #-a 동작방식 -p psid설정(17160) -x wsm 송신인터페이스설정 -n wsm송신채널설정 -r datarate설정 -b디버깅레벨 #
        echo PRCS_WSM_PID=$! >> ${PIDLIST}
    fi

    ### Run prcsJ2735 ###
    do_checkRunning prcsJ2735   
    if [ $running == "no" ];then
        ./prcsJ2735 --op=tx --sock=server --msg=rtcm --port=$GPSD_PORT --interval=$INTERVAL --debug=$DEBUG &
        echo PRCS_J2735_PID=$! >> ${PIDLIST}
    fi
}

do_rsu_client()
{
    ### gpsd ###
    do_checkRunning gpsd   
    if [ $running == "no" ];then
        ./gpsd /dev/ttyACM0 -n -S 99
    fi

    ### timeSync ###
    do_checkRunning timeSync   
    if [ $running == "no" ];then
        ./timeSync &
            fi

    ### Run prcsWSM ###
    do_checkRunning prcsWSM   
    if [ $running == "no" ];then
        ./prcsWSM -a tx -p $PSID -x $NETIFINDEX -n $RTCM_CH -r $DATARATE -b $DEBUG &
        echo PRCS_WSM_PID=$! >> ${PIDLIST}
    fi

    ### Run prcsJ2735 ###
    do_checkRunning prcsJ2735   
    if [ $running == "no" ];then
        ./prcsJ2735 --op=tx --sock=client --msg=rtcm --port=$GPSD_PORT --interval=$INTERVAL --udpPort=$UDP_PORT --debug=$DEBUG &
        echo PRCS_J2735_PID=$! >> ${PIDLIST}
    fi
}

### run ###
if [ "$OP" == "obu" ]; then
    do_obu
elif [ "$OP" == "rsu" ]; then
    if [ "$SOCKET" == "server" ]; then
        echo 'Set Server'
        do_IPSET
        do_rsu_server
    elif [ "$SOCKET" == "client" ]; then
        echo 'Set Client'
        do_IPSET
        do_rsu_client
    else
        usage
    fi
else
    usage
fi

