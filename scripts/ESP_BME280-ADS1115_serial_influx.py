#!/usr/bin/env python3

import serial
import datetime
import argparse
import json

from serial.tools import list_ports
from time import sleep


# ----------------------
# Config
# ----------------------

def load_config(path):
    with open(path, "r") as f:
        return json.load(f)


# ----------------------
# Serial
# ----------------------

def find_device():
    ports = list_ports.comports()
    device = [info.device for info in ports if "CP2102N" in info.description]
    return device[0] if device else None


def open_serial(port, baud):
    return serial.Serial(port, baud, timeout=1)


# ----------------------
# InfluxDB
# ----------------------

def init_influx(cfg):
    from influxdb import InfluxDBClient
    return InfluxDBClient(
        host=cfg["host"],
        port=cfg["port"],
        username=cfg["username"],
        password=cfg["password"],
        database=cfg["database"]
    )


def write_influx(client, cfg, lines):
    json_data = [
        {
            "measurement": cfg["measurement"],
            "fields": {
                "sensor": lines[0],
                "pre": float(lines[1]),
                "temp": float(lines[2]),
                "hum": float(lines[3]),
                "adc0": float(lines[4]),
                "adc1": float(lines[5]),
                "adc2": float(lines[6]),
                "adc3": float(lines[7])
            },
            "time": datetime.datetime.utcnow(),
            "tags": {
                "device": cfg["device_tag"],
                "host": cfg["host_tag"]
            }
        }
    ]
    client.write_points(json_data)


# ----------------------
# Monitoring loop
# ----------------------

def run_monitoring(cfg, interval, oneshot):

    port = find_device()
    if not port:
        print("Device not found")
        return

    print("Using port:", port)

    ser = open_serial(port, cfg["serial"]["baud"])

    client = None
    if cfg["influx"]["enable"]:
        client = init_influx(cfg["influx"])

    try:
        while True:
            line = ser.readline().decode().strip()
            parts = line.split()

            if len(parts) == 8:
                now = datetime.datetime.now()
                print(now.strftime("%Y/%m/%d/%H:%M:%S"), "\t", line)

                if client:
                    write_influx(client, cfg["influx"], parts)
            else:
                print("Incomplete data")

            if oneshot:
                break

            sleep(interval)

    except KeyboardInterrupt:
        print("\nGraceful shutdown")

    finally:
        ser.close()


# ----------------------
# Main
# ----------------------

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", default="../config/config.json")
    parser.add_argument("-o", action="store_true")
    parser.add_argument("-i", type=int, default=10)

    args = parser.parse_args()

    cfg = load_config(args.config)

    print("ESP_BME280 serial reader")
    print("Device ID =", cfg["device_id"])

    run_monitoring(cfg, args.i, args.o)


if __name__ == "__main__":
    main()