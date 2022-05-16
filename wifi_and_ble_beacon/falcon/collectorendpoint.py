#!/usr/bin/env python3

import falcon

class CollectorEndpointWifi:
    def __init__(self, storage):
        self.storage = storage

    def on_post(self, req, resp):
        req_data = req.get_media()
        if 'id' in req_data and 'rssi' in req_data and 'beacon' in req_data:
            self.storage.insert_wifi(req_data['id'],req_data['rssi'],req_data['beacon'])
            resp.status = falcon.HTTP_200
            resp.content_type = falcon.MEDIA_TEXT
            resp.text = ""
        else:
            resp.status = falcon.HTTP_400
            resp.content_type = falcon.MEDIA_TEXT
            resp.text = ""


class CollectorEndpointBLE:
    def __init__(self, storage):
        self.storage = storage

    def on_post(self, req, resp):
        req_data = req.get_media()
        if 'id' in req_data and 'rssi' in req_data and 'beacon' in req_data:
            self.storage.insert_ble(req_data['id'],req_data['rssi'],req_data['beacon'])
            resp.status = falcon.HTTP_200
            resp.content_type = falcon.MEDIA_TEXT
            resp.text = ""
        else:
            resp.status = falcon.HTTP_400
            resp.content_type = falcon.MEDIA_TEXT
            resp.text = ""