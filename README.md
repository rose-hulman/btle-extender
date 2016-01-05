Rose-Hulman Design Bluetooth Low Energy (BTLE) Range Extender Project
=====================================================================

Makefile setup:
---------------
There are 2 projects: the peripheral the central.
The examples are compiled from within the Nordic SDK directory.
Each is built using hte Nordic examples for HRM.
They reference the SDK locations:
+ examples/ble_peripheral/ble_app_hrs
+ examples/ble_central/ble_app_hrs_c
++ There is a bug in this Nordic example.
   To fix this add the following line to ./examples/ble_central/ble_app_hrs_c/main.c
   in the SDK directory
<code>
#include "app_uart.h"
</code>

+ In the file common/rules/nrf5x_defs.mak
  Set the location of your Nordic SDK installation:
  <code>
  ifeq ($(NORDIC_SDK_VERSION), 10)
  NORDIC_SDK_ROOT		?= ../../../nRF/SDK/nRF51_SDK_10.0.0_dc26b5e
  endif
  </code>

+ From the top-level directory you can compile both examples with the command 'make'.
+ To program the peripheral or central example you have to change directory (cd)
  into the project that you want to work with and issue the command 'make flash-all'
+ This will write the softdevice that is being compiled and the example into flash.
+ This project uses softdevice s130 instead of the older s110.
+ Nordic is end of lifing the s110 in favor of the s130.
+ To see debug output you need to run the "JLink RTTC Client" once the program has started running
  on the nRF51 board.


Windows Cygwin Install
----------------------
+ [Cygwin](https://www.cygwin.com/) is a "Unix" emulator for Windows.
	<p>There are plenty of options within the Cygwin installation.
       Use the github repository rose-hulman/tools/Cygwin/install.zip
	   This will install many of the 'Unix' tools you will need to build
	   ARM projects on a windows machine.
	</p>

Toolchain Install
-----------------
### [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded)
+ Extract (via tar, unzip) into the directory
	<p><code>
	/usr/local/gcc-arm-none-eabi-4\_9-2015q2
	</code></p>
+ Create a symlink
	/usr/local/gcc-arm-none-eabi -> /usr/local/gcc-arm-none-eabi-4\_9-2015q2
	<p><code>
	$ ln -s /usr/local/gcc-arm-none-eabi-4\_9-2015q2 /usr/local/gcc-arm-none-eabi
	</code></p>
	This needs to match the Makefile symbol GNU\_GCC\_ROOT

### [Segger JLink](https://www.segger.com/jlink-software.html)
+ Download and install the appropriate for you OS.
+ Works on Linux and OSX.
+ Should work on Windows - I have not tested it.

### [Eclipse, CDT](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/marsr)
+ Not necessary for merely running your program.
+ Eclipse Addtional Installation
	+ Eclipse->Help->Install New Software...
	+ Work with: http://embsysregview.sourceforge.net/update
	+ Work with: http://gnuarmeclipse.sourceforge.net/updates
+ Once installed, copy the nrf51.xml file into the embsysreg plugins directory
	+ Linux:
	<p><code>
	$ cp nRF51\_SDK\_9.0.0\_2e23562/SVD/nrf51.xml ~/.eclipse/org.eclipse.platform\_793567567\_linux\_gtk\_x86\_64/plugins/org.eclipse.cdt.embsysregview.data\_0.2.5.r180/data/cortex-m0/Nordic/nrf51.xml
	</code></p>
	+ OSX:
	<p><code>
	cp nRF51\_SDK\_9.0.0\_2e23562/SVD/nrf51.xml /Applications/Eclipse.app/Contents/Eclipse/plugins/org.eclipse.cdt.embsysregview.data\_0.2.5.r180/data/cortex-m0/Nordic/nrf51.xml
	</code></p>

Linux Notes
-----------
### JLink access
+ Access to the device /dev/ttyACMx (where x is 0, 1, ...) requires root priveledges.
+ The Makefile uses sudo to deal with this. To make your life easier
1. Add yourself to the dialout group
   usermod -a -G dialout <username>	(Requires logout for activation)
   id <username>			(Used to check your groups)
2. Edit the sudoers file using visudo.
   Add JLink apps to require not passwords for members of dialout:
   <p><code>
   ## Allow members of the dialout group to sudo the JLink apps without a password.
   %dialout        ALL=(ALL)       NOPASSWD: /opt/JLink/JLinkExe
   %dialout        ALL=(ALL)       NOPASSWD: /opt/JLink/JLinkGDBServer
   %dialout        ALL=(ALL)       NOPASSWD: /opt/JLink/JLinkRemoteServer
   %dialout        ALL=(ALL)       NOPASSWD: /opt/JLink/JLinkRTTClient
   %dialout        ALL=(ALL)       NOPASSWD: /opt/JLink/JLinkSTM32
   %dialout        ALL=(ALL)       NOPASSWD: /opt/JLink/JLinkSWOViewer
   </code></p>

Nordic SDK
----------
### [Softdevice nRF51\_SDK v10.x.x](http://developer.nordicsemi.com/nRF5_SDK/)
+ The softdevice version is 8.0.0.
+ The Noridc device is NRF51.
+ We are using the S130 library - was the S110 library.
+ [Documentation](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk51.v9.0.0%2Fnrf51_getting_started.html)


