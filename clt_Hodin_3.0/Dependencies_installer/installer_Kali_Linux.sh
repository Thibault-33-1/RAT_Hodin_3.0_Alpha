#!/bin/bash

#echo "deb http://http.kali.org/kali kali-rolling main non-free contrib" | sudo tee -a /etc/apt/sources.list

#echo "deb-src http://http.kali.org/kali kali-rolling main non-free contrib" | sudo tee -a /etc/apt/sources.list

#echo "deb http://http.kali.org/kali kali-rolling main non-free contrib" | sudo tee -a /etc/apt/sources.list

sudo apt update -y 
#-qq -y 2>/dev/null

sudo apt upgrade -y
#qq -y 2>/dev/null

sudo apt install -y ffmpeg 
# -qq -y ffmpeg 2>/dev/null

sudo apt install -y gstreamer1.0-plugins-*
#-qq -y 2>/dev/null

sudo apt -y install libgtk2.0-dev

sudo apt-get install -y curl

echo -e 'Dependecies installed ....'