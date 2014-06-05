# Step 1: Adding a weather service

You can add a weather service by opening the loxplan file with notepad
And insert XML tags below after this line:

```xml
    <Control V="63" UUID="????????-????-????-????????????????" Title="Server communicatie" Type="WeatherCaption"/>
```

So here's the tag to insert:

```xml
    <Control V="63" UUID="6b028449-8152-11e1-b36ada6017029158" Title="myweather" Type="WeatherServer" Color="0,0,0" SType="1" Address="" Program="" VisuUUID="d83b0000-8727-11e1-bd1df582fe1a4aa2">
        <IoData Visu="true" UserGroupRef="7e6c7840-74eb-11e1-9520fbc337c08ef8" UserGroupRefRemote="7e6c7840-74eb-11e1-9520fbc337c08ef8">
            <LoggerMailer V="20" MinimumTime="20"/>
        </IoData>
    </Control>
```

Starting with Loxone Config 5.1, use after:

```xml
	<C Type="WeatherCaption" V="70" U="????????-????-????-????????????????" Title="Serverkommunikation"/>
```

And the tag to insert:

```xml
    <C V="63" UUID="6b028449-8152-11e1-b36ada6017029158" Title="myweather" Type="WeatherServer" Color="0,0,0" SType="1" Address="" Program="" VisuUUID="d83b0000-8727-11e1-bd1df582fe1a4aa2">
        <IoData Visu="true" UserGroupRef="7e6c7840-74eb-11e1-9520fbc337c08ef8" UserGroupRefRemote="7e6c7840-74eb-11e1-9520fbc337c08ef8">
            <LoggerMailer V="20" MinimumTime="20"/>
        </IoData>
    </C>
```

Then save the file and open in loxplan. You will see myweather in the server communication section.

Beware of
  1 memory leaks though, and keep a spare micro SD at hand in case you mess with the code a lot.
  2 The UUID and visuUUID and usergroupref work perfectly in my setup and I guess these are defaults. However if they are different in your loxplan I cannot predict what happens. Create a backup of your own config before playing with my config.

Note: once you have it in the loxplan it will port through all updates etc.

# Step 2: Add the PicoC to your project #

So after you now open your loxone project you should see the myweather object.
Copy paste the PicoC code inside the programm.

![Alt text](https://raw2.github.com/netdata/loxone/master/img/myweather.png "Loxone weather service object")

After pasting you have to change a couple of settings at the top:

    char* apiKey   = "YOUR-API-KEY";     // your personal 8 hex characters api key from your account at Wunderground (need to request this key once !)
    char* location = "YOUR-LOCATION";    // your location


# Step 3: Use the weather service inside your program #

Currently there is an issue with all Loxone version in regard to the `setweatherdata` call.
The weather data is set and visible inside the webinterface and inside the mobile app, however when using the weather data inside your loxone project, it never get's updated.
After creating a support ticket at loxone I received the following answer:

    I have made a note on the lack of documentation on the weatherdata scripts specifically,
    and also looking into possible issues with the PicoC functions.
    This has all been forwarded for action, however this may take some time,
    and I understand you would like this issue resolved.
    
    I have therefore developed a workaround that still achieves the desired result for you.

The workaround I received was to set a `Virtual Input` at the certain value and use this `Virtual Input` in your project, so not the actual weatherdata.

The following `Virtual Inputs` are set by the PicoC program:

  * `currentOutsideTemp` - Is set the to current Temperature
  * outsideTemp_offset_hour_1` - Is set to the what the temp will be after 1 hour
  * `outsideTemp_offset_hour_2` - Is set to the what the temp will be after 2 hour
  * `outsideTemp_offset_hour_3` - Is set to the what the temp will be after 3 hour
  * `...`
  * `outsideTemp_offset_hour_x` - Is set to the what the temp will be after x hours

So create the `Virtual Inputs` you would like to use and add them to your project.

This image shows the properties of the Virtual Input

![Alt text](https://raw2.github.com/netdata/loxone/master/img/virtual_input_properties.png "Loxone weater service virtual inputs")

This image shows the live view

![Alt text](https://raw2.github.com/netdata/loxone/master/img/virtual_inputs.png "Loxone weater Virtual Input liveview")

And if you look at the webinterface weather you will notice it has the same values:

![Alt text](https://raw2.github.com/netdata/loxone/master/img/weather_service_webinterface.png "Loxone weater service webinterface")

And finally if you use the virtual input in your visualization it looks like this:

![Alt text](https://raw2.github.com/netdata/loxone/master/img/outside_temp_graph.png "Loxone weater service webinterface")


# Developer documentation #

Here you can find all dev related stuff I could find on the NET

## Small things to note ##

  * Time values are indicated in seconds since 1.1.2009 0 UTC clock. ( 1230768000 in epoch )
  * An integer has 32 bits, little-endian.
  * Outputs with printf will be written to the log window of Loxone Config.

## setweatherdata ##

    int setweatherdata(int type,unsigned int time,float value);



Type |Description                    | Extra info
-----|-------------------------------|---------
1    | Temerature                    |
2    | Dewpoint                      |
3    | Rel. Humidity                 |
4    | wind Speed                    |
5    | Wind Direction                |
6    | Gusts                         |
7    | Abs. Radiation                |
8    | Rel. Radiation                |
9    | Precipitaion                  |
10   | Weather Type                  |[See weather type for valid return codes](.#weather-type)
11   | Air Pressure                  |
12   | Custom 1                      |
13   | Custom 2                      |
14   | Custom 3                      |
15   | Custom 4                      |
16   | Custom 5                      |
17   | Custom 6                      |
18   | Custom 7                      |
19   | Custom 8                      |
20   | Custom 8                      |
21   | Custom 10                     |
22   | Time Weather Data             |
23   | Last weather forecast update  |
24   | Weather data error            |
25   | Error weather forecast update |
26   | Perceived Temperature         |
27   | Solar Radiation               |

### Weather type ##

Return value  | Desc                 |
------------- | -------------------  |
1             | No clouds            |
2             | Clear                |
3             | Scattered clouds     |
4             | Heavy cloud cover    |
5             | Overcast             |
6             | Fog                  |
7             | Havy fog             |
8             | N/A                  |
9             | N/A                  |
10            | Light Rain           |
11            | Rain                 |
12            | Heavy Rain           |
13            | Drizzle              |
14            | Sleet                |
15            | Heavy Freezing Rain  |
16            | Light Shower         |
17            | Heavy rain showers   |
18            | Thunderstorms        |
19            | Strong thunderstorms |
20            | Light snow           |
21            | Snow                 |
22            | Heavy snow           |
23            | Light snow showers   |
24            | heavy snow showers   |
25            | Light sleet          |
26            | sleet                |
27            | heavy sleet          |
28            | Light sleet showers  |
29            | heavy sleet showers  |

