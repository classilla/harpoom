# Harpoom: Doom for the Apple Network Server

[Another Old Vintage Computing Research Offense to Humanity!](https://oldvcr.blogspot.com/2025/05/harpoom-of-course-apple-network-server.html)

Copyright 1993-1996 id Software and Successors.  
Copyright 2005-2014 Simon Howard.  
Copyright 2017 Maxime Vincent.  
Copyright 2025 Cameron Kaiser.  
All rights reserved. Other contributors acknowledged.  
GNU General Public License v2.

## What it is

It's the equivalent of Doom 1.9 running on the console (or remote X, your
choice) of any Apple Network Server running AIX 4.1.5 in CDE, letting you use
a $10,000 Apple server to play games and goof up production. It is derived
from [Doom Generic](https://github.com/ozkl/doomgeneric). It should support
most WADs compatible with vanilla Doom, including the shareware WAD, the
registered WAD, Thy Flesh Consumed, Doom II and Final Doom. Ultimedia is not
required, just the basic operating system and CDE as provided by a standard
install of "Harpoon" AIX for the ANS, which is where the name comes from.

Although it should run fine on other AIX 4.1.5 systems, they may
behave differently (patches accepted as long as they don't regress the ANS).

[Read more about the port](https://oldvcr.blogspot.com/2025/05/harpoom-of-course-apple-network-server.html).

## What it isn't

This is not the NCommander port of [Chocolate Doom for AIX 4.3](https://github.com/NCommander/aix_doom_things),
which won't even run on the ANS.

## What's different about it

  * No sound (ANS AWACS audio is not supported by AIX except "quacking")
  * Remote X mode (slower, requires 24-bit depth visual)
  * Console 256-colour mode (faster, requires 8-bit depth PseudoColor visual)
  * Strafe with either Alt or, on the ANS console with an ADB keyboard, Command keys (Command keys preferred since CDE doesn't intercept them)
  * A couple silly AIX-specific easter eggs

## How to run

Select any of the binaries and start it. The debug versions have symbols available which I will ask you for if you run into crashes. By default the game will look for WADs in the same directory as the executable, and store saved games in a directory `./.savegame`. These and other options, if set in `default.cfg`, are also honoured.

If you are running the remote X version, make sure your `$DISPLAY` variable is set correctly before starting the game. You might get an error message, but it might also just bomb.

## How to build

  * Install at least `egcs`/`gcc` 2.91. `xlc` is not currently supported but I'd like to. Send patches if you get it to work. Also install at least GNU `make` 3.81.
    * For debugging, also install `gdb` 5.3 or later. You can use `adb` for basic stuff but it probably won't fully grok the symbols these compilers generate.
    * If you need binaries, you can get pre-built versions from the [Floodgap Gopher server](gopher://gopher.floodgap.com/1/archive/ans-aixpdslib-aix-4).
    * It's recommended you symlink `gmake` to the new GNU make in `/usr/local/bin/make` so that you know which one you're using. I'll assume you did that. You're welcome.
  * Copy either `Makefile.aix` (remote X version) or `Makefile.aix256` (256-colour console version) to `Makefile` and then run a `gmake`. The resulting binary is called `doomgeneric` and by default is built with `gdb`-compatible symbols at `-O3`.

## Other notes

Although I've not removed the other ports that Doom Generic supports from this forkish thing, they probably don't work anymore either.

## License

GNU GPL v2, like the O.G. stuff.
