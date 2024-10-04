#!/bin/bash

#echo "deb http://http.kali.org/kali kali-rolling main non-free contrib" | sudo tee -a /etc/apt/sources.list

#echo "deb-src http://http.kali.org/kali kali-rolling main non-free contrib" | sudo tee -a /etc/apt/sources.list

#echo "deb http://http.kali.org/kali kali-rolling main non-free contrib" | sudo tee -a /etc/apt/sources.list

sudo apt-get update -y 
#-qq -y 2>/dev/null

sudo apt-get upgrade -y
#qq -y 2>/dev/null

sudo apt-get install -qq -y ffmpeg 
# -qq -y ffmpeg 2>/dev/null

sudo apt-get install -qq -y gstreamer1.0-plugins-*
#-qq -y 2>/dev/null

sudo apt-get -qq -y install libgtk2.0-dev

echo -e 'Dependecies installed ....'