#!/bin/sh

PROJECT_ROOT="/home/msgc/repo/ESP_BME280"
. "$PROJECT_ROOT/setup.sh" || exit 1

# スクリプトが実行中か確認
if ! pgrep -f "$ESP_BME280_PROGRAM" > /dev/null; then
    echo "$(date): ESP_BME280-ADS1115_serial_influx.py is not running. Starting it..."
    echo "sudo password may be required to access the serial device."

    sudo "$ESP_BME280_PYTHON" "$ESP_BME280_PROGRAM" --config "$ESP_BME280_CONFIG" > "$ESP_BME280_LOGFILE" 2>&1
else
    echo "$(date): ESP_BME280-ADS1115_serial_influx.py is already running."
fi
