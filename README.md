# DataFace
A Nerdy Pebble Time Watch Face

Screen Shot
---
![Screen Shot](resources/images/screenshot.png)

Features:
---
  - Terse, easy-to-read layout:
    - ISO Work Week Number, Abbreviated Day of Week
    - Date (YYYYMMDD)
    - 12 or 24 hour time (HH:MM:SS, respects watch settings)
    - Weather (Current Conditions abbreviated, Temperature)
    - Bluetooth Status (bt = connected, nc = not connected), Battery Status
  - Fetches the weather with your [OpenWeatherMap AppID](http://openweathermap.org/appid).
    Remember to sign up and put your AppID into the settings for the watch face.
  - Can display temperature in different units: Fahrenheit, Celcius, and Kelvin.
  - Color themes.
  - Doesn't drain your battery.

Planned Features:
---
  - Support for Other Weather APIs (Yahoo, wunderground, etc)
  - More pages/windows with moar data like pollution/haze, barametric pressure,
    latitude, longitude, altitude, etc.
  - Notification / Timeline indicator
  - AM/PM Indicator
  - Time Zone Indicator
  - CI through [Travis CI](https://travis-ci.org/), [travis-ci-pebble](https://github.com/spangborn/travis-ci-pebble)

Build Instructions
---
You'll need the [pebble-sdk](http://developer.getpebble.com/sdk/) installed.

The Makefile for the project streamlines the process of cleaning, building, installing, and tailing the device logs.

To install the watch face on an emulator:
  1. Run: `make`

To install the watch face directly on a watch:
  1. Make sure the developer connection is turned on and setup for the phone you want to install to.
  1. Tell the pebble tool where your phone is. You'll only have to do this once per new terminal session.
     Do either of the following:
    - Set the environment variable `PEBBLE_PHONE` to the IP address or hostname for your phone.
    - Set the environment variable `PEBBLE_CLOUDPEBBLE` and use `pebble login` to sign in.
  1. Run: `make`

Read the [Makefile](Makefile) for more info.

Contributing
---
I happily accept feature and pull requests.  I'll probably have comments and
suggest alterations to suit my style and chosen semantics.  It's not you, it's
code.  I don't get paid to work on this, so requests will be resolved as I have
the enthusiasm and time to do so.

