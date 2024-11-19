#!/bin/bash

sudo apt-get update -y 
#-qq -y 2>/dev/null

sudo apt-get install -y ffmpeg 
#-qq -y 2>/dev/null

sudo apt-get install -y libgtk2.0-dev
#-qq -y 2>/dev/null

sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
#-qq -y 2>/dev/null

sudo apt-get install -y curl

sudo apt-get --fix-broken install -y

echo -e 'Dependencies installed ...'