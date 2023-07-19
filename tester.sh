#/bin/bash

#note max TTL is 225 see info ping > TTL details

RED=$'\e[0;31m'
GREEN=$'\e[0;32m'
BLUE=$'\e[0;34m'
WHITE=$'\e[0;37m'

echo "${BLUE}Quick ping tester${WHITE}"
echo "${RED}ping 127.0.0.1 - USE CTRL + C after basic ping${WHITE}"
echo "${RED}A delay of +/- 30ms is tolerated on the reception of a packet.${WHITE}"
ping 127.0.0.1
echo "error code = $?"

echo "${RED}ping localhost >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping localhost
echo "error code = $?"

echo "${RED}ping 163.172.250.16 >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping 163.172.250.16
echo "error code = $?"

echo "${RED}ping 42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping 42.fr
echo "error code = $?"

echo "${RED}ping www.42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping www.42.fr
echo "error code = $?"

echo "${RED}ping -v 127.0.0.1 - USE CTRL + C after basic ping -v${WHITE}"
ping -v 127.0.0.1
echo "error code = $?"

echo "${RED}ping -v localhost - USE CTRL + C after basic ping -v${WHITE}"
ping -v localhost
echo "error code = $?"

echo "${RED}ping -v 42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping -v 42.fr
echo "error code = $?"

echo "${RED}ping -v www.42.fr >> FQDN (Fully Qualified Domain Name)${WHITE}"
ping -v www.42.fr
echo "error code = $?"

echo "${RED}ping -?${WHITE}"
ping -? > help_ft_ping.txt
diff help.txt help_ft_ping.txt
echo "error code = $?"

echo "${RED}ping -? localhost${WHITE}"
ping -? localhost
echo "error code = $?"

echo "${RED}ping -v -? localhost${WHITE}"
ping -v -? localhost
echo "error code = $?"

echo "${RED}ping -? -v localhost${WHITE}"
ping -? -v localhost
echo "error code = $?"

echo "${RED}ping -v localhost -?${WHITE}"
ping -v localhost -?
echo "error code = $?"

echo "${RED}ping -? localhost -v${WHITE}"
ping -v localhost -?
echo "error code = $?"

echo "${RED}ping localhost -?${WHITE}"
ping localhost -?
echo "error code = $?"

#Ping errors tests
echo "${BLUE}Ping errors tests${WHITE}"
echo "${RED}ping 999.999.999.999 ${WHITE}"
ping 999.999.999.999
echo "error code = $?"

echo "${RED}ping -v localhost localhost${WHITE}"
ping -v localost 456
echo "error code = $? (must be 1)"

echo "${RED}ping -v -v${WHITE}"
ping -v -v
echo "error code = $?"

echo "${RED}ping 172.1368${WHITE}"
ping 172.1368

echo "${RED}ping -v 192.168.1.56${WHITE}"
ping -v 192.168.1.56

#ping host unreachable at home
echo "${RED}ping host unreachable${WHITE}"
ping 192.168.1.2
echo "error code = $?"

#alarm tests?
