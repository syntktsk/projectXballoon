# coding: UTF-8
"""
This program is a sample program that controls Graphtec GL series by Python.
The connection supports USB and TCP/IP(LAN) connection.
(* USB connection is Windows OS only)
Refer to the SDK specifications for the commands that can be entered. It does not support binary commands.

System requirements (software)
    Python 3.8

Required packages
    pythonnet (for USB connection)
"""

import os
import datetime
from tcp import Tcp
from usb import Usb
from filetrans import File_trans

#Timeout(1sec)
Timeout_default = 1

def main():
    #Instantiation of the LAN communication class
    tcp = Tcp(Timeout_default)
    #usb = Usb(Timeout_default)

    print("USB(0) or TCP/IP(1)?")
    IF = int(input())
    if IF == 0:
        if_usb()
    else:
        if_tcp()

# USB Connection
def if_usb():

    print("USB ID of the device?")
    ID = int(input())

    usb = Usb(Timeout_default)
    if not usb.open(ID):
        print("Connection error")
        return

    print("Mode? 0=Ascii/1=Binary/2=FileTrans/3=FileList")
    mode = int(input())
    if mode == 0:
        ascii_mode(usb)
    elif mode == 1:
        binary_mode(usb)
    elif mode == 2:
        file_trans = File_trans(usb)
        path = __file__
        dir = os.path.dirname(path)
        dt_now = datetime.datetime.now()
        tm = dt_now.strftime('%Y%m%d-%H%M%S')
        fn = f'{tm}.CSV'
        file = os.path.join(dir, fn)
        file_trans.transfar_file('\\MEM\\210730\\210730-162427.CSV', file)
    elif mode == 3:
        file_list(usb)

    usb.close()

# TCP/IP Connection
def if_tcp():

    print("IP address of the device?")
    IP = input()
    print("Port? (default:8023)")
    port = int(input())
    tcp = Tcp(Timeout_default)
    if not tcp.open(IP, port):
        print("Connection error")
        return

    print("Mode? 0=Ascii/1=Binary")
    mode = int(input())
    if mode == 0:
        ascii_mode(tcp)
    else:
        binary_mode(tcp)
    
    tcp.close()

# Ascii mode
def ascii_mode(obj):
    #Send and receive commands
    while True:
        print("Enter the command (Exit with no input) Ex.:MEAS:OUTP:ONECSV?")
        command = input()
        #Exit if no input
        if command == "":
            break
        #If the command contains "?"
        if "?" in command :
            msgBuf = obj.send_read_command(command, Timeout_default)
            print(msgBuf) 
        #Send only
        else:
            obj.send_command(command)

# Binary mode
def binary_mode(obj):
    #Send and receive commands
    while True:
        print("Enter the command (Exit with no input) Ex. :MEAS:OUTP:ONE?")
        command = input()
        #Exit if no input
        if command == "":
            break
        
        # Send command
        obj.send_command(command)

        #If the command contains "?"
        if "?" in command :
            msgbuf = bytes(obj.read_binary(8, Timeout_default))    # Read header
            len = int(msgbuf[2:8].decode())          # Read binary byte length
            print("Len " + str(len))
            msgbuf2 = bytes(obj.read_binary(len, Timeout_default)) # Read data
            print(msgbuf2, type(msgbuf2))
        #Send only

# File List
def file_list(obj):
    obj.send_command(":FILE:LIST:FORM SHORT")
    obj.send_command(":FILE:CD \"\\MEM\"")     # Set path
    # Exsample: SD1/210826/AAAA.CSV
    # obj.send_command(":FILE:CD \"\\SD1\\210826\"")   # Set path
    print(obj.send_read_command(":FILE:LIST?"))


if __name__ == '__main__':
  main()
