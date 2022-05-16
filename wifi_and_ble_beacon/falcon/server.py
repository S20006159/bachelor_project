#!/usr/bin/env python3

import falcon
import ssidendpoint as ssep
import collectorendpoint as collep
import requestendpoint as reqep
import storage

# Create storage object
global_storage = storage.Storage()
# Create all necessary objects for handling the requests
ssid_endpoint = ssep.SSIDEndpoint()
collector_wifi_endpoint = collep.CollectorEndpointWifi(global_storage)
collector_ble_endpoint = collep.CollectorEndpointBLE(global_storage)
request_wifi_endpoint = reqep.RequestEndpointWifi(global_storage)
request_ble_endpoint = reqep.RequestEndpointBLE(global_storage)
request_all = reqep.RequestEndpointList(global_storage)

application = falcon.App()

# Create all necessary uri-paths and assign the corresponding handlers
application.add_route('/ssid', ssid_endpoint)
application.add_route('/collector/wifi', collector_wifi_endpoint)
application.add_route('/collector/ble', collector_ble_endpoint)
application.add_route('/request/wifi', request_wifi_endpoint)
application.add_route('/request/ble', request_ble_endpoint)
application.add_route('/request/all',request_all)
