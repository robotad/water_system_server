#!/usr/bin/env python

import requests

req = requests.get("https://18.206.145.189:8000", cert=("pi.crt", "pi.key"), verify="ca.crt")

