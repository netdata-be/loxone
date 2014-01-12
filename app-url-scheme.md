# Starting Loxone iOS/Android APPs using an URL #

iOS/Android Applications can be launched by simply clicking on a link that specifies an APP as target
using an URL rather than a website.

These features are supported by the Loxone iOS APPs in Versions 3.1 and above, except for simply starting or activating the App, this already is available
since 3.0.

Lets start of with an example of an URL pointing to the Loxone APP in the simplest of all
possible ways:

## Simple start or activate the APP ##

One of the few differences between starting the URL-Start of the iPhone and iPad-Version of
the Loxone APP is the scheme-Identifier of the APP. If you simple create a link like “loxone://”
you will be redirected to the Loxone iOS-APP, if it is installed. If the targeted App isn’t installed,
nothing is going to happen. `loxonehd://` on the other hand is going to start the iPad-Version.
*
Examples
    loxone://

Starts the Loxone iPhone APP

    loxonehd://

Starts the iPad-Version of the Loxone APP


## Connect to a specific target Miniserver ##
*
So simply starting the APP is pretty straight forward. But if you do not specify a target Miniserver
to connect to, it is always going to reconnect to the Miniserver that was connected when the
APP was closed/sent to background. If no Miniserver was connected, the APP is simply going to
pop up.

But we want to make sure the APP is going to connect to a specific target Miniserver. We have
to provide either the Local IP-Address or the Remote-URL. The APP will then look in it’s archive
if it already has the connection information needed to connect to this Miniserver or it has to
create a new entry and then makes a connection attempt.

If you’re sure that your APPs Archive already contains the address information for your target
Miniserver, you might aswell use its serial number to identify it.

Also if you want to, you can specify the username and optionally also the password in the link.
Usually you won’t provide the password in the link, since those links aren’t encrypted at all.
*
Examples:

    loxone{hd}://10.7.7.47:90

Starts the Loxone APP and connects to the given Miniserver. It will ask for credentials if
they are not already known by the App.

    loxone{hd}://my.dyndns.com:90

Does the same as above, but this one uses the remote address

    loxone{hd}://EEE0000000AA

In order for this to work, your app has to have the connection information in its archive.
Otherwise it won’t know how to connect to the Miniserver identified by this serial number.

    loxone{hd}://admin@{addr/ip/snr}

This link shows how you can define the username in the link already.

    loxone{hd}://adminassword@{addr/ip}

This link would directly connect you to the targeted Miniserver, regardless of any contact
information you’ve stored in your APP before.

## Navigate to a function overview ##

Specifying the targeted Miniserver is pretty neat already, but there is more. Beware, the
following steps require additional information of your Miniservers configuration, that is not visible
unless you know some things.

As you probably know, Blockfunctions and Functions are being visualized in different ways. All
regular Functions and some Blockfunctions are of the display-type “Cell only” (e.g. Heatmixer, regular Input) or “Inline” 
(e.g. Radiobutton) and do not provide the ability to navigate to a
seperate overview-screen. But there are others that do, those are of the type “Overview”, like
the Blinds, the intelligent Roomcontroller or the Alarm-Blockfunction.

The URL-Start allows you to specify one of those “Overview”-Blockfuncitons to be displayed
after the connection is established. To do so, you need to identify this Blockfunction explicitly -
which isn’t possible by just making use of the Config-Software alone.

As soon as you’ve stored your configuration to the Miniserver, there is an XML-File available on
it. We call it the `Structure-File`, since it contains (almost) all the information an App needs of a
configuration to build up it’s GUI. This “Structure-File” contains UUIDs (Unique Identifiers) that
are used to identify a Blockfunction or Function explicitly.

To aquire this file, use a browser and navigate to `http://{ip/url}/data/LoxAPP2.xml` - you will be asked
for your username and password. After successfully authenticating, you should be displayed the
*
`Structure-File`. This file contains "Function"-XML-Elements, that represent both Functions and
Blockfunctions (off all different display types). Now you need to look for the Blockfunction, who’s
overview you want to be displayed after opening the link.

