#!/bin/bash

sudo apt-get update -qq -y 
#2>/dev/null

sudo apt-get install -qq -y ffmpeg 
#2>/dev/null

#sudo apt-get install gtk2.0 -qq -y 2>/dev/null

sudo apt-get install libgtk2.0-dev -qq -y 
#2>/dev/null

sudo apt-get install -qq -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
#2>/dev/null

echo -e 'Dependencies installed ...'