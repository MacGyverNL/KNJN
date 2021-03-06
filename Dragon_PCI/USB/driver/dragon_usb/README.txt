Here is a Linux USB driver for KNJN Dragon. You can control 

LED1    (close to USB)
LED2    (close to FPGA) 
LED3    (close to Ethernet)

by sending hex value to Dragon through the driver. You can also control text LCD (HD44780) as
described in http://www.knjn.com/FPGA-PCI-LCD.html.

First you should load firmware then design with 'dragon-ctrl' :

# dragon-ctrl -i firmware.hex reboot
# dragon-ctrl -b USB_IO8.bit program


You can compile the driver with :

$ make

You can load it with :

# insmod dragon_usb.ko

Once driver is loaded, you can access Dragon with both /sys entry or /dev/dragon_usb0.
Value to send depends on design (USB_IO8,  USB_reg_banks, LCD_Text). Don't forget to load 
design file *before* loading Linux driver ! You control board by sending hex value (32 bits max) 
or ONE char, examples :

# echo -n 61 > /dev/dragon_usb0   ==> will display 'a'
# echo -n \'a > /dev/dragon_usb0   ==> will display 'a'


- IO8 test (USB_IO8.bit) :

You should send the following values to set leds on:

    0x1 LED1
    0x2 LED2
    0x4 LED3

examples:

# echo 4 > /sys/bus/usb/drivers/dragon_usb/6-1:1.0/ledmask
# cat /sys/bus/usb/drivers/dragon_usb/6-1:1.0/ledmask 0x4
0x4

  or

# echo 4 > /dev/dragon_usb0

- USB_reg_banks test (USB_reg_banks.bit) :

You should send the following values to set leds on:

  0x00010000 LED1
  0x01000002 LED2
  0x80001000 LED3
  ^     ^
  |     |__ addr
  |__ data 

Examples:

# echo 80001000 > /sys/bus/usb/drivers/dragon_usb/6-1:1.0/ledmask
# cat /sys/bus/usb/drivers/dragon_usb/6-1:1.0/ledmask 0x80001000
0x80001000

  or

# echo 80001000 > /dev/dragon_usb0

- LCD test (LCD_Text.bit) :

Just try the lcd_test.sh script as follows :

# ./lcd_test.sh > /dev/dragon_usb0

It should display "Hello Linux!" on the LCD screen. You can also give a
string as parameter :

# ./lcd_test.sh "A sample string" > /dev/dragon_usb0


Enjoy.

PF
08/2011 
