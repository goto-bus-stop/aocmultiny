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

## Building

On Linuxes:

 1. Install Wine and Mingw32. Something like
    `apt-get install wine mingw32 mingw32-binutils`.
 2. Download and build [wxWidgets] 3.1.0 using [MinGW][mingwbuild].
 3. Download the last [DirectX SDK with DirectPlay (August 2007)].
 4. `make` to build
 5. `make run` to run

On Windowses with Visual Studio:

 1. Download and build [wxWidgets] using [Visual Studio][vsbuild]. Import one of
    the VS project files that comes with wxWidgets and build it.
 2. Download the last [DirectX SDK with DirectPlay (August 2007)].
 3. Configure environment variables:
    - `WXWIN=` path to your wxWidgets build base directory.
    - `AOCMULTINY_DPLAY_SDK=` path to where you installed the DirectX SDK.
 4. Import the AoCMultiny.vcxproj project file.

## Credits

 - IRC: Took clues (and an IRC message parser) from [Fredi/IRCClient]
 - DirectPlay: I used [@biegleux]'s Delphi [DPLobbySystem] as a reference for
   most of the DirectPlay lobby/game setup. I'm not sure if the sources are
   still online, but I'll link them here if I can find them.

[Fredi/IRCClient]: https://github.com/Fredi/IRCClient
[@biegleux]: https://github.com/biegleux
[DPLobbySystem]: https://web.archive.org/web/20140519214627/https://code.google.com/p/dplobbysystem/
[wxWidgets]: https://wxwidgets.org/
[DirectX SDK with DirectPlay (August 2007)]: https://www.microsoft.com/en-us/download/details.aspx?id=13287
[mingwbuild]: https://wiki.wxwidgets.org/Cross-Compiling_Under_Linux#Cross-compiling_under_Linux_for_MS_Windows
[vsbuild]: https://wiki.wxwidgets.org/Microsoft_Visual_C%2B%2B_Guide
