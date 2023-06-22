#/bin/bash

RED=$'\e[0;31m'
GREEN=$'\e[0;32m'
BLUE=$'\e[0;34m'
WHITE=$'\e[0;37m'

echo "${BLUE}Quick ping tester${WHITE}"
echo "${RED}ping 127.0.0.1 - USE CTRL + C after basic ping${WHITE}"
echo "${RED}A delay of +/- 30ms is tolerated on the reception of a packet.${WHITE}"
ping 127.0.0.1

echo "${RED}ping localhost >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping localhost

echo "${RED}ping -v 127.0.0.1 - USE CTRL + C after basic ping -v${WHITE}"
ping -v 127.0.0.1

echo "${RED}ping -v localhost - USE CTRL + C after basic ping -v${WHITE}"
ping -v localhost

echo "${RED}ping -?${WHITE}"

#Ping errors tests
#alarm tests?