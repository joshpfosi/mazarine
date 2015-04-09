# Final Swarmbot Software Design

## FSM

### States

* ON
* GO

#### ON State

Infinite loop which reads and sets a boolean reflecting which bot (1 or 2) our bot is. The loop is broken only by a `HIGH` input on pin `GO_SWITCH` causing a transition to `GO`.

#### GO State

Sequential code to run through the task, making bot specific decisions based on boolean.

## BOT 1

* Puts bot in motion (`forward()`)
* Loops until collision (boolean is set in ISR)
* Assumes it hit the correct wall and turns a predetermined angle to the left
* Moves forward 
* Stops on red, flashing a red LED
* Turns a predetermined angle to the right, and follows red
* Stops on yellow, flashing yellow LED twice
* Moves forward
* Follows red
* Stops on yellow, turns on yellow LED, turns 180 degrees
* Moves forward until red
* Follows red
* Stops on yellow
* Communicates to Bot 2: `START`
* Waits for `ACK_START`
* Flashes green LED
* Moves forward until red
* Follows red
* TBD // we want this to be close to a specific location
* Turns on green LED
* Waits for `TOXIC`
* Flash yellow LED continuously
* Waits for `STOP_YELLOW`
* Turns off yellow LED
* Waits for `DONE`
* Flashes green LED

## BOT 2

* Waits for `START`
* Flash green LED
* Puts bot in motion (`forward()`)
* Loops until collision (boolean is set in ISR)
* Assumes it hit the correct wall and turns a predetermined angle to the left
* Moves forward 
* Stops on blue, flashing a blue LED
* Turns a predetermined angle to the right, and follows blue
* Stops on yellow, flashing yellow LED twice
* Moves forward
* Follows blue
* Stops on yellow, turns on yellow LED, turns 180 degrees
* Communicates to Bot 1: `TOXIC`
* Flash yellow continuously
* Moves forward until blue
* Follows blue
* Stops on yellow
* Communicates to Bot 1: `STOP_YELLOW`
* Stop flashing yellow LED
* Moves forward until blue
* Follows blue
* TBD // we want this to be close to a specific location
* Communicates to Bot 1: `DONE`
* Flashes green LED

## Communication Protocol

* `START`:
* `ACK_START`: 
* `TOXIC`:
* `YELLOW_LED`:
* `DONE`:

## Pin Mapping

See `pinmap.h` for definitions.
