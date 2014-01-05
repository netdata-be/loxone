See the original forum [thread][1] for comments.

### 1)  Requirements

You need a garage door with Hormann Supramatic 2/3
Buy yourself a Hormann UAP1.
I've bought mine [here][2] 

The UAP enables you to

- control the light of the supramatic 
- open the garage door
- close the garage door
- stop the garage door (Not needed on seperate output for me)
- check whether the door is totally closed or totally open, you don't get an exact position
- light status? (not tried that one)

###  2) Start working

Find your serial number of your Supramatic. Mine was a D version. 

Wiring of version D is not the same as for version a, b or c.
Check out the included "schaltplan" and wire.

I've wired 01 (0.5 & 0.8) status door open and 02 (0.5 & 0.8) status door closed to digital inputs of my Loxone
and contacts 10 (light),17 (close door),15 (open door),23 (stop door) to digital outputs.

### 3) Loxone config

Open your loxone config and configure.
I've used the `gate block` to open/close the door
Connect the buttons you would like to use to Io and Ic

Connect 15 to Qo
Connect 17 to Qc

Status of the door and lighting control of the door is nothing special. 
When I tried that I needed to press two to stop the gate (or open it or close it)

I found out it had to do with the Td time, so I changed the time to 0.5 second or something like that.

This has some drawbacks, like the animation of the door on my android app that closes/opens too quick but I haven't looked in depth to look for another solution.

###  4) Enjoy and relax

Pushing input button lo opens the gate, pushing it again stops the gate
same for button lc for closing

That should be it, hormann remote sender still works as well without a hassle for me.


  [1]: http://forum.loxone.com/enen/faqs-tutorials-and-howto-s/4399-hormann-garage-door-via-loxone.html
  [2]: http://www.tor7.de/Garagentorantriebe/SupraMatic/Hoermann-Universaladapterplatine-UAP-1-fuer-Supramatic-Serie-2-Serie-3::119.html
