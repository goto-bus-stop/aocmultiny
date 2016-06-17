# aocmultiny

Tiny NAT-traversing Age of Empires 2 Multiplayer client.

> VERY work in progress, it's probably pretty difficult to get it to compile. I
> symlink dependency `.lib`s into the `lib/` folder manually currently.

The idea is to get a single portable zero-install executable that you can hand
to a few friends and immediately start playing. No creating accounts, no
installation, no router configuration, etc. Just the bare minimum.

## Design

Fluctuating. I wanted to build the matchmaking part using IRC, but that's
turning out to be a bit difficult, because you would need server-side
adjustments to get IP addresses for the NAT traversal stuff (…I think.)

Anyway, it comes in two parts: matchmaking and connecting. Matchmaking will be
using a nickname (no real authentication), and using public or passworded rooms
(like IRC channels). Players can chat in rooms and the first player to join a
room (the host) can start the game. Initially, up to 8 players can join a room,
but in the future there'll have to be more for UserPatch spectator mode.
(≤8 players + ≤32 spectators.) Connecting is probably going to be using ICE
somehow (libnice?), but I don't know yet how to direct Age of Empires traffic
onto an ICE-established socket. All in due time :D

UserPatch includes automatic port forwarding using UPNP, but that doesn't always
cut it, and you still need to exchange IP addresses manually.

## Credits

 - IRC: Took clues (and an IRC message parser) from [Fredi/IRCClient]
 - DP: I used [@biegleux]'s Delphi DPLobbySystem as a reference for most of the
   DirectPlay lobby/game setup. I'm not sure if the sources are still online,
   but I'll link them here if I can find them.

[Fredi/IRCClient]: https://github.com/Fredi/IRCClient
[@biegleux]: https://github.com/biegleux
