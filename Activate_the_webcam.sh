#!/bin/bash

mknod /dev/video0 c 81 0
chmod 666 /dev/video0
ln -s /dev/video0 /dev/video