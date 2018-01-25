#!/bin/bash

echo "Instalando python3-pip e as bibliotecas numpy e matplotlib"
sudo apt-get -y install python3-pip
sudo apt-get -y install python3-ptk
sudo apt-get install python3-tk

sudo pip3 install --upgrade pip
sudo pip3 install numpy
sudo pip3 install matplotlib