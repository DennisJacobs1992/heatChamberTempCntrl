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




