#!/bin/bash

mkdir ~/ctf/tool
cd ~/ctf/tool
git clone https://github.com/mozilla/codemoji
cd codemoji
sudo apt-get install -y nodejs
sudo apt install -y node-gyp npm
sudo npm config set registry https://registry.npm.taobao.org
sudo npm i -g npm
npm install -g bower
npm install -g grunt-cli
npm install -g gulp-cli 

npm install
bower install --allow-root
grunt dev
