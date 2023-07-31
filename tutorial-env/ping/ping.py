from scapy.all import *
from netfilterqueue import NetfilterQueue
import copy

def test(packet):
    print("display packet")
    print(packet)
    pkt = IP(packet.get_payload())
    if str(pkt.getlayer(ICMP).type) == "8":
        print(pkt[ICMP].type)
        print(pkt[ICMP].code)
        #test = pkt.copy()
        #cpy = pkt[IP].src
        #pkt[IP].src = pkt[IP].dst
        #pkt[IP].dst = cpy
        #pkt[ICMP].type = 5
        #pkt[ICMP].gw = "192.168.57.0"
        #pkt[ICMP].code = 0
        #del pkt[ICMP].chksum
        
        ###RAW###
        raw = copy.deepcopy(pkt.load)
        cpy_src = copy.deepcopy(pkt[IP].src)
        cpy_dst = copy.deepcopy(pkt[IP].dst)
        ###IP ORIGINAL
        ip_original = IP()
        ip_original.id = copy.deepcopy(pkt[IP].id)
        ip_original.flags = copy.deepcopy(pkt[IP].flags)
        ip_original.frag = copy.deepcopy(pkt[IP].frag)
        ip_original.proto = copy.deepcopy(pkt[IP].proto)
        ip_original.ttl = copy.deepcopy(pkt[IP].ttl)
        ip_original.len = copy.deepcopy(pkt[IP].len)
        ip_original.tos = copy.deepcopy(pkt[IP].tos)
        ip_original.src = cpy_src
        ip_original.dst = cpy_dst
        ip_original.chksum = copy.deepcopy(pkt[IP].chksum)
        #####IP#####
        ip = IP()
        #ip.tos = pkt[IP].tos
        #ip.len = pkt[IP].len
        ip.id = pkt[IP].id
        ip.flags = pkt[IP].flags
        ip.proto = pkt[IP].proto
        ip.ttl = pkt[IP].ttl
        
        ip.src = cpy_dst
        ip.dst = cpy_src
        
        ###ICMP###
        icmp = ICMP()
        icmp.type=14
        icmp.code=0
        #icmp.id = pkt[ICMP].id
        #icmp.seq = pkt[ICMP].seq
        #icmp.gw = "192.168.57.0"
       

        #pkt[ICMP].type=5
        #pkt.show2()
        icmp.show2()
        udp=UDP()
        send(ip/icmp/ip_original/udp, iface="lo")

nfqueue = NetfilterQueue()
nfqueue.bind(1, test)
try:
    nfqueue.run()
except :
    print("error")
nfqueue.unbind()
#pkts = sniff(iface="lo", filter="icmp", count=1, prn=test)
#packets.summary()