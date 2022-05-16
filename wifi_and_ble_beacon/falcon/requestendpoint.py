#!/usr/bin/env python3

import falcon

class RequestEndpointWifi:
    def __init__(self, storage):
        self.storage = storage

    def on_post(self, req, resp):
        req_data = req.get_media()
        if 'id' in req_data:
            if self.storage.check_wifi_id(req_data['id']):
                resp.status = falcon.HTTP_200
                resp.content_type = falcon.MEDIA_TEXT
                resp.text = self.storage.get_wifi_info(req_data['id'])
            else:
                resp.status = falcon.HTTP_404
                resp.content_type = falcon.MEDIA_TEXT
                resp.text = ""                
        else:
            resp.status = falcon.HTTP_400
            resp.content_type = falcon.MEDIA_TEXT
            resp.text = ""

class RequestEndpointBLE:
    def __init__(self, storage):
        self.storage = storage

    def on_post(self, req, resp):
        req_data = req.get_media()
        if 'id' in req_data:
            if self.storage.check_ble_id(req_data['id']):
                resp.status = falcon.HTTP_200
                resp.content_type = falcon.MEDIA_TEXT
                resp.text = self.storage.get_ble_info(req_data['id'])
            else:
                resp.status = falcon.HTTP_404
                resp.content_type = falcon.MEDIA_TEXT
                resp.text = ""                
        else:
            resp.status = falcon.HTTP_400
            resp.content_type = falcon.MEDIA_TEXT
            resp.text = ""


class RequestEndpointList:
    def __init__(self, storage):
        self.storage = storage

    def on_get(self, req, resp):
        resp.status = falcon.HTTP_200
        resp.content_type = falcon.MEDIA_TEXT
        print("Request received")
        resp.text = self.storage.get_all()