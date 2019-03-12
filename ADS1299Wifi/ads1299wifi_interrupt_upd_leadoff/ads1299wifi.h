
#define SECRET_SSID "raspi_wifi"
#define SECRET_PASS "fluidfluid"


#define int32_t unsigned long
#define SIGN_EXT_24(VAL)    ((int32_t)((uint32_t)(VAL) ^ (1UL<<(23))) - (1L<<(23)))

#define SEND_SIZE 25
#define PACKET_SIZE 40


/*************************************************************************************************************************************************
 *               Sampling Config                                                                                                       *
 *************************************************************************************************************************************************/

/**
 *  \brief Default data rate from the ADS1299.
 *
 *    To monitor electrode impedance continuously, an AC current is pulsed through each electrode and the corresponding
 *  voltage perturbation observed in the measured signal. This signal will not be easily separable from the EEG if it
 *  is within the typical 0-100 Hz EEG bandwidth; since the fastest possible AC excitation rate the ADS1299 can
 *  generate is (data rate)/4, the lowest recommended data rate that allows continuous impedance monitoring is 1000 Hz.
 *  Using a 500 Hz data rate will generate an AC excitation at 125 Hz, which is dangerously close to, if not in,
 *  the EEG band.
 */
#define DEFAULT_SAMPLE_RATE         250
#define MAX_CHANNELS            8


/**************************************************************************************************************************************************
*               Other Useful Definitions                                                                                                          *
**************************************************************************************************************************************************/


#define SIGN_EXT_24(VAL)                        ((int32_t)((uint32_t)(VAL) ^ (1UL<<(23))) - (1L<<(23)))

/* Default register values */
#define ADS1299_REGDEFAULT_ID           ADS1299_DEVICE_ID
#define ADS1299_REGDEFAULT_CONFIG1      0x96            ///< Multiple readback mode, OSC output disabled, DR = FMOD/4096
#define ADS1299_REGDEFAULT_CONFIG2      0xD0            ///< Test signal sourced internally, low-amplitude test signal pulsed at FCLK/(2^21)
#define ADS1299_REGDEFAULT_CONFIG3      0x68            ///< Ref buffer off, bias measurement off, internal bias ref, bias buffer off, bias sense disabled
#define ADS1299_REGDEFAULT_LOFF         0x00            ///< 95%/5% LOFF comparator threshold, DC lead-off at 6 nA
#define ADS1299_REGDEFAULT_CHNSET       0xE0            ///< Channel off, gain 24, SRB2 disconnected, normal electrode input
#define ADS1299_REGDEFAULT_BIAS_SENSP   0x00            ///< All BIAS channels disconnected from positive leads
#define ADS1299_REGDEFAULT_BIAS_SENSN   0x00            ///< All BIAS channels disconnected from negative leads
#define ADS1299_REGDEFAULT_LOFF_SENSP   0x00            ///< All LOFF channels disconnected from positive leads
#define ADS1299_REGDEFAULT_LOFF_SENSN   0x00            ///< All LOFF channels disconnected from negative leads
#define ADS1299_REGDEFAULT_LOFF_FLIP    0x00            ///< No flipping in this house; source/pull-up at INP, sink/pull-down at INN
#define ADS1299_REGDEFAULT_LOFF_STATP   0x00            ///< This is a read-only register; reset value is 0x00
#define ADS1299_REGDEFAULT_LOFF_STATN   0x00            ///< This is a read-only register; reset value is 0x00
#define ADS1299_REGDEFAULT_GPIO         0x0F            ///< All GPIO set to inputs
#define ADS1299_REGDEFAULT_MISC1        0x00            ///< SRB1 disconnected
#define ADS1299_REGDEFAULT_MISC2        0x00            ///< Register not used in this silicon; should stay at 0x00
#define ADS1299_REGDEFAULT_CONFIG4      0x00            ///< Continuous conversion, LOFF comparator powered down


/**************************************************************************************************************************************************
*               Typedefs and Struct Declarations/Definitions                                                                                      *
**************************************************************************************************************************************************/

/**
 *  \brief Error codes for interacting with the ADS1299.
 *
 */
