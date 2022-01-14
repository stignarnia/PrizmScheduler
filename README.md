# PrizmTCP

A simplified TCP Tahoe implementation for CASIO PRIZM calculators like the **fx-CG50**. Based on the [PrizmSDK+libfxcg](https://github.com/Jonimoose/libfxcg) community project. 
 
This **isn't** an actual way to make the calculator communicate via TCP, just an implementation of its algorithm to see how it works and evolves over time given some initial parameters and some that are requested every time it wants to send a group of segments. 
  
I built this using excercises from the Telecommunications Networks course at my university so it might not reflect reality and contain several simpifications that were made to make those excercises doable by hand.

## How to install

 1. [Update](https://edu.casio.com/dl/) your calculator;
 2. [Download](https://github.com/stignarnia/PrizmTCP/releases/latest/download/PrizmTCP.g3a) the program;
 3. Connect the calculator to you PC using the USB cable included in the box and press F1 on the calculator;
 4. Drag&Drop the `.g3a` file into the calculator which is now connected as a normal thumbstick;
 5. Safely remove the device;
 6. You'll find the program in the calculator's home screen, Enjoy!

## How to modify and compile

 1. Get the [PrizmSDK+libfxcg](https://github.com/Jonimoose/libfxcg) ready using their own README as instructions;
 2. Download this repository and, once extracted to a folder named `PrizmTCP`, place the former inside the `projects` directory of the SDK;
 3. Open a terminal window inside the `PrizmTCP` folder and give the `./make` command;
 4. You'll find the new `.g3a` executable in the same folder.
 
If you want to change the name that appears on the calculator as program name, modify line 32 of the `Makefile` like this: `MKG3AFLAGS := -n basic:<NEW NAME> -i uns:../unselected.bmp -i sel:../selected.bmp` 
 
Changing the name of the `PrizmTCP` folder will also change the name of the output executable but not one that appears on the calculator's menu.
 
If you want to change the icon, modify both the `.bmp` files and export them with a color depth of 24 and without changing the dimensions. 
 
If you want to modify the actual program, just use the `src\main.c` file. You can also change the name and add multiple `.c` and `.h` files, everything in that directory will be compiled. It should also do C++ but refer to the SDK documentation for that.