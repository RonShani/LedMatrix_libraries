First thing first - Credits
This code relies on the wonderful work of:

https://github.com/zaphoyd/websocketpp
Peter Thorson - @zaphoyd https://www.zaphoyd.com

https://github.com/hzeller/rpi-rgb-led-matrix
Henner Zeller - @hzeller https://github.com/hzeller

So - thank you

#Installation:
1. Download and compile rpi-rgb-led-matrix and websocketpp libraries.
2. Update build.sh file to point the correct paths of these libs.
3. Make sure there's a boost lib ref in the system env path (if not - download and compile it and reference to it).
4. run build.sh
5. when done compiling - run the python stream script (there's also a script for Android build if you want to stream using your mobile):
https://github.com/RonShani/LedMatrix_libraries/tree/main/RPi_Websocket_stream/streamer_script
6. Take the ip address shown on the screen and put it in the start.sh file instead of the one in it (make sure the streamer and the RPi are connected to the same net.
7. run the start.sh file to start streaming.

#Author
ronicube@gmail.com