Just hit ctrl+f and enter it’s name - don’t scroll through it all. Got it? Good. Now you can see
this Function-Element contains serveral attributes, the one we are interested in, is called
`UUIDaction`. Copy the desired Blockfunctions `UUIDAction`-Attribute value, it should be a 32
digit long mixture of letters and numbers, divided into 4 parts by the minuses (e.g.: 07991c82-
0104-01de-ffffeee0000000aa). This UUID identifies your Blockfunction explicitly and is
furthermore referred to as “UUID”.

Now we’ve got everything we need to know, so we’re able to construct an URL, that will not
only make the app connect to a specific target Miniserver, but also will lead to the Functions
Overview to appear after the connection was established. This way you don’t need to navigate
through the app to find the concerned Function after an alert or something.

Examples

    loxone{hd}://{ip/url/snr}/Function/{UUID}

This link, will open the app, establish a connection to the specified target Miniserver and
after that, the overview of the given Function is going to be displayed.



## Navigate to a room or category ##

Just like with navigating to a function, when you want to navigate to a room or category, you will
need to identify this `Group` (thats what we call both rooms and categories in general) explicitly.
All the rooms and categories are listed inside the `Structure-File`, for information on how to get
the `Structure-File”, please see “Navigate to a function overview”.

In the “Structure-File”, look up the “Cats”- or “Rooms”-XML-Element, their child-elements list the
rooms and categories that you are using in your visualisation. Each room or category has its
own number, ranging from 1 to n (= Nr. of all rooms or groups). So just pick out the room and/or
category you want to navigate to after establishing the connection.

When it comes to navigating through rooms and categories there is a slight difference between
the iPhone and the iPad-Application. On the iPad, you only have one subview for displaying
a “GroupList” (= list of rooms or categories) and the “FunctionList” (= content of a group
selected inside the GroupList) at the same time. Clicking groups inside this screen (either on
the GroupList or the FunctionList) will only change this screens content, but no other layers for
displaying groups (rooms or categories) will be displayed.
*
The iPhone Application on the other hand visualizes these parts in layers of views stacked onto
each other. Which means there is one screen-filling GroupList and/or one or two FunctionLists
stacked onto each other instead of being displayed next to each other like on the iPad. And just
*
like this, the URL to navigate to one of these is built up - the order of group and category does
matter.
*
Examples

    loxone://{url/ip/snr}/Room

Shows a list of all rooms after connecting.

    loxonehd://{url/ip/snr}/{Group}

Shows a list of all groups of type {Group} (= Room or Category) and the FunctionList of
the first group of this type.

    loxone://{url/ip/snr}/Category

Shows a list of all categories.

    loxonehd://{url/ip/snr}/Category

Shows a list of all categories and the FunctionList of the first category.

    loxone{hd}://{url/ip/snr}/{Group}/{GNr}

Directly navigates to the FunctionList of the given room or category.

    loxone://{url/ip/snr}/Room/{RNr}/Category/{CNr}

Two stacked FunctionLists, at first the FunctionList of the specified room is shown and
then the FunctionList of the category is shown on top of it.

    loxone://{url/ip/snr}/Category/{CNr}/Room/{RNr}

Same as above, only vice versa with group stacked onto the category.

## Combining the above ##
*
Using the info from the previous sections, you can now build up a variety of URL-Paths, that
directly take you to almost any screen in the app.
*
Examples

    loxone{hd}://{user}@{url/ip/snr}/Room/{RNr}/Category/{CNr}/Function/{uuid}

The above link is going to take you to the function overview of `{uuid}`. It is laid as a layer
above the FunctionList of the `{CNr}`-Category. On the iPhone the FunctionList of the
`{RNr}`-Room will be displayed below the `{CNr}`s FunctionList and above the overview.