typedef int ads1299_error_t;

/**
 *  \brief ADS1299 register addresses.
 *
 * Consult the ADS1299 datasheet and user's guide for more information.
 */
#define ADS1299_REGADDR_ID           0x00           ///< Chip ID register. Read-only. ID[4:0] should be 11110.
#define ADS1299_REGADDR_CONFIG1      0x01           ///< Configuration register 1. Controls daisy-chain mode; clock output; and data rate.
#define ADS1299_REGADDR_CONFIG2      0x02           ///< Configuration register 2. Controls calibration signal source, amplitude, and frequency.
#define ADS1299_REGADDR_CONFIG3      0x03           ///< Configuration register 3. Controls reference buffer power and the bias functionality.
#define ADS1299_REGADDR_LOFF         0x04           ///< Lead-off control register. Controls lead-off frequency, magnitude, and threshold.
#define ADS1299_REGADDR_CH1SET       0x05           ///< Channel 1 settings register. Controls channel 1 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH2SET       0x06           ///< Channel 2 settings register. Controls channel 2 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH3SET       0x07           ///< Channel 3 settings register. Controls channel 3 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH4SET       0x08           ///< Channel 4 settings register. Controls channel 4 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH5SET       0x09           ///< Channel 5 settings register. Controls channel 5 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH6SET       0x0A           ///< Channel 6 settings register. Controls channel 6 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH7SET       0x0B           ///< Channel 7 settings register. Controls channel 7 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_CH8SET       0x0C           ///< Channel 8 settings register. Controls channel 8 input mux, SRB2 switch, gain, and power-down.
#define ADS1299_REGADDR_BIAS_SENSP   0x0D           ///< Bias drive positive sense selection. Selects channels for bias drive derivation (positive side).
#define ADS1299_REGADDR_BIAS_SENSN   0x0E           ///< Bias drive negative sense selection. Selects channels for bias drive derivation (negative side).
#define ADS1299_REGADDR_LOFF_SENSP   0x0F           ///< Lead-off positive sense selection. Selects channels that will use lead-off detection (positive side).
#define ADS1299_REGADDR_LOFF_SENSN   0x10           ///< Lead-off negative sense selection. Selects channels that will use lead-off detection (negative side).
#define ADS1299_REGADDR_LOFF_FLIP    0x11           ///< 1: Swaps lead-off current source and sink on the corresponding channel. See datasheet.
#define ADS1299_REGADDR_LOFF_STATP   0x12           ///< Lead-off positive side status register. Read-only. 0: lead on, 1: lead off.
#define ADS1299_REGADDR_LOFF_STATN   0x13           ///< Lead-off negative side status register. Read-only. 0: lead on, 1: lead off.
#define ADS1299_REGADDR_GPIO         0x14           ///< GPIO register. Controls state and direction of the ADS1299 GPIO pins.
#define ADS1299_REGADDR_MISC1        0x15           ///< Miscellaneous 1. Connects/disconnects SRB1 to all channels' inverting inputs.
#define ADS1299_REGADDR_MISC2        0x16           ///< Miscellaneous 2. No functionality in current revision of ADS1299.
#define ADS1299_REGADDR_CONFIG4      0x17           ///< Configuration register 4. Enables/disables single-shot mode and controls lead-off comparator power.

/**
 *  \brief ADS1299 SPI communication opcodes.
 *
 * Consult the ADS1299 datasheet and user's guide for more information.
 * For RREG and WREG opcodes, the first byte (opcode) must be ORed with the address of the register to be read/written.
 * The command is completed with a second byte 000n nnnn, where n nnnn is (# registers to read) - 1.
 */
#define ADS1299_OPC_WAKEUP       0x02           ///< Wake up from standby.
#define ADS1299_OPC_STANDBY      0x04           ///< Enter standby.
#define ADS1299_OPC_RESET        0x06           ///< Reset all registers.
#define ADS1299_OPC_START        0x08           ///< Start data conversions.
#define ADS1299_OPC_STOP         0x0A           ///< Stop data conversions.

