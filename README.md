# Info on how to use the repository

Firstly:

- User and IP is: `joel@10.69.185.14`
- Image is Raspberry Pi OS Lite (32-bit) (479 MB)

Below are the commands to set up my clock code and the RTC I2C.

## Panel APIs

```bash
git clone https://github.com/hzeller/rpi-rgb-led-matrix/
cd rpi-rgb-led-matrix
make -j1
```

## Test the demo

```bash
cd examples-api-use
sudo ./demo -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=32
```

## Set up my code

```bash
g++ -O2 -std=c++17 clock.cc -o clock -Iinclude -Llib -lrgbmatrix -lpthread
sudo ./clock -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=32
```

# RTC

Enable I2C in the configs.

## Run these first time only

```bash
sudo modprobe rtc-pcf8563
sudo python3 -c "open('/sys/bus/i2c/devices/i2c-1/new_device','w').write('pcf8563 0x51\n')"
ls /dev/rtc*
```

## Make presistant

```bash
sudo nano /etc/systemd/system/pcf8563-bind.service
```

## Add

```bash
[Unit]
Description=Bind PCF8563 RTC on I2C-1 address 0x51
DefaultDependencies=no
After=systemd-modules-load.service
Before=time-sync.target

[Service]
Type=oneshot
ExecStart=/usr/bin/python3 -c "import os,subprocess; subprocess.run(['modprobe','rtc-pcf8563'],check=False); p='/sys/bus/i2c/devices/1-0051'; n='/sys/bus/i2c/devices/i2c-1/new_device'; (os.path.exists(p) or open(n,'w').write('pcf8563 0x51\\n'))"
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
```

## Enable and reboot

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now pcf8563-bind.service
sudo reboot
```

## Verify

```bash
timedatectl
cat /proc/driver/rtc
```

# Automatic clock start-up

```bash
sudo nano /etc/systemd/system/led-clock.service
```

## Add this

```bash
[Unit]
Description=RGB Matrix LED Clock
After=network.target
Wants=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/home/joel/rpi-rgb-led-matrix
ExecStart=/home/joel/rpi-rgb-led-matrix/clock -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=32
Restart=always
RestartSec=2

[Install]
WantedBy=multi-user.target
```

## Make persistent

```bash
sudo systemctl daemon-reload
sudo systemctl enable led-clock.service
```
