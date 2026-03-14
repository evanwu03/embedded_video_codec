## Summary

This project is a software video codec written for bare metal microcontrollers such as the MSP432P410R. 
The MCU video player was designed to be as memory-efficient and require little computational overhead as possible, keeping in mind the limited memory (64kB RAM and 256kB flash) of the MSP432P4xx series MCUs. The LCD drivers and codec APIs were written to be as platform-independent as necessary, allowing portability to other microcontrollers. While there is much room for improvement, the proof of concept has been successfully accomplished, demonstrating how video compression algorithms can be deployed to embedded systems. 

**At the moment I am not currently developing this project** further due to school, but I may return in the future to refine existing implementations and test new video compression algorithms.

## Prerequisites 

* MSP432P410R Launchpad and SDK: https://www.ti.com/tool/download/SIMPLELINK-MSP432-SDK/3.40.01.02
* arm-none-eabi-gcc compiler 
* openocd 
* gdb-multiarch 
* makefile

## How To Run 


1. To compile the project using make: 
```
$ cd gcc 
$ make all
``` 

2.  Open an openocd session and run the following command 

``` 
$ openocd -f board/ti_msp432_launchpad.cfg 
```


3. Open an arm gdb session and connect to openocd server and load application
``` 
$ gdb-multiarch
$ (gdb) target remote :3333
$ (gdb) load <executable_name.out>
```

4. That's all! you can type `continue` or press physical reset button to continue programs


### Cortex Debug 

The .vscode/ directory contains a launch.json configuration file to be paired with the Cortex Debug Extension.
This can be used as an alternative option to opening a gdb server from the command line.


## Directory Structure 

```
.
└── msp432_sprite_renderer/
    ├── codec/
    │   ├── decoder.py
    │   ├── encoder.py
    │   ├── main.py
    │   ├── quantizer.py
    │   ├── video.py
    │   ├── tests/
    │   │   ├── test_encoder.py
    │   │   └── test_quantizer.py
    │   └── videos/ 
    ├── docs/
    │   └── dev_log.md
    |   └── software_docs.md
    ├── gcc/
    │   ├── makefile
    │   ├── msp432401r.lds
    │   ├── sprite_renderer.map
    │   └── startup_msp432p401r_gcc.c
    ├── hal/
    │   ├── include/
    │   │   ├── gpio.h
    │   │   ├── spi.h
    │   │   ├── uart.h
    │   │   └── wdt.h
    │   └── msp432/
    │       ├── gpio_msp432.c
    │       ├── msp432_regs.h
    │       ├── spi_msp432.c
    │       ├── uart_msp432.c
    │       └── wdt_msp432.c
    ├── include/
    │   ├── pixel_map.h
    │   ├── video_fsm.h
    │   └── video.h
    ├── LcdDriver/
    │   ├── hal_lcd.h
    │   ├── hal_lcd_msp432.c
    │   ├── lcd.h
    │   └── lcd_st7735.c
    ├── README.md
    ├── src/
    │   ├── main.c
    │   ├── system_msp432p401r.c
    │   ├── video.c
    │   ├── video_data.inc
    │   └── video_fsm.c
    ├── svd/
    │   └── MSP432P401R.svd
    └── tests/
        ├── AllTests.cpp
        ├── decoderTests.cpp
        ├── helper.cpp
        └── makefile
```

### Directory Description.

- `codec/` folder contains the python command line utility for compressing video into a readable format for the MSP432.

- `docs/` explains software architecture for the codec pipeline and decisions made along the way.

- `gcc` contains makefile, linker script, and startup code required to build programs for the microcontroller.

- `hal` includes custom drivers for gpio, SPI, UART, and watchdog timer functionality.

- `include` application-specific header program files.

- `src` holds main.c and application source files.

- `svd` svd file that describes hardware register and memory mapping of ARM Cortex-M MCUs that is used by debuggers (optional).

- `test` platform independent tests written with CPPUtest framework. 

