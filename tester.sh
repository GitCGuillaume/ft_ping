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

echo "${RED}ping 163.172.250.16 >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping 163.172.250.16
163.172.250.16

echo "${RED}ping 42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping 42.fr

echo "${RED}ping www.42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping www.42.fr

echo "${RED}ping -v 127.0.0.1 - USE CTRL + C after basic ping -v${WHITE}"
ping -v 127.0.0.1

echo "${RED}ping -v localhost - USE CTRL + C after basic ping -v${WHITE}"
ping -v localhost

echo "${RED}ping -v 42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping -v 42.fr

echo "${RED}ping -v www.42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping -v www.42.fr

echo "${RED}ping -?${WHITE}"
ping -? > help_ft_ping.txt
diff help.txt help_ft_ping.txt

#Ping errors tests
echo "${BLUE}Ping errors tests${WHITE}"
echo "${RED}ping 999.999.999.999 ${WHITE}"
ping 999.999.999.999

#ping host unreachable at home
echo "${RED}ping host unreachable${WHITE}"
ping 192.168.1.2

#alarm tests?
