#!/usr/bin/python3
import serial
import argparse

def main():
    port="/dev/ttyUSB0"
    print("serial_out")
    parser = argparse.ArgumentParser()    
    parser.add_argument('arg1', help='ON or OFF')
    args=parser.parse_args()
    serialCommand = args.arg1
    print("sending ",serialCommand, " command")
    #serialCommand = "ON"
    #writeSer = serial.Serial('/dev/ttyUSB0',115200, timeout=3)
    writeSer = serial.Serial(port,115200, timeout=3)
    writeSer.write(serialCommand.encode())
    writeSer.close()

    
if __name__ == "__main__":
    main()
