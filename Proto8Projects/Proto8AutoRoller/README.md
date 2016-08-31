# Template multi-knob sketch for deployment of proto pedal sketches

The demo expects:

* 1x16 button row at matrix rows 1-2 and cols 1-8
* Corrisponding 16 LEDs at 1-16
* System knob at 64
* HP calculator 9 digit bubble display at 48-64

In P8Interface.h, set p8ParamActive for buttons which correlate to parameter positions.
In use, active params will flash.

LEDs mean:

* Off: Parameter position not active
* On: Parameter active but not selected
* Flashing: Parameter active and knob synced to it
* Fast Flashing: Parameter active but not synced.  Sweep knob until synced.

**NOTE:** Flashing in position 16 indicated non-active parameter selected.  Or, idle.

Use p8Param[] to get data from "knobs".  It will be updated at knob change.
