/*
 *
 * Header file for CC1101 To be used with non TI MCU's
 *
 * Contents from multiple sources which compiled to
 * something applicable for the NRF51 4228
 *
 * All contents in library are values found in the CC1101 datasheet
 *
 * Complied by Anthony Adamo
 * 12/17/2015
 * v1.0
 */

/*
 * Pins to be connected to the CC1101
 * I don'tknow these pins yet so the values are currently 0.
 */
const SCK_PIN   				  = 0;
const MISO_PIN  				  = 0;
const MOSI_PIN  				  = 0;
const SS_PIN    				  = 0;
const GDO0						  = 0;
const GDO2						  = 0;

/*
 * Type of transfers
 */
const WRITE_BURST				  = 0x40;
const READ_SINGLE             	  = 0x80;
const READ_BURST               	  = 0xC0;

/*
 * Type of register
 */
const CONFIG_REG                  = 0x80;
const STATUS_REG                  = 0xC0;

/*
 * PATABLE & FIFO addresses
 */
const PATABLE          	  		  = 0x3E; // PATABLE address
const TXFIFO        		      = 0x3F; // TX FIFO address
const RXFIFO		         	  = 0x3F; // RX FIFO address


/*
 * Command Strobes
 */

const _SRES                       = 0x30; // Reset chip.
const _SFSTXON                    = 0x31; // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                          // If in RX/TX: Go to a wait state where only the synthesizer is
                                          // running (for quick RX / TX turnaround).
const _SXOFF                      = 0x32; // Turn off crystal oscillator.
const _SCAL                       = 0x33; // Calibrate frequency synthesizer and turn it off
                                          // (enables quick start).
const _SRX                        = 0x34; // Enable RX. Perform calibration first if coming from IDLE and
                                          // MCSM0.FS_AUTOCAL=1.
const _STX                        = 0x35; // In IDLE state: Enable TX. Perform calibration first if
                                          // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                          // Only go to TX if channel is clear.
const _SIDLE                      = 0x36; // Exit RX / TX, turn off frequency synthesizer and exit
                                                // Wake-On-Radio mode if applicable.
const _SAFC                       = 0x37; // Perform AFC adjustment of the frequency synthesizer
const _SWOR                       = 0x38; // Start automatic RX polling sequence (Wakeon-Radio)
const _SPWD                       = 0x39; // Enter power down mode when CSn goes high.
const _SFRX                       = 0x3A; // Flush the RX FIFO buffer.
const _SFTX                       = 0x3B; // Flush the TX FIFO buffer.
const _SWORRST                    = 0x3C; // Reset real time clock.
const _SNOP                       = 0x3D; // No operation. May be used to pad strobe commands to two
                                          // INT8Us for simpler software.




/*
 * CC1101 configuration register ADDRESSES
 */
