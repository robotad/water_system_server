#!/usr/bin/env python

import json
import requests
import serial
import sys
from time import sleep


ready = False
while not ready:
    try:
        ser = serial.Serial('/dev/ttyACM0', 9600, 8, 'N', timeout=5)
        ready = True
    except:
        sys.stderr.write('Arduino is not ready!\n')
        sleep(10)

 
while True:
    ser_data = ser.readline()
    print('Serial in: %s' % (ser_data))
    
    try:
        ser_json = json.loads(ser_data)
        try:
            req = requests.post("https://18.206.145.189:8000", json=ser_json, cert=("pi.crt", "pi.key"), verify="ca.crt")
            sys.stdout.write('Response(%s): %s\n' % (req.status_code, req.text))
        except:
            sys.stderr.write('Error: could not send serial data to server!\n') 
    except:
        sys.stderr.write('Error: serial in is not json!\n')
        
        
        