#define ADS1299_OPC_RDATAC       0x10           ///< Read data continuously (registers cannot be read or written in this mode).
#define ADS1299_OPC_SDATAC       0x11           ///< Stop continuous data read.
#define ADS1299_OPC_RDATA        0x12           ///< Read single data value.

#define ADS1299_OPC_RREG         0x20           ///< Read register value.
#define ADS1299_OPC_WREG         0x40           ///< Write register value.

/* ID REGISTER ***********************************************************************************************************************************/

/**
 *  \brief Factory-programmed device ID for ADS1299, stored in ID[3:0].
 */
// Factory-programmed device ID for ADS1299. Stored in ID[3:0].
#define ADS1299_DEVICE_ID       0b1110

/* CONFIG1 REGISTER ******************************************************************************************************************************/

/**
 *  \brief Bit location and size definitions for CONFIG1.CLK_EN bit (oscillator output on CLK pin en/disabled).
 *
 * Consult the ADS1299 datasheet, page 40, for more information.
 */
#define ADS1299_REG_CONFIG1_CLOCK_OUTPUT_DISABLED    (0<<5)
#define ADS1299_REG_CONFIG1_CLOCK_OUTPUT_ENABLED     (1<<5)

/**
 *  \brief Bit location and size definitions for CONFIG1.DAISY_EN bit.
 *
 * Consult the ADS1299 datasheet, pp. 40 and 31-34, for more information.
 */
#define ADS1299_REG_CONFIG1_DAISY_CHAIN_MODE         (0<<6)
#define ADS1299_REG_CONFIG1_MULTI_READBACK_MODE      (1<<6)

/**
 *  \brief Bit mask definitions for CONFIG1.DR (data rate).
 *
 * FMOD = FCLK/2, where FCLK is the clock frequency of the ADS1299. This is normally 2.048 MHz.
 */
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_64           0      ///< Data is output at FMOD/64, or 16 kHz at 2.048 MHz.
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_128          1      ///< Data is output at FMOD/128, or 8 kHz at 2.048 MHz.
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_256          2      ///< Data is output at FMOD/256, or 4 kHz at 2.048 MHz.
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_512          3      ///< Data is output at FMOD/512, or 2 kHz at 2.048 MHz.
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_1024         4      ///< Data is output at FMOD/1024, or 1 kHz at 2.048 MHz.
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_2048         5      ///< Data is output at FMOD/2048, or 500 Hz at 2.048 MHz.
#define ADS1299_REG_CONFIG1_FMOD_DIV_BY_4096         6      ///< Data is output at FMOD/4096, or 250 Hz at 2.048 MHz.

/**
 *  \brief Combined value of reserved bits in CONFIG1 register.
 *
 * Consult the ADS1299 datasheet, page 40, for more information.
 */
#define ADS1299_REG_CONFIG1_RESERVED_VALUE          (1<<7)|(1<<4)

/* CONFIG2 REGISTER ******************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for CONFIG2.CAL_FREQ (calibration signal frequency).
 *
 * FCLK is the clock frequency of the ADS1299. This is normally 20 MHz.
 */
#define ADS1299_REG_CONFIG2_CAL_PULSE_FCLK_DIV_2_21     0       ///< Calibration signal pulsed at FCLK/2^21, or approx. 1 Hz at 2.048 MHz.
#define ADS1299_REG_CONFIG2_CAL_PULSE_FCLK_DIV_2_20     1       ///< Calibration signal pulsed at FCLK/2^20, or approx. 2 Hz at 2.048 MHz.
#define ADS1299_REG_CONFIG2_CAL_DC                      3       ///< Calibration signal is not pulsed.


/**
 *  \brief Bit mask definitions for CONFIG2.CAL_AMP0 (calibration signal amplitude).
 */
#define ADS1299_REG_CONFIG2_CAL_AMP_VREF_DIV_2_4_MV     (0<<2)      ///< Calibration signal amplitude is 1 x (VREFP - VREFN)/(2.4 mV).
#define ADS1299_REG_CONFIG2_CAL_AMP_2VREF_DIV_2_4_MV    (1<<2)      ///< Calibration signal amplitude is 2 x (VREFP - VREFN)/(2.4 mV).

