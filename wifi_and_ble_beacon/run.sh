#!/usr/bin/with-contenv bashio

CONFIG_PATH=/data/options.json

# Read the values from the config and store them
export WIFI_FILTER="$(bashio::config 'wifi_filter')"
export BLE_FILTER="$(bashio::config 'ble_filter')"
export OW_THRESHOLD="$(bashio::config 'overwrite_threshold')"
export SSID="$(bashio::config 'ssid')"
export AWAY_TIMER="$(bashio::config 'away_timer')"

# Start the web server and load the rest server application from server.py
gunicorn --chdir /server server:application -c /server/gunicorn.conf.py
