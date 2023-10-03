#!/usr/bin/python3
import serial
import time
import datetime
import os,sys
import argparse
import json
import re
#import pyvisa as visa

from serial.tools import list_ports
from time import sleep

DEV_ID="CP2102N"
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
#        else:
#            print("")
    return -1


#ID=0x7f36e066ab38
int_def=10
parser = argparse.ArgumentParser(description='BME280 monitoring tool')
parser.add_argument("-o",help="one shot flag",action='store_true')
parser.add_argument("-i",help="interval (sec)",type=int,default=int_def)

args=parser.parse_args();
interval=args.i
oneshot=args.o

config_output = 'both'

stadir='./'
#stadir=os.environ['HOME']+'/status/BME280/'
ocsv, odb = True, True
if config_output == 'db':
    ocsv, odb = False, True
if config_output == 'both':
    ocsv, odb = True, True

sql_dbname='BME280'
               
if odb:
    from influxdb import InfluxDBClient
    client = InfluxDBClient( host     = "10.37.0.212",port     = "8086",username = "root",password = "root",database= "BME280_1" )

print ("ESP_BME280 serial reader")
print ("device ID=",DEV_ID)

ports=list_ports.comports();
#print(ports)
#device=[info.description for info in ports]

ser=serial.Serial()
ser.baud=baud
#device=[info for info in ports if "CP2102N" in info.description]
device=[info.device for info in ports if "CP2102N" in info.description]
print(device)
if len(device)==0:
    print("device not found")
    exit(0)
    
#ser.
port=device[0]
#ports=port_search() #search for active ports
#print(ports)


#port=ESP_search(ports)
#print(str(DEV_ID)+" USB port="+str(port))
#exit(0)
#ser=serial.Serial('/dev/ttyUSB0',baud)

    
while 1:
#while 0:
#    ser.open()
    ser=serial.Serial(port,baud)
    line=ser.readline().decode().replace('\n','')
    lines=line.split()
    dt=datetime.datetime.today()
    print(dt.strftime('%Y/%m/%d/%H:%M:%S'),"\t",line)
    #print(lines[0])
    #print(line.decode())
    
    if odb:
        #db_data = [datetime.datetime.utcnow()]

        
        json_data = [
            {
                'measurement' : 'env',
                'fields' : {
                    'sensor'  : lines[0],
                    'pre'  : float(lines[1]),
                    'temp'  : float(lines[2]),
                    'hum'  : float(lines[3])
                },
                'time' : datetime.datetime.utcnow(),
                'tags' : {
                    'device' : 'BME280_1'
                }
            }
        ]   
        result = client.write_points(json_data)
        #cursor.execute("insert into BME280_1(sensor,pre,temp,hum) values(%s,%s,%s,%s)",(lines[0],lines[1],lines[2],lines[3]))
        
    ser.close()
    if oneshot:
        break
    sleep(interval)





    
    #ser=