/**
 *  \brief Bit mask definitions for CONFIG2.INT_CAL (calibration signal source).
 */
#define ADS1299_REG_CONFIG2_CAL_EXT                     (0<<4)      ///< Calibration signal is driven externally.
#define ADS1299_REG_CONFIG2_CAL_INT                     (1<<4)      ///< Calibration signal is driven internally.

/**
 *  \brief Combined value of reserved bits in CONFIG2 register.
 *
 * Consult the ADS1299 datasheet, page 41, for more information.
 */
#define ADS1299_REG_CONFIG2_RESERVED_VALUE              (6<<5)


/* CONFIG3 REGISTER ******************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for CONFIG3.PD_REFBUF (internal voltage reference buffer enable/disable).
 *
 * Note that disabling the buffer for the internal voltage reference requires that a reference voltage
 * must be externally applied on VREFP for proper operation. This is not related to the reference ELECTRODE
 * buffer, which is an external op-amp on the PCB. Brainboard does not apply a voltage to VREFP, and thus
 * the buffer must be enabled.
 */
#define ADS1299_REG_CONFIG3_REFBUF_DISABLED         (0<<7)
#define ADS1299_REG_CONFIG3_REFBUF_ENABLED          (1<<7)

/**
 *  \brief Bit mask definitions for CONFIG3.BIAS_MEAS (enable or disable bias measurement through BIASIN pin).
 */
#define ADS1299_REG_CONFIG3_BIAS_MEAS_DISABLED          (0<<4)
#define ADS1299_REG_CONFIG3_BIAS_MEAS_ENABLED           (1<<4)

/**
 *  \brief Bit mask definitions for CONFIG3.BIASREF_INT (bias reference internally or externally generated).
 */
#define ADS1299_REG_CONFIG3_BIASREF_EXT                 (0<<3)
#define ADS1299_REG_CONFIG3_BIASREF_INT                 (1<<3)

/**
 *  \brief Bit mask definitions for CONFIG3.PD_BIAS (power-down or enable bias buffer amplifier).
 */
#define ADS1299_REG_CONFIG3_BIASBUF_DISABLED            (0<<2)
#define ADS1299_REG_CONFIG3_BIASBUF_ENABLED             (1<<2)

/**
 *  \brief Bit mask definitions for CONFIG3.BIAS_LOFF_SENS (detection of bias lead-off en/disable).
 */
#define ADS1299_REG_CONFIG3_BIAS_LOFF_SENSE_DISABLED    (0<<1)
#define ADS1299_REG_CONFIG3_BIAS_LOFF_SENSE_ENABLED     (1<<1)

/**
 *  \brief Combined value of reserved bits in CONFIG3 register.
 *
 * Consult the ADS1299 datasheet, page 42, for more information.
 */
#define ADS1299_REG_CONFIG3_RESERVED_VALUE              (3<<5)

/* CONFIG4 REGISTER ******************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for CONFIG4.SINGLE_SHOT (single-shot or continuous conversion setting).
 *
 * This can more easily be set with the RDATAC/SDATAC opcodes.
 */
#define ADS1299_REG_CONFIG4_CONTINUOUS_CONVERSION_MODE      (0<<3)
#define ADS1299_REG_CONFIG4_SINGLE_SHOT_MODE                (1<<3)

/**
 *  \brief Bit mask definitions for CONFIG4.PD_LOFF_COMP (power-down lead-off comparators).
 *
 */
#define ADS1299_REG_CONFIG4_LEAD_OFF_DISABLED               (0<<1)
#define ADS1299_REG_CONFIG4_LEAD_OFF_ENABLED                (1<<1)

/**
 *  \brief Combined value of reserved bits in CONFIG4 register.
 *
 * Consult the ADS1299 datasheet, page 47, for more information.
 */
#define ADS1299_REG_CONFIG4_RESERVED_VALUE      0

/* LOFF REGISTER *********************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for LOFF.COMP_TH (lead-off comparator threshold).
 *
 * Definition names are for the positive side (LOFFP). The corresponding LOFFN thresholds
 * are the difference between these thresholds and 100%. Default value is _95_PERCENT.
 */
