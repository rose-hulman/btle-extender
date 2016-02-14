Rose-Hulman Design Bluetooth Low Energy (BTLE) Range Extender Project
=====================================================================

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
### [Softdevice nRF51\_SDK v9.x.x](http://developer.nordicsemi.com/nRF51_SDK/nRF51_SDK_v9.x.x/)
+ Version says 9.x.x, yet actual softdevice version is 8.0.0
+ We are using the S110 library.
+ [Documentation](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk51.v9.0.0%2Fnrf51_getting_started.html)



Working Code
-------------
Project in its current working state is in /workingProject/ble_app_uart