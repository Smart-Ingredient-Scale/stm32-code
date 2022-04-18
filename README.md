<h1 style="color: salmon"> <b>Touchscreen</b>: STM32 Embedded C Code Repo </h1>

## **Touchscreen TODO**
- [ ] Display code finalization
  - [x] Overwriting with smaller text, make sure background is always erased
  - [ ] Faster screen switching: "inverse" screen write, same screen but use background color parameter in function for original screen write
  - [ ] On screen switch: try turning off the backlight
  - [ ] Add home logo
  - [ ] Add check/confirm logo for volume calibration screen
  - [ ] Have a sleep timer that dims the backlight if no interaction has been recorded in a while
- [ ] Code cleanup
  - [x] Macro in screen.c for updating text color if currently selected
  - [ ] ifdef guards on .h files
  - [ ] Move LCD code into a folder
  - [x] Overview on screen.c and touchscreen.c
- [ ] Core functionality (prioritize!!)
  - [x] Switch to a different timer for touch debounce
  - [x] PWM backlight
  - [ ] Finalize units and update on display
- [x] "State machine" code
  - [x] Keep track of state when input volume
  - [x] Keep track of unit to display on 8-seg
  - [x] Keep track of current screen shown
- [ ] Testing
  - [ ] Add panic error screens for error checking

## **DONE**: Merge the `modified` project files into each new branch:
```
modified-project-files/
├── CMSIS
│   └── device
│       └── stm32f4xx.h      # Replaced the BSRR in GPIO_TypeDef
├── src
│   └── system_stm32f4xx.c   # Removed confusing empty braces (warning)
└── StdPeriph_Driver
    └── src
        ├── stm32f4xx_gpio.c # Updated the BSRRL and BSRRH references

5 directories, 3 files
```


## **Branch Summary**
```
$ git branch -a
  eeprom
  example0
  load-cell
*  master
 touchscreen

```

1. `master`: Contains the "master" code for the whole project
which will eventually contain the full project. Initialized
with similar contents to the `example0` code.

2. `example0`: Contains example code for the **NUCLEO F401RE** testing:
    * TIM2
    * TIM5
    * GPIO: PA0

3. `load-cell`: Contains the code for the load cell HX711 interface

4. `eeprom`: Contains the code for reading/writing data to/from EEPROM.

5. `touchscreen`: Ben's wonderful touchscreen interfacing nightmare 😴
  * Draws images to touchscreen
  * Uses ADC samples for the touchscreen
  * Implements UI
  * Implements touchscreen brightness modulation (PWM the NMOS)


## **Command Line or Git Bash Instructions**
Since we are creating separate branches for each of these projects
based around the skeleton master branch:

#### **List All Existing Branches**
```
git branch -a
```

#### **Changes Branches**: keep master contents plus contents of `<existing-branch-name>`
```
git checkout <existing-branch-name>
```

#### **Create New Branch**: create new branch called `<new-branch>`
```
git branch <new-branch>
```