#define ADS1299_REG_LOFF_95_PERCENT                 (0<<5)
#define ADS1299_REG_LOFF_92_5_PERCENT               (1<<5)
#define ADS1299_REG_LOFF_90_PERCENT                 (2<<5)
#define ADS1299_REG_LOFF_87_5_PERCENT               (3<<5)
#define ADS1299_REG_LOFF_85_PERCENT                 (4<<5)
#define ADS1299_REG_LOFF_80_PERCENT                 (5<<5)
#define ADS1299_REG_LOFF_75_PERCENT                 (6<<5)
#define ADS1299_REG_LOFF_70_PERCENT                 (7<<5)

/**
 *  \brief Bit mask definitions for LOFF.ILEAD_OFF (lead-off current magnitude).
 *
 * This should be as small as possible for continuous lead-off detection, so as not to noticeably alter
 * the acquired signal. Default is _6_NA.
 */
#define ADS1299_REG_LOFF_6_NA                       (0<<2)          ///< 6 nA lead-off current.
#define ADS1299_REG_LOFF_24_NA                      (1<<2)          ///< 24 nA lead-off current.
#define ADS1299_REG_LOFF_6_UA                       (2<<2)          ///< 6 uA lead-off current.
#define ADS1299_REG_LOFF_24_UA                      (3<<2)          ///< 24 uA lead-off current.

/**
 *  \brief Bit mask definitions for LOFF.FLEAD_OFF (lead-off current frequency).
 *
 * This should be as large as possible for continuous AC lead-off detection to ensure that it is out
 * of the EEG frequency band (approx. 0-100 Hz for most applications). The excitation signal can then
 * be filtered out of the acquired overall signal, and its voltage amplitude measured in order to determine
 * the electrode impedance.
 * FCLK is the clock frequency of the ADS1299. This is normally 2.048 MHz.
 * FDR is the output data rate. With the default clock, this must be at least 1 kHz in order to use
 * continuous AC impedance monitoring, since the excitation frequency of FDR/4 = 250 Hz is the lowest
 * possible frequency outside of the EEG band. If only a specific band is needed and it is lower than
 * 62.5 Hz or 125 Hz, the 250/500 Hz settings may be used.
 */
#define ADS1299_REG_LOFF_DC_LEAD_OFF                0       ///< Lead-off current is at DC.
#define ADS1299_REG_LOFF_AC_LEAD_OFF_FCLK_DIV_2_18  1       ///< Lead-off current is at FCLK/2^18, or 7.8125 Hz at 2.048 MHz.
#define ADS1299_REG_LOFF_AC_LEAD_OFF_FCLK_DIV_2_16  2       ///< Lead-off current is at FCLK/2^16, or 31.25 Hz at 2.048 MHz.
#define ADS1299_REG_LOFF_AC_LEAD_OFF_FDR_DIV_4      3       ///< Lead-off current is at FDR/4.

/**
 *  \brief Combined value of reserved bits in LOFF register.
 *
 */
#define ADS1299_REG_LOFF_RESERVED_VALUE             0

/* CHnSET REGISTERS ******************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for CHnSET.PD (channel power-down).
 */
#define ADS1299_REG_CHNSET_CHANNEL_ON           (0<<7)
#define ADS1299_REG_CHNSET_CHANNEL_OFF          (1<<7)

/**
 *  \brief Bit mask definitions for CHnSET.GAIN (channel PGA gain).
 *
 * Take care to ensure that the gain is appropriate for the common-mode level of the device inputs.
 * Higher gain settings have lower input-referred noise.
 * Consult the ADS1299 datasheet, pages 6-7 and 19-20, for more information.
 */
#define ADS1299_REG_CHNSET_GAIN_1               (0<<4)          ///< PGA gain = 1.
#define ADS1299_REG_CHNSET_GAIN_2               (1<<4)          ///< PGA gain = 2.
#define ADS1299_REG_CHNSET_GAIN_4               (2<<4)          ///< PGA gain = 4.
#define ADS1299_REG_CHNSET_GAIN_6               (3<<4)          ///< PGA gain = 6.
#define ADS1299_REG_CHNSET_GAIN_8               (4<<4)          ///< PGA gain = 8.
#define ADS1299_REG_CHNSET_GAIN_12              (5<<4)          ///< PGA gain = 12.
#define ADS1299_REG_CHNSET_GAIN_24              (6<<4)          ///< PGA gain = 24.

