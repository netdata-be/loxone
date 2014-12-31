# Integration in loxone

## Remote control socket

![Example on how to control the socket](img/power_control.png)

## Power Consumption stats



# Details about the smartSocket webservice

I gathered here all the progress made on this "forum": http://www.dealabs.com/bons-plans/prise-wifi-/85521?page=36

Must be replaced by your IP 192.168.0.25 IP. Login / pase default password is admin / admin

Access to Web pages hosted by taking
http://admin:admin@192.168.0.25/rd.asp

and Administration> System Command

## List of commands:

`GpioForCrond 1`: Switch On

`GpioForCrond 0`: Switch Off

`GetInfo I`: Get the current in mili-amps

`GetInfo V`: current voltage in mili-Volts

`GetInfo W`: current consumption in centi-Watts

`GetInfo E`: Usage since the last reset in mWatts / h

## Use with wget
Off:
    curl http://admin:admin@192.168.0.25/goform/SystemCommand?command=GpioForCrond+0

On:
    curl http://admin:admin@192.168.0.25/goform/SystemCommand?command=GpioForCrond+1

Read the results of the command: wget -O - -q http://admin:admin@192.168.0.25/adm/system_command.asp

Operating via a web form
View Command.html file

## Using telnet

telnet 192.168.0.25 login admin admin pwd

On: `GpioForCrond 1`
Off: `GpioForCrond 0`


## Direct access by URL
For GetInfo

http: // admin: admin@192.168.0.25/goform/SystemCommand GetInfo command = + V?

To Turn

http: // admin: admin@192.168.0.25/goform/SystemCommand GpioForCrond command = + 1?

To Extinguish

http: // admin: admin@192.168.0.25/goform/SystemCommand GpioForCrond command = + 0?
