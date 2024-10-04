#!/bin/bash

sudo dnf check-update -y
#-qq -y 2>/dev/null

sudo dnf upgrade -y
# -qq -y 2>/dev/null

#dnf --y install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm 2>/dev/null
#dnf -y install https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm 2>/dev/null

sudo dnf -y install ffmpeg 
#-qq 2>/dev/null

sudo dnf -y install ffmpeg-devel 
#-qq 2>/dev/null

sudo dnf install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
#-qq -y 2>/dev/null

sudo dnf install -y gtk2-devel

echo -e "Dependencies installed ..."