/**
 *  \brief Bit mask definitions for CHnSET.SRB2 (channel internal connection to SRB2 pin).
 */
#define ADS1299_REG_CHNSET_SRB2_DISCONNECTED    (0<<3)
#define ADS1299_REG_CHNSET_SRB2_CONNECTED       (1<<3)

/**
 *  \brief Bit mask definitions for CHnSET.MUX (channel mux setting).
 *
 * Controls the channel multiplexing on the ADS1299.
 * Consult the ADS1299 datasheet, pages 16-17, for more information.
 */
#define ADS1299_REG_CHNSET_NORMAL_ELECTRODE     0           ///< Channel is connected to the corresponding positive and negative input pins.
#define ADS1299_REG_CHNSET_INPUT_SHORTED        1           ///< Channel inputs are shorted together. Used for offset and noise measurements.
#define ADS1299_REG_CHNSET_BIAS_MEASUREMENT     2           ///< Used with CONFIG3.BIAS_MEAS for bias measurement. See ADS1299 datasheet, pp. 53-54.
#define ADS1299_REG_CHNSET_MVDD_SUPPLY          3           ///< Used for measuring analog and digital supplies. See ADS1299 datasheet, p. 17.
#define ADS1299_REG_CHNSET_TEMPERATURE_SENSOR   4           ///< Measures device temperature. See ADS1299 datasheet, p. 17.
#define ADS1299_REG_CHNSET_TEST_SIGNAL          5           ///< Measures calibration signal. See ADS1299 datasheet, pp. 17 and 41.
#define ADS1299_REG_CHNSET_BIAS_DRIVE_P         6           ///< Connects positive side of channel to bias drive output.
#define ADS1299_REG_CHNSET_BIAS_DRIVE_N         7           ///< Connects negative side of channel to bias drive output.

/**
 *  \brief Combined value of reserved bits in CHnSET registers.
 *
 */
#define ADS1299_REG_CHNSET_RESERVED_VALUE       0

/* BIAS_SENSP REGISTER ****************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for BIAS_SENSP register (read-only).
 *
 * Consult the ADS1299 datasheet, page 44, for more information.
 */
#define ADS1299_REG_BIAS_SENSP_BIASP8   (1<<7)
#define ADS1299_REG_BIAS_SENSP_BIASP7   (1<<6)
#define ADS1299_REG_BIAS_SENSP_BIASP6   (1<<5)
#define ADS1299_REG_BIAS_SENSP_BIASP5   (1<<4)
#define ADS1299_REG_BIAS_SENSP_BIASP4   (1<<3)
#define ADS1299_REG_BIAS_SENSP_BIASP3   (1<<2)
#define ADS1299_REG_BIAS_SENSP_BIASP2   (1<<1)
#define ADS1299_REG_BIAS_SENSP_BIASP1   (1<<0)


/* BIAS_SENSN REGISTER ****************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for BIAS_SENSN register (read-only).
 *
 * Consult the ADS1299 datasheet, page 44, for more information.
 */
#define ADS1299_REG_BIAS_SENSN_BIASN8   (1<<7)
#define ADS1299_REG_BIAS_SENSN_BIASN7   (1<<6)
#define ADS1299_REG_BIAS_SENSN_BIASN6   (1<<5)
#define ADS1299_REG_BIAS_SENSN_BIASN5   (1<<4)
#define ADS1299_REG_BIAS_SENSN_BIASN4   (1<<3)
#define ADS1299_REG_BIAS_SENSN_BIASN3   (1<<2)
#define ADS1299_REG_BIAS_SENSN_BIASN2   (1<<1)
#define ADS1299_REG_BIAS_SENSN_BIASN1   (1<<0)

