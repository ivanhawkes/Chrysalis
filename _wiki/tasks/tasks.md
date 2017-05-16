# Ivan's Tasks

## Content Management

* Work out a system of version management
* Consider how to make / use templates of items, missions, skills

## Database

* Generic routines that take the work out of CRUD operations
* Test out how collections of components go when serialised to Go / Datastore

## Game Engine

* Find a way to make it handle live changes to the underlying model (items, missions, etc). Monitoring an RSS feed might work well here as it can also be used to feed information to the players about upcoming events and the like (or use a few feeds for this). Feed can be proxied to keep hits to the database nice and low. Should all clients be monitoring the feed? Maybe allow them to hit it once every few (4-12) hours. Upcoming events would be published and able to notify at least an hour or two in advance, and most likely 1+ days - so this should work.

* Investigate value of packaging all the database 'static' data (items, missions, etc) into a .pak file and distribute this on a regular basis e.g. CloudshatterDb.pak - contains all the fairly static data for the whole Cloudshatter realm. Datestamps allow fast checks against the DB to work out what needs to be pulled in over the wire. The way the .pak file system works would allow us to send out packed updates daily if we wanted, then a weekly repack (and delete all the dailies). Need to balance reduction in traffic vs time to jump into game each day.
