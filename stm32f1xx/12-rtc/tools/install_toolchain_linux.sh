#!/bin/bash
# Username should be provided. Suggested call: "sudo ./install_toolchain.sh $USER"
if [ $# -eq 0 ]
then
    echo "Error! No user supplied. Try \"sudo ./install_toolchain.sh \$USER\""
    exit 1
elif [ $(grep -c "^$1:" /etc/passwd) -eq 0 ]; then
    echo "The provided user ($1) does not exist. Try \"sudo ./install_toolchain.sh \$USER\""
    exit 1
fi
sudo apt update
sudo apt-get -y install git
sudo apt-get -y install build-essential
sudo apt-get -y install cmake
sudo apt-get -y install ninja-build
sudo apt-get -y install gcc-arm-none-eabi
sudo apt-get -y install gdb
sudo apt-get -y install gdb-multiarch
sudo apt-get -y install stlink-tools

sudo apt-get -y install linux-tools-virtual hwdata
sudo update-alternatives --install /usr/local/bin/usbip usbip `ls /usr/lib/linux-tools/*/usbip | tail -n1` 20
sudo apt-get -y install libudev-dev
if [ ! -f "/etc/udev/rules.d/60-openocd.rules" ]
then
    cd /etc/udev/rules.d
    sudo wget https://raw.githubusercontent.com/openocd-org/openocd/master/contrib/60-openocd.rules
    sudo usermod -a -G plugdev $1
    sudo service udev restart
    sudo udevadm control --reload
fi

