#!/bin/bash

echo 'close 3 days before index of log'

python /opt/antell/CloudHands/bin/ESTools.py localhost closeWithDate log_* 3
