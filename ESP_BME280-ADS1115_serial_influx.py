#!/usr/bin/python3
import serial
import time
import datetime
import os,sys
import argparse
import json
import re

from serial.tools import list_ports
from time import sleep

DEV_ID="RMNS-12"
baud=115200


#search for a ESP module
def ESP_search(ports):
    for i in range(len(ports)):
        print(str(i)+" "+ports[i],end="\t\t\t")
        dev = serial.Serial(ports[i], baud, timeout=1, xonxoff=True, bytesize=serial.EIGHTBITS, stopbits=serial.STOPBITS_ONE, parity=serial.PARITY_NONE)
        reply = query_value(cmdstr, replystr,dev)
        print(reply)
        if str(reply)[0:len(DEV_ID)] == DEV_ID :
            print(DEV_ID+" found")
            return ports[i]
    return -1


int_def=10
parser = argparse.ArgumentParser(description='BME280 monitoring tool (w/ ADC ADS1115)')
parser.add_argument("-o",help="one shot flag",action='store_true')
parser.add_argument("-i",help="interval (sec)",type=int,default=int_def)

args=parser.parse_args();
interval=args.i
oneshot=args.o

# csv output not inplemented at this moment...
config_output = 'both'
ocsv, odb = True, True
if config_output == 'db':
    ocsv, odb = False, True
if config_output == 'both':
    ocsv, odb = True, True

sql_dbname='BME280'
               
if odb:
    from influxdb import InfluxDBClient
    client = InfluxDBClient( host     = "10.37.0.216",port     = "8086",username = "",password = "",database= "mod1" )

print ("ESP_BME280 serial reader")
print ("device ID=",DEV_ID)

ports=list_ports.comports();

ser=serial.Serial()
ser.baud=baud
device=[info.device for info in ports if "CP2102N" in info.description]
print(device)
if len(device)==0:
    print("device not found")
    exit(0)
    
port=device[0]
while 1:
    ser=serial.Serial(port,baud)
    line=ser.readline().decode().replace('\n','')
    lines=line.split()
    dt=datetime.datetime.today()
    print(dt.strftime('%Y/%m/%d/%H:%M:%S'),"\t",line)

    if odb:
        json_data = [
            {
                'measurement' : 'bme',
                'fields' : {
                    'sensor' : lines[0],
                    'pre'    : float(lines[1]),
                    'temp'   : float(lines[2]),
                    'hum'    : float(lines[3]),
                    'adc0'    : float(lines[4]),
                    'adc1'    : float(lines[5]),
                    'adc2'    : float(lines[6]),
                    'adc3'    : float(lines[7])
                },
                'time' : datetime.datetime.utcnow(),
                'tags' : {
                    'device' : 'BME280',
                    'host'   : 'RMNS-12'
                }
            }
        ]   
        result = client.write_points(json_data)
        
    ser.close()
    if oneshot:
        break
    sleep(interval)


