#!/usr/bin/env python3

import falcon
import os

class SSIDEndpoint:
    def __init__(self):
        self.ssid = os.getenv('SSID')

    def on_get(self, req, resp):
        resp.status = falcon.HTTP_200
        resp.content_type = falcon.MEDIA_TEXT
        resp.text = self.ssid