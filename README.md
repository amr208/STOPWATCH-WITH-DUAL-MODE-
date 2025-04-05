# ATmega32 Stopwatch Project

## Description
This project implements a digital stopwatch using an ATmega32 microcontroller. The system features both count-up and count-down functionality with time adjustment capabilities, displayed on multiplexed 7-segment displays. The stopwatch is controlled through external interrupts and includes a buzzer alarm feature.

## Features
- **Count Up Mode**: Standard stopwatch functionality counting upwards
- **Count Down Mode**: Timer functionality counting down to zero
- **Time Adjustment**: Ability to set hours, minutes, and seconds
- **Buzzer Alarm**: Sounds when countdown reaches zero
- **Pause/Resume**: Control the stopwatch operation
- **Reset**: Reset all values to zero
- **Visual Indicators**: LEDs show current mode (count up/down)

## Hardware Configuration
- **Microcontroller**: ATmega32
- **Display**: 6-digit multiplexed 7-segment display
- **Inputs**: 
  - Mode selection button (PB7)
  - Time adjustment buttons (PB0-PB6)
  - Reset (INT0), Pause (INT1), Resume (INT2) buttons
- **Outputs**: 
  - Buzzer (PD0)
  - Mode indicator LEDs (PD4, PD5)

## Software Architecture
### Main Components
1. **Timer1**: Configured in CTC mode for 1-second interrupts
2. **External Interrupts**: 
   - INT0 (Reset)
   - INT1 (Pause)
   - INT2 (Resume)
3. **Display Multiplexing**: Rapidly cycles through digits to create persistence of vision

### Key Variables
- Timekeeping variables for each digit (hours, minutes, seconds)
- Mode flags (count up/down, pause/resume)
- Button debouncing through software delays

## Functions
1. **`Atmega32_pins()`**: Configures I/O pins
2. **`Enable_Up_Down()`**: Handles 7-segment display multiplexing
3. **`Mode_Up_Down()`**: Toggles between count up and count down modes
4. **`Digit_increment()`/`Digit_decrement()`**: Logic for time counting
5. **`C_Down_Adjustments()`**: Time setting in countdown mode
6. **`Buzzer_Check()`**: Manages buzzer activation
7. **`Timer1_Stop_Watch()`**: Configures Timer1 for 1-second interrupts
8. **`Ex_Interrupts()`**: Configures external interrupts

## Interrupt Service Routines
1. **`TIMER1_COMPA_vect`**: 1-second timebase interrupt
2. **`INT0_vect`**: Reset handler
3. **`INT1_vect`**: Pause handler
4. **`INT2_vect`**: Resume handler

## Usage Instructions
1. **Mode Selection**: Press PB7 to toggle between count up and count down
2. **Time Adjustment** (in countdown mode when paused):
   - PB1: Increment hours
   - PB4: Increment minutes
   - PB6: Increment seconds
   - PB0: Decrement hours
   - PB3: Decrement minutes
   - PB5: Decrement seconds
3. **Control**:
   - Reset button: Reset all values to zero
   - Pause button: Stop the timer
   - Resume button: Continue counting

## Technical Notes
- Timer1 is configured with a prescaler of 1024 and OCR1A value of 15624 for 1-second interrupts at 16MHz
- Display multiplexing uses PORT A for digit selection and PORT C for segment control
- The system implements roll-over handling for all time units (seconds, minutes, hours)

## Build Requirements
- AVR GCC compiler
- AVRDUDE for programming
- ATmega32 microcontroller
- Supporting hardware (7-segment displays, buttons, buzzer)

## Example Circuit
(Note: Include schematic diagram here if available)

## Limitations
- Time adjustment only available in countdown mode when paused
- No persistent storage of timer values
- Basic button debouncing may need improvement for noisy environments
