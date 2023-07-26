from scapy.all import *
from netfilterqueue import NetfilterQueue

def test(packet):
    print("display packet")
    print(packet)
    pkt = IP(packet.get_payload())
    if str(pkt.getlayer(ICMP).type) == "8":
        print(pkt[ICMP].type)
        print(pkt[ICMP].code)
        #icmp = Ether(dst=pkt[Ether].src)/IP(dst=pkt[IP].src, src=pkt[IP].dst)/ICMP(type=3, code=0)/Raw()
        icmp = pkt.copy()
        icmp[IP].src = pkt[IP].dst
        icmp[IP].dst = pkt[IP].src
        #icmp[ICMP].src = pkt[ICMP].dst
        #icmp[ICMP].dst = pkt[ICMP].src
        icmp[ICMP].type = 0
        icmp[ICMP].code = 0

        #cpy = icmp[Ether].src
        #icmp[Ether].src = icmp[Ether].dst
        #icmp[Ether].dst = cpy
        #ip = IP()
        #ip.src = pkt[IP].dst
        #ip.dst = pkt[IP].src

        #icmp = ICMP()
        #icmp.type = 0
        #icmp.code = 0
        #icmp.id = pkt[ICMP].id
        #icmp.seq = pkt[ICMP].seq
        #icmp.payload = pkt[ICMP].payload
       # icmp.payload = 
        #cpy2 = icmp[IP].src
        #icmp[IP].src = icmp[IP].dst
        #icmp[IP].dst = cpy2

        #icmp.type = 0
        #icmp.code = 0
        send(icmp, iface="lo")
        #if (resp == None):
        #    print("down")
        #else:
        #    print("ok")

nfqueue = NetfilterQueue()
nfqueue.bind(1, test)
try:
    nfqueue.run()
except :
    print("error")
nfqueue.unbind()
#pkts = sniff(iface="lo", filter="icmp", count=1, prn=test)
#packets.summary()