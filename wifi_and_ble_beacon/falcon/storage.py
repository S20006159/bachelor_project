#!/usr/bin/env python3
import os
import json
from datetime import datetime

default_filter = 'FF:FF:FF:FF:FF:FF'

class Storage:
    def __init__(self):
        # Load the value for the overwrite threshold
        self.ow_threshold = 5
        if os.getenv("OW_THRESHOLD") is not None:
            try:
                self.ow_threshold = float(os.getenv("OW_THRESHOLD"))
            except ValueError:
                pass

        self.away_timer = 60
        if os.getenv("AWAY_TIMER") is not None:
            try:    
                self.away_timer = float(os.getenv("AWAY_TIMER"))
            except ValueError:
                pass
        
        # Load the filter values from the enviroment variables
        self.wifi_filter = None
        wifi_filter_str = os.getenv('WIFI_FILTER')
        if wifi_filter_str is not None and wifi_filter_str.casefold() != default_filter.casefold():
            print("Filter found, ", wifi_filter_str)
            self.wifi_filter = [ x.casefold() for x in wifi_filter_str.split(',') ]    
            print("Normalized filter: ", self.wifi_filter)
        else:
            print('No  Filter or default filter: ', wifi_filter_str)

        self.ble_filter = None
        ble_filter_str = os.getenv('BLE_FILTER')
        if ble_filter_str is not None and ble_filter_str.casefold() != default_filter.casefold():
            print("Filter found, ", ble_filter_str)
            self.ble_filter = [ x.casefold() for x in ble_filter_str.split(',') ]    
            print("Normalized filter: ", self.ble_filter) 
        else:
            print('No  Filter or default filter: ', ble_filter_str)

        # Create the storage dictionary
        self.storage_dict_wifi = {}
        self.storage_dict_ble = {}
        self.last_cleanup = datetime.now()

    def cleanup_storage(self, time):
        if (time-self.last_cleanup).total_seconds() > 1:
            self.last_cleanup = time
            wifi_keys = list(self.storage_dict_wifi)
            for wifi_key in wifi_keys:
                last_seen, rssi, beacon = self.storage_dict_wifi[wifi_key]
                if (time - last_seen).total_seconds() > self.away_timer:
                    self.storage_dict_wifi.pop(wifi_key, None)
            ble_keys = list(self.storage_dict_ble)
            for ble_key in ble_keys:
                last_seen, rssi, beacon = self.storage_dict_ble[ble_key]
                if (time - last_seen).total_seconds() > self.away_timer:
                    self.storage_dict_ble.pop(ble_key, None)
    
    def insert_wifi(self, id, rssi, beacon):
        time = datetime.now()
        self.cleanup_storage(time)
        #print("trying to insert: ", id, rssi, beacon)
        if self.wifi_filter is None or (self.wifi_filter is not None and id in self.wifi_filter):
            #print("Id in filter")
            if id in self.storage_dict_wifi:
                #print("Id in storage")
                old_time, old_rssi, old_beacon = self.storage_dict_wifi[id]
                #print("Old values: ", old_time, old_rssi, old_beacon)
                if beacon is old_beacon:
                    #print("Same beacon")
                    self.storage_dict_wifi[id] = (time, rssi, beacon)    
                elif (rssi >= old_rssi) or ((time-old_time).total_seconds() >= self.ow_threshold):
                    # print("Higher rssi or too old info")
                    self.storage_dict_wifi[id] = (time, rssi, beacon)
            else:
                # print("Id not in storage")
                self.storage_dict_wifi[id] = (time, rssi, beacon)
        else:
            # print("Id not in filter")
            pass


    def insert_ble(self, id, rssi, beacon):
        time = datetime.now()
        #print("trying to insert: ", id, rssi, beacon)
        if self.ble_filter is None or (self.ble_filter is not None and id in self.ble_filter):
            #print("Id in filter")
            if id in self.storage_dict_ble:
                #print("Id in storage")
                old_time, old_rssi, old_beacon = self.storage_dict_ble[id]
                #print("Old values: ", old_time, old_rssi, old_beacon)
                if beacon is old_beacon:
                    #print("Same beacon")
                    self.storage_dict_ble[id] = (time, rssi, beacon)    
                elif (rssi >= old_rssi) or ((time-old_time).total_seconds() >= self.ow_threshold):
                    # print("Higher rssi or too old info")
                    self.storage_dict_ble[id] = (time, rssi, beacon)
            else:
                # print("Id not in storage")
                self.storage_dict_ble[id] = (time, rssi, beacon)
        else:
            # print("Id not in filter")
            pass

    def check_wifi_id(self, id):
        return id in self.storage_dict_wifi

    def check_ble_id(self, id):
        return id in self.storage_dict_ble

    def get_wifi_info(self, id):
        time, rssi, beacon = self.storage_dict_wifi[id]
        return '{"time":"'+time.strftime("%Y-%m-%d %H:%M:%S")+'","rssi":'+str(rssi)+',"beacon":"'+beacon+'"}'

    def get_ble_info(self, id):
        time, rssi, beacon = self.storage_dict_ble[id]
        return '{"time":"'+time.strftime("%Y-%m-%d %H:%M:%S")+'","rssi":'+str(rssi)+',"beacon":"'+beacon+'"}'

    def get_all(self):
        return '{"WiFi":' + json.dumps(self.storage_dict_wifi, default=str, separators=(',',':'), indent=2) + ',\n"BLE":' + json.dumps(self.storage_dict_ble, default=str, separators=(',',':'), indent=2) + '}'