const _IOCFG2                     = 0x00; // GDO2 output pin configuration
const _IOCFG1                     = 0x01; // GDO1 output pin configuration
const _IOCFG0                     = 0x02; // GDO0 output pin configuration
const _FIFOTHR                    = 0x03; // RX FIFO and TX FIFO thresholds
const _SYNC1                      = 0x04; // Sync word, high INT8U
const _SYNC0                      = 0x05; // Sync word, low INT8U
const _PKTLEN                     = 0x06; // Packet length
const _PKTCTRL1                   = 0x07; // Packet automation control
const _PKTCTRL0                   = 0x08; // Packet automation control
const _ADDR                       = 0x09; // Device address
const _CHANNR                     = 0x0A; // Channel number
const _FSCTRL1                    = 0x0B; // Frequency synthesizer control
const _FSCTRL0                    = 0x0C; // Frequency synthesizer control
const _FREQ2                      = 0x0D; // Frequency control word, high INT8U
const _FREQ1                      = 0x0E; // Frequency control word, middle INT8U
const _FREQ0                      = 0x0F; // Frequency control word, low INT8U
const _MDMCFG4                    = 0x10; // Modem configuration
const _MDMCFG3                    = 0x11; // Modem configuration
const _MDMCFG2                    = 0x12; // Modem configuration
const _MDMCFG1                    = 0x13; // Modem configuration
const _MDMCFG0                    = 0x14; // Modem configuration
const _DEVIATN                    = 0x15; // Modem deviation setting
const _MCSM2                      = 0x16; // Main Radio Control State Machine configuration
const _MCSM1                      = 0x17; // Main Radio Control State Machine configuration
const _MCSM0                      = 0x18; // Main Radio Control State Machine configuration
const _FOCCFG                     = 0x19; // Frequency Offset Compensation configuration
const _BSCFG                      = 0x1A; // Bit Synchronization configuration
const _AGCCTRL2                   = 0x1B; // AGC control
const _AGCCTRL1                   = 0x1C; // AGC control
const _AGCCTRL0                   = 0x1D; // AGC control
const _WOREVT1                    = 0x1E; // High INT8U Event 0 timeout
const _WOREVT0                    = 0x1F; // Low INT8U Event 0 timeout
const _WORCTRL                    = 0x20; // Wake On Radio control
const _FREND1                     = 0x21; // Front end RX configuration
const _FREND0                     = 0x22; // Front end TX configuration
const _FSCAL3                     = 0x23; // Frequency synthesizer calibration
const _FSCAL2                     = 0x24; // Frequency synthesizer calibration
const _FSCAL1                     = 0x25; // Frequency synthesizer calibration
const _FSCAL0                     = 0x26; // Frequency synthesizer calibration
const _RCCTRL1                    = 0x27; // RC oscillator configuration
const _RCCTRL0                    = 0x28; // RC oscillator configuration
const _FSTEST                     = 0x29; // Frequency synthesizer calibration control
const _PTEST                      = 0x2A; // Production test
const _AGCTEST                    = 0x2B; // AGC test
const _TEST2                      = 0x2C; // Various test settings
const _TEST1                      = 0x2D; // Various test settings
const _TEST0                      = 0x2E; // Various test settings

/*
 * CC1101 configuration register VALUES
 *
 * We need to edit these values with results from RF STUDIO
 */
const RF_IOCFG2                     = 0x00; // GDO2 output pin configuration
const RF_IOCFG1                     = 0x01; // GDO1 output pin configuration
const RF_IOCFG0                     = 0x02; // GDO0 output pin configuration
const RF_FIFOTHR                    = 0x03; // RX FIFO and TX FIFO thresholds
const RF_SYNC1                      = 0x04; // Sync word, high INT8U
const RF_SYNC0                      = 0x05; // Sync word, low INT8U
const RF_PKTLEN                     = 0x06; // Packet length
const RF_PKTCTRL1                   = 0x07; // Packet automation control
const RF_PKTCTRL0                   = 0x08; // Packet automation control
const RF_ADDR                       = 0x09; // Device address
const RF_CHANNR                     = 0x0A; // Channel number
const RF_FSCTRL1                    = 0x0B; // Frequency synthesizer control
const RF_FSCTRL0                    = 0x0C; // Frequency synthesizer control
const RF_FREQ2                      = 0x0D; // Frequency control word, high INT8U
const RF_FREQ1                      = 0x0E; // Frequency control word, middle INT8U
const RF_FREQ0                      = 0x0F; // Frequency control word, low INT8U
const RF_MDMCFG4                    = 0x10; // Modem configuration
const RF_MDMCFG3                    = 0x11; // Modem configuration
const RF_MDMCFG2                    = 0x12; // Modem configuration
const RF_MDMCFG1                    = 0x13; // Modem configuration
const RF_MDMCFG0                    = 0x14; // Modem configuration
const RF_DEVIATN                    = 0x15; // Modem deviation setting
const RF_MCSM2                      = 0x16; // Main Radio Control State Machine configuration
const RF_MCSM1                      = 0x17; // Main Radio Control State Machine configuration
const RF_MCSM0                      = 0x18; // Main Radio Control State Machine configuration
const RF_FOCCFG                     = 0x19; // Frequency Offset Compensation configuration
const RF_BSCFG                      = 0x1A; // Bit Synchronization configuration
const RF_AGCCTRL2                   = 0x1B; // AGC control
const RF_AGCCTRL1                   = 0x1C; // AGC control
const RF_AGCCTRL0                   = 0x1D; // AGC control
const RF_WOREVT1                    = 0x1E; // High INT8U Event 0 timeout
const RF_WOREVT0                    = 0x1F; // Low INT8U Event 0 timeout
const RF_WORCTRL                    = 0x20; // Wake On Radio control
const RF_FREND1                     = 0x21; // Front end RX configuration
const RF_FREND0                     = 0x22; // Front end TX configuration
const RF_FSCAL3                     = 0x23; // Frequency synthesizer calibration
const RF_FSCAL2                     = 0x24; // Frequency synthesizer calibration
const RF_FSCAL1                     = 0x25; // Frequency synthesizer calibration
const RF_FSCAL0                     = 0x26; // Frequency synthesizer calibration
const RF_RCCTRL1                    = 0x27; // RC oscillator configuration
const RF_RCCTRL0                    = 0x28; // RC oscillator configuration
const RF_FSTEST                     = 0x29; // Frequency synthesizer calibration control
const RF_PTEST                      = 0x2A; // Production test
const RF_AGCTEST                    = 0x2B; // AGC test
const RF_TEST2                      = 0x2C; // Various test settings
const RF_TEST1                      = 0x2D; // Various test settings
const RF_TEST0                      = 0x2E; // Various test settings









