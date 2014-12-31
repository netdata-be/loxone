# Integration in loxone

## Remote control socket

Here is a simple example on how you could control the SmartSocket:
![Example on how to control the socket](img/power_control.png)

To control the socket you have to create a "virtual output":http://www.loxone.com/enuk/service/documentation/loxone-config/global-functions/virtual-io.html and a virtual output command.
I have 3 smart sockets so I added 3 `Virtual outputs` and 3 `virtual output commands`.

![My virtual outputs](img/virtual_output.png)

Here are the details of the virtual output:

![virtual output details](img/virtual_output_settings.png)

And this is the `virtual output command`:

![virtual output command details](img/virtual_output_command.png)

For easy copy paste this are the settings:

Command for On:

    /goform/SystemCommand?command=GpioForCrond+1

Command for Off:

    /goform/SystemCommand?command=GpioForCrond+0

That's it for remote controlling the socket.
Now you are able to use this output command as a regular output in loxone.

## Power Consumption stats

![Power Consumption](img/socket3.png)

In order to collect the power consuption we need a PicoC script to parse the values.
The source code for this PicoC program can be found here.

This is how I configured in inside my loxone config:

![Power Consumption config](img/powerstats.png)



# Details about the smartSocket webservice

I gathered here all the progress made on this "forum":http://www.dealabs.com/bons-plans/prise-wifi-/85521?page=36

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
