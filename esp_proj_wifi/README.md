A small project consisting basic wifi scenarios that you may use in project.

For Eg:

    -ESP8266 starts with station mode enabled -> tries to connect to AP info saved in flash.
    -If it fails to connect, it will switch on LOS led.
    -User can then press scan button which will cause esp to scan available AP's and then switch to SoftAP mode.
    -User can then connect it to through wifi on phone or pc with ssid ESP8266 and password esp8266_01. (max 1 connection allowed).
    -Then they can go to browser and go to URL 192.168.4.1/ which will retrun the list of scanned router AP's.
    -User can select the router from the list and enter its password and submit it or they can press scan button again to scan again.
    -When submitted esp8266 will go in station mode and try to connect to ssid with credentials provided.
    -When successful it will ping to google DNS 8.8.8.8 to check for internet connectivity. If successful it switch offs the LOS LED.

