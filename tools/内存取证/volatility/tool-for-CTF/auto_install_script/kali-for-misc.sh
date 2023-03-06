#!/bin/bash
# 换源
sudo rm /etc/apt/sources.list
sudo echo "deb http://mirrors.tuna.tsinghua.edu.cn/kali kali-rolling main contrib non-free" > /etc/apt/sources.list
sudo echo "deb-src https://mirrors.tuna.tsinghua.edu.cn/kali kali-rolling main contrib non-free" >> /etc/apt/sources.list
apt-get update
# pip
sudo apt-get install -y python-pip
sudo apt-get install -y python3-pip
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
# mkdir
mkdir -p ~/ctf/tool
cd ~/ctf/tool
# volatility
sudo apt-get install -y volatility
# foremost
sudo apt-get install -y foremost
# pngcheck
sudo apt-get install -y pngcheck
# imagemagick(convert)
sudo apt-get install -y imagemagick
# exiftool
sudo apt-get install -y exiftool
# steghide
sudo apt-get install -y steghide
# gaps
git clone https://github.com/nemanja-m/gaps
cd gaps
pip3 install numpy opencv-python pytest matplotlib pillow
python3 setup.py install
cd ../
# 盲水印
git clone https://github.com/chishaxie/BlindWaterMark
# F5隐写
git clone https://github.com/matthewgao/F5-steganography
# lsb
git clone https://github.com/livz/cloacked-pixel
# RSA
git clone https://github.com/Ganapati/RsaCtfTool
sudo apt-get install -y libgmp3-dev libmpc-dev
cd RsaCtfTool
pip3 install -r requirements.txt -y
cd ../
# outguess隐写
git clone https://github.com/crorvick/outguess
cd outguess
sudo ./configure
sudo make
sudo make install
cd ../
# zsteg
gem sources --remove https://rubygems.org/
gem sources --add https://gems.ruby-china.com/
gem install zsteg
