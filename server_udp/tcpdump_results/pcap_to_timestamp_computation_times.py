import argparse
import os
import sys
import numpy as np
import scapy
from scapy.all import *
from scapy.utils import RawPcapNgReader
from scapy.layers.l2 import Ether
from scapy.layers.inet import IP, TCP
from pprint import pprint

def process_pcap(folder, file_name):
    print('Opening {}...'.format(file_name))
    file =  open(folder +"timestamps.txt", mode='w')
    dspace = "192.168.1.1" # 200
    last_dst = "192.168.1.1" # 200
    last_dst_ts = 0
    last_rcv_ts = 0
    
    # Initialize a counter to limit the number of samples
    counter = 0
    max_samples = 25000
    
    for (pkt_data, pkt_metadata,) in RawPcapReader(file_name):
        # Stop after max_samples
        if counter >= max_samples:
            break
        # Timestamp in usec
        pkt_timestamp = (pkt_metadata.sec * 1000000) + (pkt_metadata.usec)
        ether_pkt = Ether(pkt_data)

        if IP in ether_pkt:
            ip_pkt = ether_pkt[IP]
            dst = ip_pkt.dst
            src = ip_pkt.src
            if dst == dspace: # and dst != last_dst:
                last_dst = dst
                last_dst_ts = pkt_timestamp
                diff = last_dst_ts - last_rcv_ts 
                file.write(str(diff) + "\n")
                counter += 1
            elif src == dspace: # and dst != last_dst:
                last_dst = dst
                last_rcv_ts = pkt_timestamp
                #last_dst = dst       


if __name__ == '__main__':
    process_pcap(sys.argv[1],sys.argv[2])
    sys.exit(0)


