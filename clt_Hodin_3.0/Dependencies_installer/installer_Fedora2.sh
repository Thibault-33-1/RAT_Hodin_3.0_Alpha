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

sudo dnf install -y gstreamer1-devel gstreamer1-plugins-base-tools gstreamer1-doc gstreamer1-plugins-base-devel gstreamer1-plugins-good gstreamer1-plugins-good-extras gstreamer1-plugins-ugly gstreamer1-plugins-bad-free gstreamer1-plugins-bad-free-devel gstreamer1-plugins-bad-free-extras
#-qq -y 2>/dev/null

sudo dnf install -y gtk2-devel

sudo dnf install -y curl

echo -e "Dependencies installed ..."
