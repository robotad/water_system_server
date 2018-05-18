#!/usr/bin/env python

import requests
import serial
import sys


ser = serial.Serial('/dev/ttyACM0', 9600, 8, 'N', timeout=5)

while True:
    print('Serial in: %s' % (ser.readline()))
    req = requests.post("https://18.206.145.189:8000", json="{\"test\":\"test\"}", cert=("pi.crt", "pi.key"), verify="ca.crt")
    sys.stdout.write('Response(%s): %s\n' % (req.status_code, req.text))
