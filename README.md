# NeuroSky-Race

In this application you can use your attention level to control car speed. Application designed for two racers. You will need following items for this project:

    - NeuroSky Mindwave Sensors * 2
    - STM32F103C8T6
    - IRLZ44N Mosfet * 2
    - HC-05 Bluetooth * 2
    - Slot Car Track
    - ST-Link
    - FTDI Converter(Optional) 
    - Micro USB Cable(Optional)
    - PC (Optional)


### First-Upload
You can upload this code to STM32 with using ST-Link debugger module. You will not need any configururation.

```

### USB or FTDI Connection

You can use FTDI connection or USB Micro cable for reading headset values from STM32. STM32 will write attention and signal quality values to this interfaces.



### Interface
If you want to use NodeJs web interface you will need a PC to run server. Connect two Arduino with USB cable to PC and set port names in "server.js"

```javascript
var portName1 = "COM5";         //if you use using Linux /dev/tty***
```

```sh

$ cd Web-Interface
$ npm install
$ node server.js

```

<img src="imgs/yarisArayüz.png"></img>



