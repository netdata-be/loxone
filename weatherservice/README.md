# Adding a weather service

You can add a weather service by opening the loxplan file with notepad
And insert XML tags below after this line:

    <Control V="63" UUID="4cbc6d88-93ce-11e2-bbe885f9504979e4" Title="Server communicatie" Type="WeatherCaption"/>

So here's the tag to insert:

    <Control V="63" UUID="6b028449-8152-11e1-b36ada6017029158" Title="myweather" Type="WeatherServer" Color="0,0,0" SType="1" Address="" Program="" VisuUUID="d83b0000-8727-11e1-bd1df582fe1a4aa2">
    <IoData Visu="true" UserGroupRef="7e6c7840-74eb-11e1-9520fbc337c08ef8" UserGroupRefRemote="7e6c7840-74eb-11e1-9520fbc337c08ef8">
    <LoggerMailer V="20" MinimumTime="20"/>
    </IoData>
    </Control>

Then save the file and open in loxplan. You will see myweather in the server communication section.
You can add code there and if you do you can call setweatherdata.
I have it up and running now for a couple of months and works quite ok.

Beware of
  1 memory leaks though, and keep a spare micro SD at hand in case you mess with the code a lot.
  2 The UUID and visuUUID and usergroupref work perfectly in my setup and I guess these are defaults. However if they are different in your loxplan I cannot predict what happens. Create a backup of your own config before playing with my config.

Note: once you have it in the loxplan it will port through all updates etc.

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
10   | Weather Type                  |[See weather type for valid return codes](.#Weathertype)
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

Return value | Desc
-------------|-------------------
1            | No clouds
2            | Clear
3            | Scattered clouds
4            | Heavy cloud cover
5            | Cloudy
6            | Fog
7            | Overcast
8            | N/A
9            | N/A
10           | Light Rain
11           | Rain
12           | Heavy Rain
13           | Drizzle
14           | Sleet
15           | Heavy Freezing Rain
16           | Light Shower
17           | Heavy rain showers
18           | Thunderstorms
19           | Strong thunderstorms
20           | Light snow
21           | Snow
22           | Heavy snow
23           | Light snow showers
24           | heavy snow showers
25           | Light sleet
26           | sleet
27           |  heavy sleet
28           | Light sleet showers
29           | heavy sleet showers