/* LOFF_SENSP REGISTER ****************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for LOFF_SENSP register (read-only).
 *
 * Consult the ADS1299 datasheet, page 45, for more information.
 */
#define ADS1299_REG_LOFF_SENSP_LOFFP8   (1<<7)
#define ADS1299_REG_LOFF_SENSP_LOFFP7   (1<<6)
#define ADS1299_REG_LOFF_SENSP_LOFFP6   (1<<5)
#define ADS1299_REG_LOFF_SENSP_LOFFP5   (1<<4)
#define ADS1299_REG_LOFF_SENSP_LOFFP4   (1<<3)
#define ADS1299_REG_LOFF_SENSP_LOFFP3   (1<<2)
#define ADS1299_REG_LOFF_SENSP_LOFFP2   (1<<1)
#define ADS1299_REG_LOFF_SENSP_LOFFP1   (1<<0)


/* LOFF_SENSN REGISTER ****************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for LOFF_SENSN register (read-only).
 *
 * Consult the ADS1299 datasheet, page 45, for more information.
 */
#define ADS1299_REG_LOFF_SENSN_LOFFN8   (1<<7)
#define ADS1299_REG_LOFF_SENSN_LOFFN7   (1<<6)
#define ADS1299_REG_LOFF_SENSN_LOFFN6   (1<<5)
#define ADS1299_REG_LOFF_SENSN_LOFFN5   (1<<4)
#define ADS1299_REG_LOFF_SENSN_LOFFN4   (1<<3)
#define ADS1299_REG_LOFF_SENSN_LOFFN3   (1<<2)
#define ADS1299_REG_LOFF_SENSN_LOFFN2   (1<<1)
#define ADS1299_REG_LOFF_SENSN_LOFFN1   (1<<0)

/* LOFF_FLIP REGISTER ****************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for LOFF_FLIP register (read-only).
 *
 * Consult the ADS1299 datasheet, page 45, for more information.
 */
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP8    (1<<7)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP7    (1<<6)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP6    (1<<5)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP5    (1<<4)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP4    (1<<3)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP3    (1<<2)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP2    (1<<1)
#define ADS1299_REG_LOFF_FLIP_LOFF_FLIP1    (1<<0)


/* LOFF_STATP REGISTER ***************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for LOFF_STATP register (read-only).
 *
 * Consult the ADS1299 datasheet, page 45, for more information.
 */
#define ADS1299_REG_LOFF_STATP_IN8P_OFF (1<<7)
#define ADS1299_REG_LOFF_STATP_IN7P_OFF (1<<6)
#define ADS1299_REG_LOFF_STATP_IN6P_OFF (1<<5)
#define ADS1299_REG_LOFF_STATP_IN5P_OFF (1<<4)
#define ADS1299_REG_LOFF_STATP_IN4P_OFF (1<<3)
#define ADS1299_REG_LOFF_STATP_IN3P_OFF (1<<2)
#define ADS1299_REG_LOFF_STATP_IN2P_OFF (1<<1)
#define ADS1299_REG_LOFF_STATP_IN1P_OFF (1<<0)

/* LOFF_STATN REGISTER ***************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for LOFF_STATN register (read-only).
 *
 * Consult the ADS1299 datasheet, page 45, for more information.
 */
#define ADS1299_REG_LOFF_STATN_IN8N_OFF (1<<7)
#define ADS1299_REG_LOFF_STATN_IN7N_OFF (1<<6)
#define ADS1299_REG_LOFF_STATN_IN6N_OFF (1<<5)
#define ADS1299_REG_LOFF_STATN_IN5N_OFF (1<<4)
#define ADS1299_REG_LOFF_STATN_IN4N_OFF (1<<3)
#define ADS1299_REG_LOFF_STATN_IN3N_OFF (1<<2)
#define ADS1299_REG_LOFF_STATN_IN2N_OFF (1<<1)
#define ADS1299_REG_LOFF_STATN_IN1N_OFF (1<<0)

/* GPIO REGISTER *********************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for GPIO.GPIODn (GPIO direction bits).
 *
 * The ADS1299 has 4 GPIO pins that can be manipulated via the SPI bus if there are not enough
 * GPIO pins available on the host.
 * GPIOD[4:1] controls the logic levels on GPIO pins 4:1.
 *
 * Consult the ADS1299 datasheet, page 46, for more information.
 */
