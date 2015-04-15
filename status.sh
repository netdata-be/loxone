#!/bin/bash

USER=admin
PASSWORD="$2"
HOST="$1"


date=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/date | ./jshon -e LL -e value -u)
time=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/time | ./jshon -e LL -e value -u)
check=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/check | ./jshon -e LL -e value -u)
echo "########## Miniserver General info ############"
echo "#  Curent date  : ${date}"
echo "#  Current Time : ${time}"
echo "#  Current user : ${check}"
echo "###############################################"
echo ""
echo ""
cpu=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/lastcpu  | ./jshon -e LL -e value -u)
heap=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/heap | ./jshon -e LL -e value -u)
numtask=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/numtasks | ./jshon -e LL -e value -u)
interupts=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/ints | ./jshon -e LL -e value -u)
cominterupts=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/comints | ./jshon -e LL -e value -u)
contextswitches=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/contextswitches | ./jshon -e LL -e value -u)
contextswitchesi=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/contextswitchesi | ./jshon -e LL -e value -u)
echo "########## System Status ######################"
echo "#  Cpu status                  : ${cpu}"
echo "#  Heap usage                  : ${heap}"
echo "#  System interupts            : ${interupts}"
echo "#  Communication  interupts    : ${cominterupts}"
echo "#  Number of tasks             : ${numtask}"
echo "#  Context switching           : ${contextswitches}"
echo "#  Context switching interupts : ${contextswitchesi}"
echo "###############################################"
echo ""
echo ""
sps_state=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sps/state | ./jshon -e LL -e value -u)
sps_status=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sps/status | ./jshon -e LL -e value -u)
case $sps_state in
  0) status="No Status";;
  1) status="booting";;
  2) status="Program is loaded";;
  3) status="PLC has started";;
  4) status="Loxone link has started";;
  5) status="Running";;
  6) status="Change";;
  7) status="Error";;
  8) status="Update in progress";;
esac
echo "########## PLC Status #########################"
echo "#  PLC state       : ${status}"
echo "#  PLC frequency   : ${sps_status}"
echo "###############################################"
echo ""
echo ""
canbus_send=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/bus/packetssent | ./jshon -e LL -e value -u)
canbus_received=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/bus/packetsreceived | ./jshon -e LL -e value -u)
canbus_errors=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/bus/receiveerrors | ./jshon -e LL -e value -u)
canbus_frameerrors=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/bus/frameerrors | ./jshon -e LL -e value -u)
canbus_overrun=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/bus/overruns | ./jshon -e LL -e value -u)
canbus_parityerrors=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/bus/parityerrors | ./jshon -e LL -e value -u)
echo "########## CAN bus Status #####################"
echo "#  Packets Send       : ${canbus_send}"
echo "#  Packets Received   : ${canbus_received}"
echo "#  Packets Error      : ${canbus_errors}"
echo "#  Frame Errors       : ${canbus_frameerrors}"
echo "#  Frame Overruns     : ${canbus_overrun}"
echo "#  Parity Errors      : ${canbus_parityerrors}"
echo "###############################################"
echo ""
echo ""
mac=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/mac | ./jshon -e LL -e value -u)
dhcp=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/dhcp | ./jshon -e LL -e value -u)
ip=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/ip | ./jshon -e LL -e value -u)
mask=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/mask | ./jshon -e LL -e value -u)
gw=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/gateway | ./jshon -e LL -e value -u)
dns1=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/dns1 | ./jshon -e LL -e value -u)
dns2=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/dns2 | ./jshon -e LL -e value -u)
ntp=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/cfg/ntp | ./jshon -e LL -e value -u)
echo "########## Network Config  ####################"
echo "#  MAC address          : ${mac}"
echo "#  DHCP Setting         : ${dhcp}"
echo "#  IP Address           : ${ip}"
echo "#  Netmask              : ${mask}"
echo "#  Gateway              : ${gw}"
echo "#  Primary DNS Server   : ${dns1}"
echo "#  Secondary DNS Server : ${dns2}"
echo "#  NTP Server           : ${ntp}"
echo "###############################################"
echo ""
echo ""
lan_txp=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/txp | ./jshon -e LL -e value -u)
lan_txe=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/txe | ./jshon -e LL -e value -u)
lan_txc=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/txc | ./jshon -e LL -e value -u)
lan_exh=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/exh | ./jshon -e LL -e value -u)
lan_txu=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/txu | ./jshon -e LL -e value -u)
lan_rxp=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/rxp | ./jshon -e LL -e value -u)
lan_eof=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/eof | ./jshon -e LL -e value -u)
lan_rxo=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/rxo | ./jshon -e LL -e value -u)
lan_nob=$(curl -s -u ${USER}:${PASSWORD} $HOST/jdev/lan/nob | ./jshon -e LL -e value -u)
echo "########## LAN interface Status ###############"
echo "#  Packets send            : ${lan_txp}"
echo "#  Packets send errors     : ${lan_txe}"
echo "#  Packets send Collisions : ${lan_txc}"
echo "#  Buffer errors           : ${lan_exh}"
echo "#  Buffer under-run errors : ${lan_txu}"
echo "#  Packets received        : ${lan_rxp}"
echo "#  EOF Errors              : ${lan_eof}"
echo "#  Recieve overflow errors : ${lan_rxo}"
echo "#  No receive buffer errors: ${lan_nob}"
echo "###############################################"
echo ""
echo ""
echo "########## 1-Wire Status ######################"
echo -en ' ' ; curl -s -u ${USER}:${PASSWORD} $HOST/jdev/sys/ExtStatistics/05D828E6  | tr -d "" | tr -d '\n' | ./jshon -e LL -e value -u | tr  ';' "\n"
echo "###############################################"
echo ""
echo ""
echo "########## Global Status ######################"
curl -s -u ${USER}:${PASSWORD} $HOST/data/status
echo "###############################################"
