# chipio-simulator

A simulator for the 8051 microcontroller onboard the CA0132 HDA codec that's
referred to as 'chipio'. This microcontroller is used for handling incoming
HDA verbs, setting up the DSP to handle audio streams, basically all codec
configuration tasks. This program allows you to go step by step through the
program to see what exactly is done.

I wrote the 8051 emulation myself, it's all entirely custom. I initially
called it `emu8051` but as the project went on I came to realize that that
name doesn't really accurately describe the project. Most of this was written
two years ago, so the programming quality inside varies. However, it is
entirely functional. :)

I won't provide binaries for the program onboard the ca0132's 8051, but if
you have a ca0132, you can get a dump of your chips current status by running
the `ca0132-8051-dump-state` program from my [ca0132-tools](https://github.com/Conmanx360/ca0132-tools)
repo. You can then use this file by doing `./chipio-simulator -s <dump-state-save-file>`.

In order to build this, you'll need the ncurses development libraries.

I intend on adding documentation for the SFR's and specific locations in the XRAM
memory at some point, but I haven't yet gotten around to it.

Below I will describe each of the custom 'windows' that exist, and what they do.

## Main Window:
![](https://imgur.com/Mc28sZP.png)

This is the main window of the simulator. By pressing tab, you can switch between
the three separate boxes, the main on the left being the instructions, the top
right being the IRAM, and the bottom right being the SFR's.

In the main instruction window, pressing enter or arrow key right steps forward
by one instruction. Pressing arrow key left reverses by one instruction. Arrow
key up steps forward by the step size defined in the settings window, and key
down reverses by the step size value defined in the settings window.

The IRAM and SFR windows use the arrow keys to navigate addresses, and by pressing
enter, you can modify their values.

To exit the program, hit `F1`.

## Settings Window:
![](https://imgur.com/tuHACu2.png)

The settings window is opened by pressing `f` in the main window. The various
fields are navigated with the arrow keys.

The step size field adjusts the step value for the up and down arrow keys in the
main instruction window.

The backlog size field adjusts how many reverse ops to keep before getting rid
of them. The default backlog size is 1000, meaning you can undo the last 1000
forward ops that have been done.

The breakpoints for PC and DPTR are toggled by hitting enter when their labels
are highlighted. If the condition for these breakpoints are met while using
the multi-step arrows in the main window, it will stop before reaching the
step count at the specific instruction that met the condition. The only
conditions currently available are PC address and dptr address.

To save the settings, hit `enter` on the `[OK]` button field. To discard and
close the window, hit `F1`.

## Save State Window:
![](https://imgur.com/4gKMTP1.png)

The save state window is opened by pressing the `s` button in the main window.
This window creates a save state file of the current state of the simulator in
the `save-states` folder.

The save state window has one field, which is the save state name entry field.
This defines what to name the save-state file to be created.

To create the save state, hit `enter` on the `[OK]` button field. To discard
and close the window, hit `F1`.

## Program Counter Input Window:
![](https://imgur.com/8oGPg4y.png)

The program counter input window is opened by pressing the `o` button in the
main window. This window allows you to change the current program counter.

The window has one field, which is the address to set the program counter to.

To set the program counter to this address, hit `enter` on the `[OK]` button
field. To discard and close the window, hit `F1`.

## XRAM Modify Window:
![](https://imgur.com/mLcixzT.png)

The XRAM modify window is opened by pressing the `e` button in the
main window. This window allows you to read and modify the contents of the
XRAM memory.

This window behaves the same as the IRAM and SFR boxes in the main windows,
in that you can navigate with the arrow keys, and hit enter to modify the value.
You can also use `page down` and `page up` to go up/down by an entire page of
values.

To close the window, hit `F1`.

## Verb Input Window:
![](https://imgur.com/wUKWIVZ.png)

The verb input window is opened by pressing the `v` button in the main window.
This allows you to run the specified HDA verb through the 8051's verb handling
interrupt. It behaves like a normal interrupt, pushing the current PC onto the
stack, and will return properly.

The three fields for `NID`, `Verb`, and `Val` allow you to set the HDA node ID,
the HDA verb, and the value for the verb.

If you toggle the `Run to interrupt exit.` switch to on, the program will
automatically run through the interrupt code and break on the RETI instruction.
This is useful when a particular verb isn't handled during the interrupt routine,
but is instead put into the main verb buffer for later handling.

To run the given verb, hit `enter` on the `[OK]` button field. To discard and close
the window, hit `F1`.

## Log File Setup Window:
![](https://imgur.com/bx8qs4C.png)

The log file setup window is opened by pressing the `l` button on the main window.
This window creates a log-file of a given name in the `log-files` directory. The
log file contains relevant information, such as SFR reads/writes, xram reads/writes,
function calls, and HIC Bus reads/writes. This format makes it much easier to parse
through what occured in a given sequence of instructions.

The first field is the file name for the log-file.

The `End on verb handler exit` toggle will have the log-file stop being written
to when the IRAM value at address `0x79` is `0x00`. This value represents the
amount of HDA verbs in the verb buffer that still need to be handled. This is
useful when you're trying to get a log of a given verb being run.

The `Create save file at start` toggle will create a save-file of the same name
as the log-file in the `save-states` directory. This is useful in the case of verb
tracing, because it allows you to run the simulator until the verb is done being
handled, open the save-file from the start of the log, and step to locations that
are specified in the log-file for further inspection.

To stop a currently running log, just hit the `l` button again from within the main
window.

My current workflow for checking out what a verb does is to use the verb input window
to set the verbs I want to see handled, then start a log with both of the toggle fields
enabled. This allows me to find important events in the log file, and inspect them by
opening the save file from when the logging started and stepping to step when they
occured.

To begin logging, hit `enter` on the `[OK]` button field. To discard and close
the window, hit `F1`.
