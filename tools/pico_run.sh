#!/bin/bash

picotool load -f -x "$1"
tio /dev/ttyACM0
