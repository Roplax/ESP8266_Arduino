# Spec Document

Code to be flashed using Arduino IDE (C, etc)
  Out of box connect to a default Wifi network
  The first client that makes a call with right serial number to /setup is the only trusted one and let them set the wifi network ssid and PSK,  reboot
  Read temp. Humidity sensor with reasonable sampling speed with a time stamp
  Have a history of Last 12 Hrs, then overwrite the buffer and keep a pointer to original data 
  When a client  makes a call to /Data give them current buffer,   flush the buffer of handed over data


Test Cases
  Try from two different cellphones/tablet
  Manual Browser call is OK
  Must be able to demonstrate switching SSID/PSK once and then back
  Show data in text format in browser initial go-around
