# chipio-simulator
![](https://imgur.com/Mc28sZP.png)

A simulator for the 8051 microcontroller onboard the CA0132 HDA codec that's
referred to as 'chipio'. This microcontroller is used for handling incoming
HDA verbs, setting up the DSP to handle audio streams, basically all codec
configuration tasks. This program allows you to go step by step through the
program to see what exactly is done.

I won't provide binaries for the program onboard the ca0132's 8051, but if
you have a ca0132, you can get a dump of your chips current status by running
the `ca0132-8051-dump-state` program from my [ca0132-tools](https://github.com/Conmanx360/ca0132-tools)
repo. You can then use this file by doing `./chipio-simulator -s <dump-state-save-file>`.

In order to build this, you'll need the ncurses development libraries.

I will attempt to add more documentation soon.
