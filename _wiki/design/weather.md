# Weather

Each day within the game world we will create a weather prediction based on reasonable weather given time of year and other available factors. The intention is to begin with a very simple model which may perhaps be expanded at a later time.

## Weather Forecast

The weather forecast will attempt to provide us with a baseline of what sort of weather to expect for the day. It is in line with what you might expect to hear from the 6 o'clock news.

The prediction will just be a broad outline of the expected weather for that day. This outline will then be used to provide dynamic weather updates as the day progresses.

* Temperature
** Minimum (celsius)
** Maximum (celsius)
** Average (celsius)
* Humidity (%)
* Rain (mm)
* Pollution
* Fog
* Conditions
** Sunny
** Mostly Sunny
** Partly Cloudy
** Mostly Cloudy
** Overcast
* Storms
** Thunder
** Lightning
** Hail
* Snow
* Sleet
* Special 
** Tornado
** Hurricane
** Sand Storm

## Weather Update

Every so often a weather update will be issued. Systems will need to be in place to migrate from the present weather to the one within the update. An update will provide information along the following lines:

* Temperature (Celsius)
* Humidity (%)
* Cloud Cover
** Sunny
** Mostly Sunny
** Partly Cloudy
** Mostly Cloudy
** Overcast
* Rain
** None
** Light
** Medium
** Heavy
* Pollution
** Density
* Fog
** Density
* Hail
** None
** Small
** Medium
** Large
* Snow
** None
** Light
** Medium
** Heavy
* Sleet
** None
** Light
** Medium
** Heavy
* Storms
** Thunder
*** Light
*** Medium
*** Heavy
** Lightning
*** Light
*** Medium
*** Heavy
* Special 
** Tornado
** Hurricane
** Sand Storm

## XML File Format

### Structure

Time of day may affect the transitions table for updates. The following special identifiers are recognised:

* dawn
* morning
* midday
* afternoon
* dusk
* evening
* night
* midnight
* default

This will allow you to tailor the weather to the rough time of day. Any entry in a given time slot will overwrite the corresponding entry in the default slot, or add one if there is no entry.

For instance, to encourage late afternoon storms simply add an entry for the afternoon where you provide higher probabilities of transitioning to heavy rain or storms e.g.

 <nowiki>
<afternoon>
  <new-state new-state-id="6" probability="1.4"/>
  <new-state new-state-id="7" probability="2.0"/>
</afternoon>
<default>
  <new-state new-state-id="5" probability="1.0"/>
  <new-state new-state-id="6" probability="1.0"/>
  <new-state new-state-id="7" probability="1.0"/>
</default>
 </nowiki>
