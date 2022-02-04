#!/usr/bin/python3
import serial
import time
import datetime
import os,sys
import argparse
import json
#import pyvisa as visa

#ID=0x7f36e066ab38
DEV_ID="CP2102N"
baud=115200
from serial.tools import list_ports
from time import sleep

int_def=10
parser = argparse.ArgumentParser(description='IT6332A Control tool')
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
    import pymysql.cursors
    conn = pymysql.connect(host='10.37.0.214',port=3306,user='rubis',passwd='password',autocommit='true')
    cursor = conn.cursor()
#    cursor.execute("DELETE DATABASE" + sql_dbname)
    cursor.execute("CREATE DATABASE IF NOT EXISTS " + sql_dbname)
    cursor.execute("USE " + sql_dbname)
#    cursor.execute("DROP TABLE  BME280_1")
    cursor.execute("CREATE TABLE IF NOT EXISTS  BME280_1(time TIMESTAMP not null default CURRENT_TIMESTAMP, sensor VARCHAR(255),pre FLOAT, temp FLOAT,hum FLOAT)")



print ("ESP_BME280 serial reader")
print ("device ID=",DEV_ID)
#r = visa.ResourceManager()
#print(r.list_resources())

ports=list_ports.comports();
print(ports)
#device=[info.description for info in ports]
device=[info for info in ports if "CP2102N" in info.description]
print(device)
if len(device)==0:
    print("device not found")

while 1:
#while 0:

    ser=serial.Serial('/dev/ttyUSB0',baud)
    line=ser.readline().decode().replace('\n','')
    lines=line.split()
    dt=datetime.datetime.today()
    print(dt.strftime('%Y/%m/%d/%H:%M:%S'),"\t",line)
#    print(lines[0])
    #print(line.decode())
  
    if odb:
        date_now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
#        cursor.execute("insert into BME280_1(pre,temp,hum) values(%s,%s,%s)",(lines[1],lines[2],lines[3]))  
        cursor.execute("insert into BME280_1(sensor,pre,temp,hum) values(%s,%s,%s,%s)",(lines[0],lines[1],lines[2],lines[3]))  
    ser.close()
    if oneshot:
        break
    sleep(interval)





    
    #ser=
