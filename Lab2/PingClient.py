from socket import *
from datetime import datetime
import time
import sys
import random

# Obtain the address
serverIP = str(sys.argv[1])
serverPort = int(sys.argv[2])

# random seq number
seq_init = random.randint(30000,40000)

# Create the list to store information
list_rtts = []


# Create an UDP socket

with socket(AF_INET, SOCK_DGRAM) as s:
    # send 15 pings
    for i in range(15):
        seq = seq_init + i
        timestamp = datetime.now().isoformat(sep= ' ')[:-7]
        message = "PING" + ' ' + str(seq) + ' ' + timestamp + '\r\n'
        time_start = datetime.now()

        #send the message 
        s.sendto(message.encode(),(serverIP,serverPort))

        try:
            # Set timeout to be 600ms
            s.settimeout(0.6)
            response, serverAddress = s.recvfrom(1024)
            time_end = datetime.now()

            # compute rtt in ms
            rtt = round((time_end - time_start).total_seconds() * 1000)

            list_rtts.append(rtt)
            # print the output
            print(f'ping to {serverIP}, seq = {seq}, rtt = {rtt} ms')

            # disable the timeout set
            s.settimeout(None)

        except timeout:
            print(f'ping to {serverIP}, seq = {seq}, rtt = time out')
    # Print the output summary
    print('\n')
    print(f'minimum = {min(list_rtts)} ms, maximum = {max(list_rtts)} ms, average = {round(float(sum(list_rtts)/len(list_rtts)),2)} ms')

