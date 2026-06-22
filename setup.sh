#!/bin/sh

export ESP_BME280_ROOT="/home/msgc/repo/ESP_BME280"
export ESP_BME280_PYTHON="$ESP_BME280_ROOT/.venv/bin/python3"
export ESP_BME280_PROGRAM="$ESP_BME280_ROOT/scripts/ESP_BME280-ADS1115_serial_influx.py"
export ESP_BME280_CONFIG="$ESP_BME280_ROOT/config/config.json"
export ESP_BME280_LOGFILE="$ESP_BME280_ROOT/bme280.log"