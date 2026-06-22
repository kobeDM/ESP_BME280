# ESP_BME280
use aruduino sketch and read with python script (mysql or influx db)

## Setup

Source `setup.sh` to register the local environment variables used by the scripts.

```sh
. ./setup.sh
```

## Run

Start the reader check script from the repository root:

```sh
./scripts/check_esp_bme280.sh
```

The reader uses `config/config.json` by default and writes its output to `bme280.log`.