/*
 * Status registers
 */
const _PARTNUM                    = 0x30; //Part number for CC1101 (chip ID)
const _VERSION                    = 0x31; //Current version number (chip ID)
const _FREQEST                    = 0x32; //Frequency Offset Estimate from demodulator
const _LQI                        = 0x33; //Demodulator estimate for Link Quality
const _RSSI                       = 0x34; //Received signal strength indication
const _MARCSTATE                  = 0x35; //Main radio Control state machine state
const _WORTIME1                   = 0x36; //High byte of WOR timer
const _WORTIME0                   = 0x37; //Low byte of WOR timer
const _PKTSTATUS                  = 0x38; //Current GDOx status and packet status
const _VCO_VC_DAC                 = 0x39; //Current setting from PLL calibration module
const _TXBYTES                    = 0x3A; //Underflow and number of bytes in the TX FIFO
const _RXBYTES                    = 0x3B; //Overflow and number of bytes in the RX FIFO
const _RCCTRL1_STATUS             = 0x3C; //Last RC oscillator calibration result
const _RCCTRL0_STATUS             = 0x3D; //Last RC oscillator calibration result

/*
 * Definitions to support burst/single access:
 */
const CRC_OK                      = 0x80; //
const RSSI                        = 0;	  //
const LQI                         = 1;	  //
const BYTES_IN_RXFIFO             = 0x7F; //


/*
 * Definitions for chip status
 */
const CHIP_RDY                    = 0x80; //stays high until power and crystal stabilize
const CHIP_STATE_IDLE             = 0x00; //chip in idle mode
const CHIP_STATE_RX               = 0x10; //receive mode
const CHIP_STATE_TX               = 0x20; //transmit mode
const CHIP_STATE_FSTON            = 0x30; //fast TX ready
const CHIP_STATE_CALIBRATE        = 0x40; //frequency synthesizer calibration is running
const CHIP_STATE_SETTLING         = 0x50; //PLL is settling
const CHIP_STATE_RXFIFO_OVERFLOW  = 0x60; //RXFIFO has overflowed.Read out useful data then flush FIFO with SFRX
const CHIP_STATE_TXFIFO_UNDERFLOW = 0x70; //TXFIFO has underflowed. Acknowledge with SFTX

/*
 *	CC1101 Pins necessary for SPI
 *	I would like to have our Nordic pins defined below these
 *			     	Pin #
 * 3.3V power 	->
 * SS      		->  20
 * SCLK    		->	1
 * SO(GDO1)		->	2
 * GDO2    		->	3
 * GDO0    		->	6
 * CSN     		->	7
 * GND     		->	16 & 19
 *
 */


/*
 * Struct for CC1101 Packet data types
 *
 * Clearly defined Buffer and data lengths
 * this is just a template for the data type
 * we may not need this later on but I do not know yet
 */
const BUFFER_LEN        	  = 64;
const DATA_LEN          	  = BUFFER_LEN - 3;

typedef struct CC1101_Packet_Tag
{
	   /**
	     * Data length
	     */
	    byte length;

	    /**
	     * Data buffer
	     */
	    byte data[CC1101_DATA_LEN];

	    /**
	     * CRC OK flag
	     */
	    boolean crc_ok;

	    /**
	     * Received Strength Signal Indication
	     */
	    byte rssi;

	    /**
	     * Link Quality Index
	     */
	    byte lqi;


}CC1101_Packet;
















