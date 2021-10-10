For this application I have installed the following

-   PITFT and GUIslice. instructions are here:
    https://github.com/ImpulseAdventure/GUIslice/wiki/Installing-GUIslice-on-Raspberry-Pi
-   wiringPi

Steps to build succesfully
- Adjust the GUIslice/examples/libirary/Makefile by adding -lwiringPi as follows:
    LDLIBS = -ISDL - ISDL_tft ${GSLC_LDLIB_EXTRA} -lwiringPi
    LDLIBS = -ISDL2 - ISDL2_tft ${GSLC_LDLIB_EXTRA} -lwiringPi
- Coppy paste the ex02_Inx_btn_txt.c file into the GUIslice/examples/libirary directory
- Compile by simply using the following command "make ex02_Inx_btn_txt"
- Run the program using the following command "sudo ./ex02_Inx_btn_txt"

The DS18B20 sensor uses the OneWire interface for data transfer. 
You can configure the OneWire inteface on any specific pin using the following command:
"sudo dtoverlay w1-gpio gpiopin=12 pullup=0"
For this project pins GPIO 12, 16, and 20 are configured for OneWire. 
Instead of 3 different GPIO's, one single GPIO could have been used, I discovered that later.
to get the data I have used the "sensor" libray and added the sensor.c and sensor.h to the Guislice location:
"Guislice/src/"
In the make files I added "../../src/sensor.c" to the "GSLC_CORE" list.







