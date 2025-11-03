# Buzzerino

This is the repository of the soldering kit by the Digital Technology Group at the University of Kassel.

![logo](https://github.com/EmilAltmann/Buzzerino/blob/main/images/DigiTec_logo.png "logo")

The contents of this document:
* [Soldering Instructions](#soldering-instructions)
* [Flashing](#flashing)
* [Example Programs](#example-programs)
* [How to Code](#how-to-code)
* [Parts and Wiki](#parts-and-wiki)

## Soldering Instructions

![Parts of the Buzzerino.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/overview_without_pcb.jpg "Parts of the Buzzerino.")

### Bill of Materials
Please check if you have all the necessary components listed below:
* 1x Buzzerino circuit board (Buzzerino PCB)
* 1x Raspberry Pi Pico
* 1x 1602A LCD display
* 1x AHT10 digital temperature and humidity sensor
* 1x USB-C breakout board
* 1x 10 mm RGB LED
* 1x buzzer
* 1x analogue joystick
* 5x 6 mm push buttons
* 1x 100 kΩ NTC temperature-dependent resistor
* 5x 330 Ω resistors
* 1x 1 kΩ resistor
* 1x 10 kΩ resistor
* 1x 120 kΩ resistor
* 2x 1x20 pin headers and sockets (keep them plugged together)
* 1x 1x16 pin header
* 2x 1x4 pin headers
* 1x 1x3 pin header
* 1x 2x16 pin header
* 1x 2x7 pin header
* 1x 2x3 pin header
* 26x jumper

### Assembly of the Raspberry Pi Pico

![Raspberry Pi Pico and pin strips.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_1.jpg "Raspberry Pi Pico and pin strips.")

We will start by soldering the two 1x20 pin headers and sockets to the Raspberry Pi Pico board. The pin strips are each two parts plugged together; keep them together, as this will keep the contacts aligned! The side with the slimmer plastic piece will be soldered to the Raspberry Pi Pico (in the picture above, the inner one).

Start by putting the pin headers into the Raspberry Pi Pico and the Buzzerino PCB. The Raspberry Pi Pico should sit flat and parallel on both pin strips.

![Raspberry Pi Pico sitting loosely on pin strips.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_2.jpg "Raspberry Pi Pico sitting loosely on pin strips.")

Now we can solder our first connections. Heat one of the corner pins with your soldering iron. Check with solder whether the joint is up to temperature by gently feeding it into the heated pin. If the solder melts, you can remove the soldering iron and let the connection cool down.

![Example of soldering a pin from the Raspberry Pi Pico.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_3.jpg "Example of soldering a pin from the Raspberry Pi Pico.")

Continue by soldering the three other corners.

![Raspberry Pi Pico with four soldered corners.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_4.jpg "Raspberry Pi Pico with four soldered corners.")

Now you can remove the Raspberry Pi Pico from the Buzzerino PCB (with the pin headers still together).

![Raspberry Pi Pico with four soldered corners.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_5.jpg "Raspberry Pi Pico with four soldered corners.")

With the Raspberry Pi Pico now free, but the pin strips still attached to it, you can fully solder both sides.

![Fully soldered Raspberry Pi Pico.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_6.jpg "Fully soldered Raspberry Pi Pico.")

### Assembly of the USB-C breakout board

![USB-C breakout board and 1x4 pin header.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_7.jpg "USB-C breakout board and 1x4 pin header.")

Now we will solder the 1x4 pin header to the USB-C breakout board. Because both pieces are very small, it's a good tactic to plug them together and place them on your table.

![USB-C breakout board and 1x4 pin header loosely plugged together.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_8.jpg "USB-C breakout board and 1x4 pin header loosely plugged together.")

Start by soldering only one pin, as the angle between the board and the pin header will likely be slightly crooked.

![One pin of the USB-C breakout board soldered.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_9.jpg "One pin of the USB-C breakout board soldered.")

![The pin header on the USB-C breakout board will be crooked.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_10.jpg "The pin header on the USB-C breakout board will be crooked.")

To correct the angle of the pin header, hold both pieces together and carefully reheat the solder joint. Now you can correct any misalignment. Once the pins are perpendicular to the board, solder all four pins.

![The pin header on the USB-C breakout board fully soldered.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_11.jpg "The pin header on the USB-C breakout board fully soldered.")

### Assembly of the AHT10 digital temperature sensor

![AHT10 and pin header side by side.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_12.jpg "AHT10 and pin header side by side.")

The assembly of the AHT10 digital temperature sensor is identical to that of the USB-C breakout board. Start by soldering only one pin and correct the alignment so it is nice and perpendicular.

![AHT10 fully soldered.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_13.jpg "AHT10 fully soldered.")

### Assembly of the 1602A LCD display

![1602A LCD and pin header side by side.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_14.jpg "1602A LCD and pin header side by side.")

The assembly of the 1602A LCD display is very similar to the previous two steps. First solder one pin, check for squareness and correct, then solder all pins.

![1602A LCD and pin header soldered by one pin.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_15.jpg "1602A LCD and pin header soldered by one pin.")

![1602A LCD and pin header fully soldered.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_16.jpg "1602A LCD and pin header fully soldered.")

### Installation of resistors
#### 100 kΩ NTC temperature-dependent resistor

The 100 kΩ NTC temperature-dependent resistor has a glass body with no markings.

![Buzzerino PCB and NTC Resistor side by side.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_17.jpg "Buzzerino PCB and NTC Resistor side by side.")

The resistors come with straight leads and need to be formed. For that purpose hold them with one hand between two fingers and pull them to a U-shape with your other hand as seen in the following pictures:

![Forming of the resistor leads by hands 1.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_18.jpg "Forming of the resistor leads by hands 1.")

![Forming of the resistor leads by hands 2.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_19.jpg "Forming of the resistor leads by hands 2.")

![Forming of the resistor leads by hands 3.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_20.jpg "Forming of the resistor leads by hands 3.")

![Fully formed resistor leads.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_21.jpg "Fully formed resistor leads.")

![The NTC position on the PCB.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_22.jpg "The NTC position on the PCB.")

The newly formed resistor can now be inserted into the Buzzerino PCB at the position marked TH_NTC.
**Important:** The printed side of the Buzzerino PCB is the front of the PCB. The lower left corner should have our DIGETEC logo!

![The NTC with slightly bent leads.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_23.jpg "The NTC with slightly bent leads.")

As a tip for soldering beginners, you can slightly bend the leads outwards after fully seating the resistor. This will help keep it in place.

![The NTC soldered by one leg.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_24.jpg "The NTC soldered by one leg.")

Like the previous solder joints, it's good practice to start by soldering one lead first, then correct the positioning, and finally solder both.

![The fully soldered NTC.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_25.jpg "The fully soldered NTC.")

![The NTC with clipped soldering joints.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_26.jpg "The NTC with clipped soldering joints.")

After the resistor is fully seated, you can clip the leads flush with side cutters.

#### 1 kΩ resistor

The 1 kΩ resistor has stripes as markings (Brown | Black | Red | | Gold).

![The 1 kΩ resistor.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_27.jpg "The 1 kΩ resistor.")

![The 1 kΩ resistor formed.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_28.jpg "The 1 kΩ resistor formed.")

The installation of the different resistors is essentially identical. For the 1 kΩ resistor, start by forming it roughly U-shaped.

![The 1 kΩ resistor position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_29.jpg "The 1 kΩ resistor position.")

Then place it in its position, solder one leg, correct the placement, solder completely, and clip the leads flush.

#### 10 kΩ resistor

The 10 kΩ resistor has stripes as markings (Brown | Black | Orange | | Gold).

![The 10 kΩ resistor.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_30.jpg "The 10 kΩ resistor.")

![The 10 kΩ resistor formed.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_31.jpg "The 10 kΩ resistor formed.")

![The 10 kΩ resistor position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_32.jpg "The 10 kΩ resistor position.")

For the 10 kΩ resistor: form, place, solder one leg, correct, solder fully, and clip flush.

#### 330 Ω resistors

The 330 Ω resistors hve stripes as markings (Orange | Orange | Black | Black | Brown).

![The 330 Ω resistors.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_33.jpg "The 330 Ω resistors.")

![The 330 Ω resistors formed.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_34.jpg "The 330 Ω resistors formed.")

![The first 330 Ω resistor position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_35.jpg "The first 330 Ω resistor position.")

![The further 330 Ω resistor positions.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_36.jpg "The further 330 Ω resistor positions.")

For the 330 Ω resistors: form, place, solder one leg, correct, solder fully, and clip flush.

#### 120 kΩ resistor

The 120 kΩ resistor has stripes as markings (Brown | Red | Yellow | | Gold).

![The 120 kΩ resistor.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_37.jpg "The 120 kΩ resistor.")

![The 120 kΩ resistor formed.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_38.jpg "The 120 kΩ resistor formed.")

![The 120 kΩ resistor position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_39.jpg "The 120 kΩ resistor position.")

For the 120 kΩ resistor: form, place, solder one leg, correct, solder fully, and clip flush.

### Installation of the buttons

![The five push buttons.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_40.jpg "The five push buttons.")

For the assembly of the Buzzerino PCB, we will go from the shortest to the highest component. This helps you successfully solder your board.

![The position of the reset button.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_41.jpg "The position of the reset button.")

![The fully seated reset button in side view.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_42.jpg "The fully seated reset button in side view.")

The shape of the button legs is designed to snap into the Buzzerino PCB. After placing the buttons, check if they sit flush to the Buzzerino PCB. Then solder and clip the leads.

![The position of the D-pad buttons.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_43.jpg "The position of the D-pad buttons.")

With the D-pad buttons it's the same procedure as with the reset button.

### Soldering the pin header

![The 2x7 pin header.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_44.jpg "The 2x7 pin header.")

![The 2x7 pin header position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_45.jpg "The 2x7 pin header position.")

We start by soldering the 2x7 pin header to the Buzzerino PCB. The short side shall go through the board.

![The 2x7 pin header soldered by one pin.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_46.jpg "The 2x7 pin header soldered by one pin.")

As with all the previous solder joints, it's best practice to first solder one pin and then check for alignment. The wider pin header will often move a bit out of the Buzzerino PCB and may need to be reseated. Only after the pin headers are placed completely in position should you solder the remaining pins.

![The 2x3 pin header.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_47.jpg "The 2x3 pin header.")

![The 2x3 pin header position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_48.jpg "The 2x3 pin header position.")

![The 2x16 pin header.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_49.jpg "The 2x16 pin header.")

![The 2x16 pin header position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_50.jpg "The 2x16 pin header position.")

Repeat the previous step for the 2x3 and 2x16 pin headers. Start by soldering one pin, then check and correct the seating, and then solder completely.

![The 1x3 pin header.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_51.jpg "The 1x3 pin header.")

![The 1x3 pin header position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_52.jpg "The 1x3 pin header position.")

The 1x3 pin header is one of the fiddliest to install. With all the previous practice, you will be able to solder it flush and perpendicular.

### Installation of the USB-C breakout board

![The prepared USB-C breakout board.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_53.jpg "The prepared USB-C breakout board.")

![The position of the USB-C breakout board.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_54.jpg "The position of the USB-C breakout board.")

![The USB-C breakout board soldered by one pin.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_55.jpg "The USB-C breakout board soldered by one pin.")

![Side view onto the USB-C breakout board.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_56.jpg "Side view onto the USB-C breakout board.")

Now we can solder the USB-C breakout board. After placing it into position (right side of the Buzzerino PCB, facing outwards, the usb port shuld be fully visible), solder one pin. Then check for alignment; it should be parallel to the Buzzerino PCB. Then solder it completely.

**WARNING:** The pins of the USB-C breakout board, the AHT10 temperature sensor, and the 1602A LCD display are too hard for electronics side cutters! Use larger wire cutters as provided.

### Installation of the AHT10 digital temperature sensor

![The prepared AHT10 digital temperature sensor.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_57.jpg "The prepared AHT10 digital temperature sensor.")

![The position of the AHT10 digital temperature sensor.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_58.jpg "The position of the AHT10 digital temperature sensor.")

The installation of the AHT10 digital temperature sensor is analogous to the USB-C breakout board (Position left side of the Buzzerino PCB, facing upwards, the components of the sub-PCB sould be visible). But remember: use the larger wire cutters for cutting the pins to length.

### Installation of the Buzzer

![The buzzer.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_59.jpg "The buzzer.")

![The position of the buzzer.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_60.jpg "The position of the buzzer.")

Finally, we can install the namesake of the board. This is our first polarized component. The footprint on the Buzzerino PCB shows a small +. Furthermore, the sticker on the buzzer has a plus on its top. These should be aligned. (If you look from the front of the Buzzerino PCB, the plus will be facing inwards.) Moreover, if you are unsure, you can check the buzzer's leads: one of them is longer than the other. This is the positive lead and shall align with the plus symbol on the Buzzerino PCB.

### Installation of the 1602A LCD display

![The prepared 1602A LCD display.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_61.jpg "The prepared 1602A LCD display.")

![The position of the 1602A LCD display.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_62.jpg "The position of the 1602A LCD display.")

![The 1602A LCD display soldered by one pin.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_63.jpg "The 1602A LCD display soldered by one pin.")

![Side view onto the 1602A LCD display.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_64.jpg "Side view onto the 1602A LCD display.")

The installation of the 1602A LCD display is analogous to the USB-C breakout board. First place, then solder one pin, then check for alignment and then solder fully. But remember: use the larger wire cutters for cutting the pins to length.

### Installation of the RGB LED

![The RGB LED.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_65.jpg "The RGB LED.")

Now we can install the RGB LED.

![Position and rotation of the RGB LED.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_66.jpg "Position and rotation of the RGB LED.")

![Fully seated RGB LED.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_67.jpg "Fully seated RGB LED.")

The LED is, like the buzzer, polarized. But here the longest leg is the negative connection. The footprint on the Buzzerino PCB indicates the negative connection with a square pad. Place the LED into its position. The spacing of the pads is a bit wider than the lead spacing, so the leads will spread out automatically.

![RGB LED leads cropped.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_68.jpg "RGB LED leads cropped.")

![Reflow RGB LED solder joints.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_70.jpg "Reflow RGB LED solder joints.")

![Cleaned up RGB LED solder joints.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_71.jpg "Cleaned up RGB LED solder joints.")

After soldering and cutting the RGB LED's leads flush, you will probably have a very ugly joint. Luckily, you can easily correct this by slowly running your soldering iron between two pins and letting them reflow. The surface tension of the solder should pull the joint nicely together, resulting in a clean joint. But check for any bridges after that! If there are any, clean your soldering tip and repeat the reflow.

### Installation of the Raspberry Pi Pico

![The prepared Raspberry Pi Pico.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_72.jpg "The prepared Raspberry Pi Pico.")

![The position of the Raspberry Pi Pico.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_73.jpg "The position of the Raspberry Pi Pico.")

![The Raspberry Pi Pico tacked into place.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_74.jpg "The Raspberry Pi Pico tacked into place.")

Now we can install the Raspberry Pi Pico. As with all previous complex connections, start by soldering only two opposite pins. Then check for alignment; after that, solder completely.

### Installation of the analogue joystick

![The analogue joystick.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_75.jpg "The analogue joystick.")

![The analogue joystick position.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_76.jpg "The analogue joystick position.")

As the last component, we will install the analogue joystick.

![Installation of the analogue joystick.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_77.jpg "Installation of the analogue joystick.")

![Side view of the fully seated analogue joystick.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_78.jpg "Side view of the fully seated analogue joystick.")

The installation of the joystick can be harder than necessary because of the many different pins. Start by putting the push-button pins into the Buzzerino PCB and then wiggle each pin from left to right slowly into position. After the joystick is fully seated, you can solder it in place.

![Everything soldered in place.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_79.jpg "Everything soldered in place.")

### Installation of the jumpers

![Installed jumpers.](https://github.com/EmilAltmann/Buzzerino/blob/main/images/pictures_of_construction/instruction_step_80.jpg "Installed jumpers.")

Before testing the board, we need to install all 26 jumpers as seen in the picture above.

**Congratulations, you have finished your soldering project.**

If you already flashed your Raspberry Pi Pico, it's time to power it up; if not, flash the self_test.uf2 onto the board.

## Flashing

If the board is not connected to a power source:
Press and hold the BOOTSEL button on the Raspberry Pi Pico and connect it via micro-USB to a PC. The Pico should mount as a mass-storage device called RPI-RP2. Now you can drag and drop a previously compiled or downloaded .uf2 file onto it. It will automatically unmount and reboot. Ignore the message from your PC that the device was disconnected without ejecting.
You don't need to unplug and replug the Raspberry Pi Pico every time you want to flash it. You can also, with an established connection to your PC, press and hold the BOOTSEL button on the Raspberry Pi Pico and then briefly press (while still holding the BOOTSEL button) the SW_RESET button on the Buzzerino PCB.

## Example Programs

The code of the example programs and their .uf2 files will be available in the folder example_programs. Unless otherwise noted, all programs are designed and compiled for the Raspberry Pi Pico (neither the W version nor version 2) but can be recompiled for these targets. The preferred way to compile is VS Code with the Raspberry Pi Pico extension.

## How to Code

You can code the Raspberry Pi Pico using the Arduino IDE or Microsoft Visual Studio Code (VS Code) with the Raspberry Pi Pico extension. If you use the Arduino IDE, you need to install the necessary board support as described in the [blog post from Random Nerd Tutorials](https://randomnerdtutorials.com/programming-raspberry-pi-pico-w-arduino-ide/).

## Parts and Wiki

### Raspberry Pi Pico
The core of the whole board is a [Raspberry Pi Pico microcontroller](https://www.raspberrypi.com/products/raspberry-pi-pico/). It can be programmed as described in the [How to Code](#how-to-code) section. The SW_RESET button to the left of the microcontroller can be used to reset the microcontroller.

### 1602A LCD display
The display is a 16-character by 2-row LCD. It is connected via a parallel bus to the Raspberry Pi Pico. The [datasheet](https://www.openhacks.com/uploadsproductos/eone-1602a1.pdf) explains its function as it is implemented in the example program. Because the display works on 5 V and the Raspberry Pi Pico on 3.3 V, you can only write to the display and the read/write pin is always set to write (tied low).
The pinout is as follows:
| GPIO | LCD |
|------|-----|
| 6    | D7  |
| 7    | D6  |
| 8    | D5  |
| 9    | D4  |
| 10   | D3  |
| 11   | D2  |
| 12   | D1  |
| 13   | D0  |
| 14   | E   |
| 15   | RS  |

### AHT10 temperature and humidity sensor
The AHT10 digital temperature and humidity sensor is connected via I2C to the Raspberry Pi Pico. It has the address 0x38 and can be accessed as described in the [datasheet](https://server4.eca.ir/eshop/AHT10/Aosong_AHT10_en_draft_0c.pdf). It is connected to the I2C0 interface on the Raspberry Pi Pico.
The pinout is as follows:
| GPIO | AHT10 |
|------|-------|
| 0    | SDA   |
| 1    | SCL   |

### Buzzer
The buzzer is an active 4.5 kHz buzzer. It is connected to GPIO pin 19 on the Raspberry Pi Pico. Active means it contains a driver and will produce this tone when powered. You can chop the sound using PWM to reproduce different sound effects.

### D-pad buttons
The D-pad buttons can be used as a generic input. They are connected directly to a digital pin of the Raspberry Pi Pico without any filter or pull-up or pull-down resistors. So the internal pull-up resistors need to be activated and debounce logic needs to be implemented in software.
The pinout is as follows:
| GPIO | Button |
|------|--------|
| 2    | up     |
| 3    | down   |
| 4    | left   |
| 5    | right  |

### Analogue joystick
The analogue joystick can be used as a fine-grained input. It is connected to the 12-bit analogue-to-digital converter (ADC) of the Raspberry Pi Pico. It has two axes and a push-button. The X-axis measures the up/down angle between 0 (fully down) and 4095 (fully up), the Y-axis measures correspondingly the left/right angle between 0 (fully left) and 4095 (fully right). Due to manufacturing tolerances, the exact midpoint and extrema should be determined experimentally. The push-button is, like the D-pad buttons, directly connected to a digital pin of the Raspberry Pi Pico.
The pinout is as follows:
| GPIO      | Joystick |
|-----------|----------|
| 18        | button   |
| 31 (ADC0) | X-axis   |
| 32 (ADC1) | Y-axis   |

### RGB LED
The RGB LED is connected to three pins of the Raspberry Pi Pico. It can be dimmed via the PWM function as seen in the example program.
The pinout is as follows:
| GPIO | LED |
|------|-----|
| 20   | R   |
| 21   | G   |
| 22   | B   |

### 100 kΩ NTC analogue temperature sensor
The 100 kΩ NTC analogue temperature sensor is a second way to measure the temperature of the surroundings. It is connected to the 12-bit analogue-to-digital converter (ADC) of the Raspberry Pi Pico like the potentiometers of the joystick. It is configured in a voltage divider with a 120 kΩ pull-down resistor and a 3.3 V positive connection. The midpoint is connected to pin 28 (ADC2) on the Raspberry Pi Pico. Its B-value is 3950 K and its resistance is 100 kΩ at 25 °C.

### USB-C power connector
The USB-C power connector can be used as an alternative way to power the board. For example, it's necessary if you want to use the Raspberry Pi Pico as a USB host for a keyboard.
**KNOWN ISSUES:** The USB-C connector doesn't completely comply with the standard, so some smart USB chargers don't recognize the board as a valid device. Just use a cable without data connections or try another charger.

### Breakout / Jumper pins
The board has three blocks of jumper pins. They connect the GPIO pins from the Raspberry Pi Pico to the peripherals. The inner row represents the GPIO, the outer the periphery. If you want to disable or reconfigure functions of the board, you can do this by means of jumper cables. Moreover, the board has a 3-pin connector which can be used as a ground and power connection to the 5 V and 3.3 V rails.

### UART Interface
If you want to communicate via a serial interface with the board, you can use UART. There are two options available. The first one is the micro-USB port on the Raspberry Pi Pico, the second is the two pins marked as UART0 TX and UART0 RX. These are connected to the GPIO pins and can be connected to other UART devices or via a USB–serial converter.
