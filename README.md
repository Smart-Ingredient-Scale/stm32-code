<h1 style="color: salmon"> <b>Load Cell</b>: STM32 Embedded C Code Repo </h1>

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
* load-cell
  master
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