![](./resources/official_armmbed_example_badge.png)
# Getting started with bootloader on mbed OS

This example shows how to create a bootloader.

To read more about the bootloader, please visit the [bootloader tutorial](https://os.mbed.com/docs/latest/tutorials/bootloader.html).

## Required hardware

* A supported target - [u-blox EVK-ODIN-W2](https://os.mbed.com/platforms/ublox-EVK-ODIN-W2/), [Nucleo F429ZI](https://os.mbed.com/platforms/ST-Nucleo-F429ZI/) or [K64F](https://os.mbed.com/platforms/FRDM-K64F/).
* SD card.
* If your supported target does not have an SD card port, you need a shield or breakout board with an SD card port that uses SPI pins.

## Import the example application

From the command-line, import the example:

```
mbed import mbed-os-example-bootloader
cd mbed-os-example-bootloader
```

## Connecting the SD card

This example uses the **onboard SD card port** for the K64F and the u-blox EVK-ODIN-W2.

Other targets use the following Arduino form-factor SPI pins by default:

- `D11` - `MOSI`
- `D12` - `MISO`
- `D13` - `SCK`
- `D10` - `CS`

To use different SPI pins, you need to modify the `mbed_app.json` file. Add a key for your target to the `target_overrides` section with the following data (replace `<TARGET_NAME>` with your target's name):

```
{
    ...
    "target_overrides": {
        ...
        "<TARGET_NAME>": {
            "sd_card_mosi": "<MOSI_PIN>",
            "sd_card_miso": "<MISO_PIN>",
            "sd_card_sck": "<SCK_PIN>",
            "sd_card_cs": "<CS_PIN>"
        }
        ...
    }
    ...
}
```

## Preparing the bootloader

All supported targets mentioned above are set up to build as a bootloader image. To add support for a new target, you must specify the size of the bootloader.

To do this, set the target value `restrict_size` to the maximum bootloader size in `mbed_app.json` (replace `<TARGET_NAME>` with your target name):

```
    "target_overrides": {
        ...
        "<TARGET_NAME>": {
            "target.restrict_size": "0x20000"
        },
        ...
```

<span class="tips">**Note:** `restrict_size` pads the build image to the requested size.</span>

## Building the bootloader

Invoke `mbed compile`, and specify the name of your target and your favorite toolchain (`GCC_ARM`, `ARM`, `IAR`). For example, for the ARM Compiler:

```
mbed compile -m <TARGET_NAME> -t ARM
```

Your PC may take a few minutes to compile your code. At the end, you see the following result:

```
Merging Regions:
  Filling region application with .\BUILD\<TARGET_NAME>\ARM\mbed-os-example-bootloader_application.bin
  Padding region application with 0x11420 bytes
Space used after regions merged: 0x20000
+-----------------------------+-------+-------+-------+
| Module                      | .text | .data |  .bss |
+-----------------------------+-------+-------+-------+
| Misc                        | 18481 |    24 |  1268 |
| drivers                     |  2132 |     8 |   184 |
| features/FEATURE_LWIP       |    46 |     0 | 12560 |
| features/filesystem         | 12756 |    12 |   540 |
| hal                         |   414 |     8 |     0 |
| hal/TARGET_FLASH_CMSIS_ALGO |   520 |    28 |     0 |
| platform                    |  2759 |    32 |   380 |
| rtos                        |   144 |     8 |     0 |
| rtos/rtx                    |  6954 |   100 |  8396 |
| targets/TARGET_STM          | 15560 |   568 |   736 |
| Subtotals                   | 59766 |   788 | 24064 |
+-----------------------------+-------+-------+-------+
Allocated Heap: unknown
Allocated Stack: unknown
Total Static RAM memory (data + bss): 24852 bytes
Total RAM memory (data + bss + heap + stack): 24852 bytes
Total Flash memory (text + data + misc): 60554 bytes

Image: .\BUILD\<TARGET_NAME>\ARM\mbed-os-example-bootloader.bin
```

After the build completes, the build system creates two binary images in the `BUILD/<TARGET_NAME>/<TOOLCHAIN_NAME>` directory: `<project-name>.bin` and `<project-name>_application.bin`. `<project-name>.bin` is the padded bootloader (used for combining with applications). `<project-name>_application.bin` is the original unpadded bootloader.

In this example, `main.cpp` defines the update binary for the bootloader as `mbed-os-example-blinky_application.bin` (specified under the config `update_file` in `mbed_app.json`). The bootloader looks for this file in the root of the SD card, flashes it to memory and then jumps to the application.

## Building the application with the bootloader

The next step is to build an application you can combine with your bootloader to create a loadable image.

<span class="tips">**Note:** Updating an application to use a bootloader currently does not work with the Arm Mbed Online Compiler because .bin and .hex files can't be uploaded.</span>

1. Instruct the build system to use the newly created padded bootloader image (named `<project-name>.bin` in the previous section). To do this, create a `target_overrides` section in your application's `mbed_app.json` file, and add a key for your target (see `<TARGET_NAME>` in the example below). If `mbed_app.json` does not already exist in your application, create one.

2. Set the target value `bootloader_img` in `mbed_app.json` to the file path of the padded bootloader image. For the below example, copy the padded bootloader image to the project directory.

```
{
    "target_overrides": {
        ...
        "<TARGET_NAME>": {
            "target.bootloader_img": "<path to padded bootloader binary>"
        },
        ...
}
```

2. Invoke `mbed compile`, and specify the name of your target and the toolchain you are using (`GCC_ARM`, `ARM`, `IAR`). For example, for the ARM Compiler:

    ```
    mbed compile -m <TARGET_NAME> -t ARM
    ```

After the build completes, the build system creates two binary images in the `BUILD/<TARGET_NAME>/<TOOLCHAIN_NAME>` directory: `<project-name>.bin` and `<project-name>_application.bin`. `<project-name>.bin` is the combined bootloader and application (suitable for flashing to the device's ROM directly). `<project-name>_application.bin` is the update binary (the one you place on the SD card). By default the bootloader built in the previous step looks for an application built from the project `from mbed-os-example-blinky` and having the name `mbed-os-example-blinky_application.bin`.

### Program the application with the bootloader included

Follow these steps to program the target with the combined bootloader and application image. You then can update the application from the SD card.

1. Connect your Mbed device to the computer over USB.
1. Copy the combined bootloader and application binary file (named `<project-name>.bin` in the previous section) to the Mbed device.
1. Press the reset button to start the program.

### Program the application from the bootloader (load from SD card)

<span class="tips">**Note:** You must first flash the combined bootloader and application image or just the bootloader image (padded or unpadded) before proceeding with the following steps.</span>

1. Connect the SD card to your computer.
1. Copy the update application binary (named `<project-name>_application.bin` in the previous section) to the root of the SD card.
1. Remove the SD card from your PC, and connect it to your Mbed board (see the above section [Connecting the SD card](#connecting-the-sd-card)).
1. Press the reset button to start the firmware update.

If a terminal is open, the following prints at a baud rate of 9600:

```
Firmware update found
Starting application
```

## Troubleshooting

If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.


## License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see [contributing.md](CONTRIBUTING.md) for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.

