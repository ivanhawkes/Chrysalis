# ObjectId

We require a system for creating globally unique IDs which may be used as keys for storing information in databases. A 64 bit key is desireable as it provides both a large address space and is a built in type universally available.

By partitioning the bits available within a 64 bit value we can create a key generation scheme that is not reliant on database queries or locking semantics.

-   32 bits for the number of seconds since the unix epoch. This provides us with both a convenient timestamp for sorting and a smaller window for possible collisions of generated keys.
-   18 bits for instanceId (262,144). The instance Id will be a unique number for every running instance of the software within a given federation. If several instances are running on the same machine they will each require their own Id.
-   14 bits for a random component. Each second a new starting value will be produced randomly. ID requests will increment this value. Note: this restricts the number of IDs that can be produced in any given second for any given instance to 16,384. For this reason, you should avoid trying to generate large numbers of keys all at once.

One issue with using a system like this is the hard limit on the number of identifiers it can produce in a single second. Pushing past this limit either means duplicate IDs are produced, which is catastrophically bad, or the requesting code must be able to handle the possibility of being denied a request for an ID. Neither situation is ideal but there are ways to mitigate the issue.

The simplest solution is to have a separate generator for each type of ID you wish to produce e.g. accounts, characters and factions could all use their own generator since IDs only need to be unique within that data entity. Entities with low amounts of traffic could share a single generator.

Even high traffic entities which are required to be unique for all federations can be fairly easily supported e.g. loot items, since it is unlikely any single instance will need to generate more than a few thousand / second.

Instance IDs will need to be requested by anyone wishing to run an instance that is capable of being a part of the federation. These can be rented to end users, providing a convenient mechanism for making a small amount of money and incentivising users to not request more than they need or to horde them. It's a limited size pool, so it needs management.

Attempts to create an ID when the amount for that second is exhausted will need to return an invalid key e.g. 00000000H Clients should check for this log the error, or raise an alert.

One final thought, these IDs should only need to be unique within a federation, since moving items, characters and the like between federations requires a move operation in any case.