#define ADS1299_REG_GPIO_GPIOD4_LOW         (0<<7)
#define ADS1299_REG_GPIO_GPIOD4_HIGH        (1<<7)
#define ADS1299_REG_GPIO_GPIOD3_LOW         (0<<6)
#define ADS1299_REG_GPIO_GPIOD3_HIGH        (1<<6)
#define ADS1299_REG_GPIO_GPIOD2_LOW         (0<<5)
#define ADS1299_REG_GPIO_GPIOD2_HIGH        (1<<5)
#define ADS1299_REG_GPIO_GPIOD1_LOW         (0<<4)
#define ADS1299_REG_GPIO_GPIOD1_HIGH        (1<<4)

/**
 *  \brief Bit mask definitions for GPIO.GPIOCn (GPIO level).
 *
 * The ADS1299 has 4 GPIO pins that can be manipulated via the SPI bus if there are not enough
 * GPIO pins available on the host.
 * GPIOC[4:1] controls the pin direction on GPIO pins 4:1.
 *
 * Consult the ADS1299 datasheet, page 46, for more information.
 */
#define ADS1299_REG_GPIO_GPIOC4_OUTPUT      (0<<3)
#define ADS1299_REG_GPIO_GPIOC4_INPUT       (1<<3)
#define ADS1299_REG_GPIO_GPIOC3_OUTPUT      (0<<2)
#define ADS1299_REG_GPIO_GPIOC3_INPUT       (1<<2)
#define ADS1299_REG_GPIO_GPIOC2_OUTPUT      (0<<1)
#define ADS1299_REG_GPIO_GPIOC2_INPUT       (1<<1)
#define ADS1299_REG_GPIO_GPIOC1_OUTPUT      (0<<0)
#define ADS1299_REG_GPIO_GPIOC1_INPUT       (1<<0)

/**
 *  \brief Combined value of reserved bits in GPIO register.
 *
 */
#define ADS1299_REG_GPIO_RESERVED_VALUE             0

/* MISC1 REGISTER ********************************************************************************************************************************/

/**
 *  \brief Bit mask definitions for MISC1.SRB1 (SRB1 internal connection).
 */
#define ADS1299_REG_MISC1_SRB1_OFF      (0<<5)          ///< Stim/ref/bias 1 turned off.
#define ADS1299_REG_MISC1_SRB1_ON       (1<<5)          ///< Stim/ref/bias 1 connected to all channel inverting inputs.

/**
 *  \brief Combined value of reserved bits in MISC1 register.
 *
 */
#define ADS1299_REG_MISC1_RESERVED_VALUE                0


/* MISC2 REGISTER ********************************************************************************************************************************/

/**
 *  \brief Combined value of reserved bits in MISC2 register.
 *
 * MISC2 don't do nothin' right now!
 * Consult the ADS1299 user's guide, page 46, for more information.
 */
#define ADS1299_REG_MISC2_RESERVED_VALUE                0


const byte WAKEUP = 0b00000010;     // Wake-up from standby mode
const byte STANDBY = 0b00000100;   // Enter Standby mode
const byte RESET = 0b00000110;   // Reset the device
const byte START = 0b00001000;   // Start and restart (synchronize) conversions
const byte STOP = 0b00001010;   // Stop conversion
const byte RDATAC = 0b00010000;   // Enable Read Data Continuous mode (default mode at power-up) 
const byte SDATAC = 0b00010001;   // Stop Read Data Continuous mode
const byte RDATA = 0b00010010;   // Read data by command; supports multiple read back

//Register Read Commands
const byte RREG = 0b00100000;
const byte WREG = 0b01000000;

const byte CH1 = 0x05;
const byte CH2 = 0x06;
const byte CH3 = 0x07;
const byte CH4 = 0x08;
const byte CH5 = 0x09;
const byte CH6 = 0x0A;
const byte CH7 = 0x0B;
const byte CH8 = 0x0C;
const byte CHn = 0xFF;
