/*
 Name:		RDA5807_FM_Tuner.h
 Created:	02/03/2019 2:49:05 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#ifndef _RDA5807_FM_Tuner_h
#define _RDA5807_FM_Tuner_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include "RdsDecoder.h"

#ifndef ENUM_CONVERSION
#define ENUM_CONVERSION 1

template<typename Enumerator>
uint8_t getEnumValue(const Enumerator& enumType)
{
	return static_cast<uint8_t>(enumType);
}

#endif // ENUM_CONVERSION

class RDA5807 final
{
public:
	/// <summary>
	/// Creates object for interfacing with RDA5807 and initializes data structures with default values.
	/// If some description of a method contains phrase "on the chip", then settings will be updated only on the chip.
	/// To update them locally, you need to read settings from the chip. Same with saving settings, you need to write them to the chip to take effect.
	/// </summary>
	/// <param name="enableRdsDecoder">true if RDS decoder has to be enabled, false otherwise</param>
	/// <param name="readRegisters">true if structures has to be initialized using data read from RDA5807, false otherwise</param>
	RDA5807(const bool& enableRdsDecoder = false, const bool& readRegisters = false)
	{
		if (readRegisters) readSettingsFromReceiver();
		if (enableRdsDecoder)
			m_rdsDecoder = new RdsDecoder
			(&m_rdaReadRegisters.reg0C.regValue,
				&m_rdaReadRegisters.reg0D.regValue,
				&m_rdaReadRegisters.reg0E.regValue,
				&m_rdaReadRegisters.reg0F.regValue);
	}

	RDA5807(const RDA5807&) = delete;
	RDA5807& operator=(const RDA5807&) = delete;

	~RDA5807() { delete m_rdsDecoder; }

#pragma region RDA enums
	/// <summary>
	/// Possible RDA5807 clock frequency values.
	/// </summary>
	enum class clkMode : uint8_t { clk32_768kHz, clk12Mhz, clk13Mhz, clk19_2Mhz, clk24Mhz = 5, clk26Mhz, clk38_4Mhz };
	/// <summary>
	/// Possible receiving bands values.
	/// </summary>
	enum class band : uint8_t { usEurope, japan, worldWide, eastEurope };
	/// <summary>
	/// Possible channel spacing values.
	/// </summary>
	enum class channelSpacing : uint8_t { spc100kHz, spc200kHz, spc50kHz, spc25kHz };
	/// <summary>
	/// Possible GPIO3 status values.
	/// </summary>
	enum class gpio3Status : uint8_t { highImpedance, monoStereoIndicator, low, high };
	/// <summary>
	/// Possible GPIO2 status values.
	/// </summary>
	enum class gpio2Status : uint8_t { highImpedance, interrupt, low, high };
	/// <summary>
	/// Possible GPIO1 status values.
	/// </summary>
	enum class gpio1Status : uint8_t { highImpedance, reserved, low, high };
	/// <summary>
	/// Possible LNA input options.
	/// </summary>
	enum class lnaInputSelection : uint8_t { noInput, lnaN, lnaP, dualInput };
	/// <summary>
	/// Possible LNA working current values.
	/// </summary>
	enum class lnaWorkingCurrent : uint8_t { i1_8mA, i2_1mA, i2_5mA, i3mA };
	/// <summary>
	/// Possible WS step values when in I2S master mode.
	/// </summary>
	enum class wsStep : uint8_t { s8kbps, s11_025kbps, s12kbps, s16kbps, s22_05kbps, s24kbps, s32kbps, s44_1kbps, s48kbps };
	/// <summary>
	/// Possible RDS block errors level values.
	/// </summary>
	enum class blockErrorLevel : uint8_t { bel0Errors, bel1to2Errors, bel3to5Errors, bel6AndMoreErrors };
#pragma endregion

private:
	RdsDecoder* m_rdsDecoder = nullptr;
#pragma region RDA write registers
	struct RDAWriteRegisters
	{
		union
		{
			uint16_t regValue;
			struct
			{
				uint8_t chipId;
				uint8_t chipModel;
			} regValues;
		} reg00 = { 0x5804 };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					powerUpEnable : 1,
					softReset : 1,
					newDemodulationMethod : 1,
					enableRDS : 1,
					clockMode : 3,
					seekMode : 1,
					seek : 1,
					seekDirection : 1,
					inputModeOfRCLK : 1,
					calibrationModeOfRCLK : 1,
					bassBoost : 1,
					monoOutput : 1,
					mute : 1,
					audioOutputDisable : 1;
			} regValues;
		} reg02 = { 0x0 };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					channelSpacing : 2,
					bandSelect : 2,
					tune : 1,
					directMode : 1,
					channelSelect : 10;
			} regValues;
		} reg03 = { 0x4FC0 };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					gpio1 : 2,
					gpio2 : 2,
					gpio3 : 2,
					i2sState : 1,
					: 1,
					autoFrequencyCorrection : 1,
					softMute : 1,
					rdsFifoClear : 1,
					deemphasis : 1,
					rdsFifoState : 1,
					rdsOnly : 1,
					seekTuneCompleteInterruptState : 1,
					: 1;
			} regValues;
		} reg04 = { 0x0400 };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					volume : 4,
					lnaWorkingCurrent : 2,
					lnaInputPort : 2,
					seekSnrThreshold : 4,
					: 1,
					seekMode : 2,
					interruptMode : 1;
			} regValues;
		} reg05 = { 0x888B };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					rChannelDataDelay : 1,
					lChannelDataDelay : 1,
					invertSclkOutWhenAsMaster : 1,
					invertWsOutputWhenAsMaster : 1,
					i2sWsStep : 4,
					normalWsEdge : 1,
					i2sSignedDataOutput : 1,
					normalSclkEdge : 1,
					invertWs : 1,
					i2sSlaveOrMaster : 1,
					openMode : 2,
					: 1;
			} regValues;
		} reg06 = { 0x0 };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					frequencySettingMode : 1,
					softBlend : 1,
					seekThresholdOldMode : 6,
					: 1,
					fm65m50mMode : 1,
					noiseSoftBlendThreshold : 5,
					: 1;
			} regValues;
		} reg07 = { 0x4202 };

		union
		{
			uint16_t regValue;
			struct
			{
				uint16_t directFrequencyValue;
			} regValues;
		} reg08 = { 0x0 };
	} m_rdaWriteRegisters;
#pragma endregion
#pragma region RDA read registers
	struct RDAReadRegisters
	{
		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					currentChannel : 10,
					stereoIndicator : 1,
					rdsBlockE : 1,
					rdsSynchronization : 1,
					seekFail : 1,
					seekTuneComplete : 1,
					rdsGroupReady : 1;
			} regValues;
		} reg0A = { 0x0400 };

		union
		{
			uint16_t regValue;
			struct
			{
				unsigned short
					rdsData1BlockErrorsLevel : 2,
					rdsData0BlockErrorsLevel : 2,
					rdsBlockId : 1,
					: 2,
					fmReady : 1,
					fmStation : 1,
					rssi : 7;
			} regValues;
		} reg0B = { 0x0 };

		union
		{
			uint16_t regValue;
			struct
			{
				uint16_t rdsBlockA;
			} regValues;
		} reg0C = { 0x5803 };

		union
		{
			uint16_t regValue;
			struct
			{
				uint16_t rdsBlockB;
			} regValues;
		} reg0D = { 0x5804 };

		union
		{
			uint16_t regValue;
			struct
			{
				uint16_t rdsBlockC;
			} regValues;
		} reg0E = { 0x5808 };

		union
		{
			uint16_t regValue;
			struct
			{
				uint16_t rdsBlockD;
			} regValues;
		} reg0F = { 0x5804 };
	} m_rdaReadRegisters;
#pragma endregion
#pragma region copy of RDA write registers
	/// <summary>
	/// Struct holding copies of data for write registers. These copies are used to check which register values were changed.
	/// </summary>
	struct
	{
		uint16_t reg02;
		uint16_t reg03;
		uint16_t reg04;
		uint16_t reg05;
		uint16_t reg06;
		uint16_t reg07;
		uint16_t reg08;
	} m_rdaWriteRegistersCheck =
	{
		m_rdaWriteRegisters.reg02.regValue,
		m_rdaWriteRegisters.reg03.regValue,
		m_rdaWriteRegisters.reg04.regValue,
		m_rdaWriteRegisters.reg05.regValue,
		m_rdaWriteRegisters.reg06.regValue,
		m_rdaWriteRegisters.reg07.regValue,
		m_rdaWriteRegisters.reg08.regValue
	};
#pragma endregion
private:
	/// <summary>
	/// Writes short to I2C slave.
	/// </summary>
	/// <param name="data">data to write</param>
	void i2cWriteShort(const uint16_t& data);

	/// <summary>
	/// Reads short from I2C slave.
	/// </summary>
	/// <returns>read data</returns>
	uint16_t i2cReadShort(void);

	/// <summary>
	/// Writes short to specified register.
	/// </summary>
	/// <param name="reg">destination register</param>
	/// <param name="value">data to write</param>
	void i2cWriteRegister(const uint8_t& reg, const uint16_t& value);

	/// <summary>
	/// Reads short from specified register.
	/// </summary>
	/// <param name="reg">source register</param>
	/// <returns>read data</returns>
	uint16_t i2cReadRegister(const uint8_t& reg);

public:
	/// <summary>
	/// Writes all settings to registers 0x02 to 0x08.
	/// </summary>
	void writeSettingsToReceiver(void);

	/// <summary>
	/// Writes settings only from modified registers to receiver.
	/// </summary>
	void writeModifiedRegistersToReceiver(void);

	/// <summary>
	/// Reads settings from registers 0x0A to 0x0F.
	/// It will update values only after a successful read operation.
	/// </summary>
	/// <returns>true if all data was received and updated locally, false otherwise</returns>
	bool readSettingsFromReceiver(void);

	/// <summary>
	/// Changes mute state.
	/// </summary>
	/// <param name="setting">true to mute, false to unmute</param>
	void updateMute(const bool& setting);

	/// <summary>
	/// Changes volume level. Min = 0, Max = 0xFF.
	/// </summary>
	/// <param name="value">volume level value</param>
	void updateVolumeLevel(const uint8_t& value);

	/// <summary>
	/// Changes received frequency. Min and Max depends on selected band. If passed value is out of selected band, then nothing is changed.
	/// Frequency is setted according to frequency setting mode (standard or direct).
	/// Pass value without decimal place, ex: 919 will set receiver to 91.9Mhz, 1080 will set frequency to 108Mhz etc.
	/// </summary>
	/// <param name="freq">frequency to set</param>
	/// <returns>true if change was made successfuly, false if nothing was changed</returns>
	bool updateReceivedFrequency(const uint16_t& freq);

	/// <summary>
	/// Updates RSSI value.
	/// </summary>
	void updateRssi(void);

	/// <summary>
	/// Starts seek operation.
	/// </summary>
	void updateSeek(void);

	/// <summary>
	/// Starts tune operation.
	/// </summary>
	void updateTune(void);

	/// <summary>
	/// Returns information about new RDS data avability.
	/// </summary>
	/// <returns>true if new data is ready, false otherwise</returns>
	bool checkIfNewRdsDataIsReady(void);

	/// <summary>
	/// Updates locally stored RDS data using data from receiver.
	/// </summary>
	void updateRdsData(void);

	/// <summary>
	/// Decodes locally stored RDS data and returns type of received RDS group.
	/// </summary>
	/// <returns>type of received RDS group. If RDA5807 was created without RDS data decoding option, then it will return groupType::none</returns>
	RdsDecoder::groupType updateDecodedRdsData(void);

	/// <summary>
	/// Returns pointer to object which contains decoded RDS data.
	/// </summary>
	/// <returns>pointer to RdsDecoder object with decoded RDS data. It will be nullptr if RDA5807 was created without RDS data decoding option</returns>
	const RdsDecoder* const getDecodedRdsData(void);

#pragma region registers get and set	
	/// <summary>
	/// Writes given value to locally stored register 00.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg00(const uint16_t& data) { m_rdaWriteRegisters.reg00.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 00.
	/// </summary>
	/// <returns>locally stored value of register 00</returns>
	const uint16_t& getReg00(void) { return m_rdaWriteRegisters.reg00.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 02.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg02(const uint16_t& data) { m_rdaWriteRegisters.reg02.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 02.
	/// </summary>
	/// <returns>locally stored value of register 02</returns>
	const uint16_t& getReg02(void) { return m_rdaWriteRegisters.reg02.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 03.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg03(const uint16_t& data) { m_rdaWriteRegisters.reg03.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 03.
	/// </summary>
	/// <returns>locally stored value of register 03</returns>
	const uint16_t& getReg03(void) { return m_rdaWriteRegisters.reg03.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 04.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg04(const uint16_t& data) { m_rdaWriteRegisters.reg04.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 04.
	/// </summary>
	/// <returns>locally stored value of register 04</returns>
	const uint16_t& getReg04(void) { return m_rdaWriteRegisters.reg04.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 05.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg05(const uint16_t& data) { m_rdaWriteRegisters.reg05.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 05.
	/// </summary>
	/// <returns>locally stored value of register 05</returns>
	const uint16_t& getReg05(void) { return m_rdaWriteRegisters.reg05.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 06.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg06(const uint16_t& data) { m_rdaWriteRegisters.reg06.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 06.
	/// </summary>
	/// <returns>locally stored value of register 06</returns>
	const uint16_t& getReg06(void) { return m_rdaWriteRegisters.reg06.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 07.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg07(const uint16_t& data) { m_rdaWriteRegisters.reg07.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 07.
	/// </summary>
	/// <returns>locally stored value of register 07</returns>
	const uint16_t& getReg07(void) { return m_rdaWriteRegisters.reg07.regValue; }

	/// <summary>
	/// Writes given value to locally stored register 08.
	/// </summary>
	/// <param name="data">value to write</param>
	void setReg08(const uint16_t& data) { m_rdaWriteRegisters.reg08.regValue = data; }
	/// <summary>
	/// Reads locally stored value of register 08.
	/// </summary>
	/// <returns>locally stored value of register 08</returns>
	const uint16_t& getReg08(void) { return m_rdaWriteRegisters.reg08.regValue; }

	/// <summary>
	/// Reads locally stored value of register 0A.
	/// </summary>
	/// <returns>locally stored value of register 0A</returns>
	const uint16_t& getReg0A(void) { return m_rdaReadRegisters.reg0A.regValue; }
	/// <summary>
	/// Reads locally stored value of register 0B.
	/// </summary>
	/// <returns>locally stored value of register 0B</returns>
	const uint16_t& getReg0B(void) { return m_rdaReadRegisters.reg0B.regValue; }
	/// <summary>
	/// Reads locally stored value of register 0C.
	/// </summary>
	/// <returns>locally stored value of register 0C</returns>
	const uint16_t& getReg0C(void) { return m_rdaReadRegisters.reg0C.regValue; }
	/// <summary>
	/// Reads locally stored value of register 0D.
	/// </summary>
	/// <returns>locally stored value of register 0D</returns>
	const uint16_t& getReg0D(void) { return m_rdaReadRegisters.reg0D.regValue; }
	/// <summary>
	/// Reads locally stored value of register 0E.
	/// </summary>
	/// <returns>locally stored value of register 0E</returns>
	const uint16_t& getReg0E(void) { return m_rdaReadRegisters.reg0E.regValue; }
	/// <summary>
	/// Reads locally stored value of register 0F.
	/// </summary>
	/// <returns>locally stored value of register 0F</returns>
	const uint16_t& getReg0F(void) { return m_rdaReadRegisters.reg0F.regValue; }
#pragma endregion
#pragma region reg02
	/// <summary>
	/// Enables or disables audio output by setting high impedance mode
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setAudioOutput(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.audioOutputDisable = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.audioOutputDisable = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of audio output.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getAudioOutput(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.audioOutputDisable) return true;
		else return false;
	}

	/// <summary>
	/// Enables or disables mute.
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setMute(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.mute = static_cast<uint8_t>(0);
		else m_rdaWriteRegisters.reg02.regValues.mute = static_cast<uint8_t>(1);
	}
	/// <summary>
	/// Returns state of mute setting.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getMute(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.mute) return false;
		else return true;
	}

	/// <summary>
	/// Sets mode of audio output.
	/// </summary>
	/// <param name="setting">true if mono, false if stereo</param>
	void setMonoOutput(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.monoOutput = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.monoOutput = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns mode of audio output.
	/// </summary>
	/// <returns>true if mono, false if stereo</returns>
	bool getMonoOutput(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.monoOutput) return true;
		else return false;
	}

	/// <summary>
	/// Sets bass boost function.
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setBassBoost(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.bassBoost = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.bassBoost = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of bass boost function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getBassBoost(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.bassBoost) return true;
		else return false;
	}

	/// <summary>
	/// Sets calibration mode of RCLK.
	/// </summary>
	/// <param name="setting">true if RCLK is always supplied, false otherwise</param>
	void setNonCalibrateModeOfRCLK(const bool& setting = false)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.calibrationModeOfRCLK = static_cast<uint8_t>(0);
		else m_rdaWriteRegisters.reg02.regValues.calibrationModeOfRCLK = static_cast<uint8_t>(1);
	}
	/// <summary>
	/// Returns calibration mode of RCLK.
	/// </summary>
	/// <returns>true if RCLK is always supplied, false otherwise</returns>
	bool getNonCalibrateModeOfRCLK(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.calibrationModeOfRCLK) return false;
		else return true;
	}

	/// <summary>
	/// Sets direct input mode of RCLK (RCLK supplied by source other than crystal oscillator).
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setDirectInputOfRCLK(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.inputModeOfRCLK = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.inputModeOfRCLK = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns direct input mode state of RCLK.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getDirectInputOfRCLK(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.inputModeOfRCLK) return true;
		else return false;
	}

	/// <summary>
	/// Sets stations seek direction.
	/// </summary>
	/// <param name="setting">true if seek up, false if seek down</param>
	void setSeekUp(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.seekDirection = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.seekDirection = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns stations seek direction.
	/// </summary>
	/// <returns>true if seek up, false if seek down</returns>
	bool getSeekUp(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.seekDirection) return true;
		else return false;
	}

	/// <summary>
	/// Starts seek in direction specified by setSeekUp and ends when a channel is found or entire band has been searched.
	/// This setting is set on the chip to false and getSeekTuneComplete is set to true when seek ends. 
	/// </summary>
	/// <param name="setting">true to start seek, false otherwise</param>
	void setSeek(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.seek = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.seek = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns seek state.
	/// </summary>
	/// <returns>true if seek in progress, false otherwise</returns>
	bool getSeek(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.seek) return true;
		else return false;
	}

	/// <summary>
	/// Sets seek mode of operation.
	/// </summary>
	/// <param name="setting">true to stop seeking at the upper or lower band limit, false to wrap and continue seeking</param>
	void setSeekModeStopAtBandLimit(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.seekMode = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.seekMode = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns seek mode of operation.
	/// </summary>
	/// <returns>true if seek has to stop at the upper or lower band limit, false to wrap and continue seeking</returns>
	bool getSeekModeStopAtBandLimit(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.seekMode) return true;
		else return false;
	}

	/// <summary>
	/// Sets CLK frequency.
	/// </summary>
	/// <param name="mode">frequency value of CLK at CLK input</param>
	void setClkMode(const clkMode mode = clkMode::clk32_768kHz) { m_rdaWriteRegisters.reg02.regValues.clockMode = getEnumValue(mode); }
	/// <summary>
	/// Returns setted CLK frequency value.
	/// </summary>
	/// <returns>frequency value</returns>
	clkMode getClkMode(void) { return static_cast<clkMode>(m_rdaWriteRegisters.reg02.regValues.clockMode); }

	/// <summary>
	/// Enables or disables RDS/RBDS function.
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setRds(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.enableRDS = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.enableRDS = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of RDS/RBDS function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getRds(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.enableRDS) return true;
		else return false;
	}

	/// <summary>
	/// Enables or disables new demodulation method (improves sensitivity about 1dB).
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setNewDemodulationMethod(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.newDemodulationMethod = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.newDemodulationMethod = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of new demodulation method selection.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getNewDemodulationMethod(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.newDemodulationMethod) return true;
		else return false;
	}

	/// <summary>
	/// Performs soft reset.
	/// </summary>
	/// <param name="setting">true to perform soft reset, false otherwise</param>
	void setSoftReset(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.softReset = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.softReset = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of soft reset function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getSoftReset(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.softReset) return true;
		else return false;
	}

	/// <summary>
	/// Sets state of receiver. After receiver is disabled, only I2C communication works, all other functions are disabled.
	/// </summary>
	/// <param name="setting">true to enable receiver, false to disable</param>
	void setEnable(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg02.regValues.powerUpEnable = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg02.regValues.powerUpEnable = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of receiver.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getEnable(void)
	{
		if (m_rdaWriteRegisters.reg02.regValues.powerUpEnable) return true;
		else return false;
	}
#pragma endregion
#pragma region reg03
	/// <summary>
	/// Sets channel fequency. Freq = channel spacing * channel + xMhz(depends on selected band). Min and Max depends on selected band.
	/// </summary>
	/// <param name="channel">selected channel frequency</param>
	void setChannel(const uint16_t& channel) { m_rdaWriteRegisters.reg03.regValues.channelSelect = channel; }
	/// <summary>
	/// Returns selected channel frequency. This setting is updated on the chip after a seek operation.
	/// </summary>
	/// <returns>selected channel frequency</returns>
	uint16_t getChannel(void) { return m_rdaWriteRegisters.reg03.regValues.channelSelect; }

	/// <summary>
	/// Sets direct control mode. Used only during testing.
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setDirectMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg03.regValues.directMode = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg03.regValues.directMode = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns status of direct control mode function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getDirectMode(void)
	{
		if (m_rdaWriteRegisters.reg03.regValues.directMode) return true;
		else return false;
	}

	/// <summary>
	/// Controls tune operation.
	/// </summary>
	/// <param name="setting">true to start, false otherwise</param>
	void setTune(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg03.regValues.tune = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg03.regValues.tune = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns status of tune operation. This value will change on the chip when operation completes.
	/// </summary>
	/// <returns>true if enabled or tunning, false otherwise</returns>
	bool getTune(void)
	{
		if (m_rdaWriteRegisters.reg03.regValues.tune) return true;
		else return false;
	}

	/// <summary>
	/// Sets selected band.
	/// </summary>
	/// <param name="setting">selected band</param>
	void setBand(const band setting = band::worldWide) { m_rdaWriteRegisters.reg03.regValues.bandSelect = getEnumValue(setting); }
	/// <summary>
	/// Returns selected band.
	/// </summary>
	/// <returns>selected band</returns>
	band getBand(void) { return static_cast<band>(m_rdaWriteRegisters.reg03.regValues.bandSelect); }

	/// <summary>
	/// Sets channel spacing.
	/// </summary>
	/// <param name="setting">channel spacing</param>
	void setChannelSpacing(const channelSpacing setting = channelSpacing::spc100kHz) { m_rdaWriteRegisters.reg03.regValues.channelSpacing = getEnumValue(setting); }
	/// <summary>
	/// Returns channel spacing.
	/// </summary>
	/// <returns>channel spacing</returns>
	channelSpacing getChannelSpacing(void) { return static_cast<channelSpacing>(m_rdaWriteRegisters.reg03.regValues.channelSpacing); }
#pragma endregion
#pragma region reg04
	/// <summary>
	/// Sets interrupt state after seek or tune operation completes.
	/// Setting this to true will generate a low pulse on GPIO2 when interrupt occurs.
	/// </summary>
	/// <param name="setting">true to enable interrupt, false otherwise</param>
	void setSeekTuneCompleteInterrupt(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.seekTuneCompleteInterruptState = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg04.regValues.seekTuneCompleteInterruptState = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of interrupt setting after seek or tune operation completes.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getSeekTuneCompleteInterrupt(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.seekTuneCompleteInterruptState) return true;
		else return false;
	}

	/// <summary>
	/// Sets RDS receiving mode.
	/// </summary>
	/// <param name="setting">true if RDS only, false if RDS and RBDS</param>
	void setRdsOnlyMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.rdsOnly = static_cast<uint8_t>(0);
		else m_rdaWriteRegisters.reg04.regValues.rdsOnly = static_cast<uint8_t>(1);
	}
	/// <summary>
	/// Returns RDS receiving mode.
	/// </summary>
	/// <returns>true if RDS only, false if RDS and RBDS</returns>
	bool getRdsOnlyMode(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.rdsOnly) return false;
		else return true;
	}

	/// <summary>
	/// Sets RDS data output mode to FIFO.
	/// </summary>
	/// <param name="setting">true if FIFO, false if FILO</param>
	void setRdsFifoMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.rdsFifoState = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg04.regValues.rdsFifoState = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns mode of RDS data output.
	/// </summary>
	/// <returns>true if FIFO, false if FILO</returns>
	bool getRdsFifoMode(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.rdsFifoState) return true;
		else return false;
	}

	/// <summary>
	/// Sets short de-emphasis time constant.
	/// </summary>
	/// <param name="setting">true if 50us, false if 75us</param>
	void setShortDeemphasis(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.deemphasis = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg04.regValues.deemphasis = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns selected de-emphasis time constant.
	/// </summary>
	/// <returns>true if 50us, false if 75us</returns>
	bool getShortDeemphasis(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.deemphasis) return true;
		else return false;
	}

	/// <summary>
	/// Clears RDS FIFO buffer.
	/// </summary>
	/// <param name="setting">true to clear, false otherwise</param>
	void setRdsFifoClear(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.rdsFifoClear = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg04.regValues.rdsFifoClear = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of RDS FIFO buffer clear function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getRdsFifoClear(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.rdsFifoClear) return true;
		else return false;
	}

	/// <summary>
	/// Sets state of soft mute function (mute if signal strength is too low).
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setSoftMute(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.softMute = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg04.regValues.softMute = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of soft mute function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getSoftMute(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.softMute) return true;
		else return false;
	}

	/// <summary>
	/// Sets state of AFC (auto frequency correction) function.
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setAutoFrequencyCorrection(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.autoFrequencyCorrection = static_cast<uint8_t>(0);
		else m_rdaWriteRegisters.reg04.regValues.autoFrequencyCorrection = static_cast<uint8_t>(1);
	}
	/// <summary>
	/// Returns state of AFC (auto frequency correction) function.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getAutoFrequencyCorrection(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.autoFrequencyCorrection) return false;
		else return true;
	}

	/// <summary>
	/// Sets state of I2S.
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setI2s(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg04.regValues.i2sState = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg04.regValues.i2sState = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of I2S.
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getI2s(void)
	{
		if (m_rdaWriteRegisters.reg04.regValues.i2sState) return true;
		else return false;
	}

	/// <summary>
	/// Sets state of GPIO3.
	/// </summary>
	/// <param name="state">GPIO3 state to set</param>
	void setGpio3(gpio3Status state) { m_rdaWriteRegisters.reg04.regValues.gpio3 = getEnumValue(state); }
	/// <summary>
	/// Returns state of GPIO3.
	/// </summary>
	/// <returns>GPIO3 state</returns>
	gpio3Status getGpio3(void) { return static_cast<gpio3Status>(m_rdaWriteRegisters.reg04.regValues.gpio3); }

	/// <summary>
	/// Sets state of GPIO2.
	/// </summary>
	/// <param name="state">GPIO2 state to set</param>
	void setGpio2(gpio2Status state) { m_rdaWriteRegisters.reg04.regValues.gpio2 = getEnumValue(state); }
	/// <summary>
	/// Returns state of GPIO2.
	/// </summary>
	/// <returns>GPIO2 state</returns>
	gpio2Status getGpio2(void) { return static_cast<gpio2Status>(m_rdaWriteRegisters.reg04.regValues.gpio2); }

	/// <summary>
	/// Sets state of GPIO1.
	/// </summary>
	/// <param name="state">GPIO1 state to set</param>
	void setGpio1(gpio1Status state) { m_rdaWriteRegisters.reg04.regValues.gpio1 = getEnumValue(state); }
	/// <summary>
	/// Returns state of GPIO1.
	/// </summary>
	/// <returns>GPIO1 state</returns>
	gpio1Status getGpio1(void) { return static_cast<gpio1Status>(m_rdaWriteRegisters.reg04.regValues.gpio1); }
#pragma endregion
#pragma region reg05
	/// <summary>
	/// Sets mode of GPIO2 interrupt generation.
	/// </summary>
	/// <param name="setting">true to generate 5ms interrupt, false to generate interrupt until read of 0x0C register occurs</param>
	void set5msInterruptMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg05.regValues.interruptMode = static_cast<uint8_t>(0);
		else m_rdaWriteRegisters.reg05.regValues.interruptMode = static_cast<uint8_t>(1);
	}
	/// <summary>
	/// Returns mode of GPIO2 interrupt generation.
	/// </summary>
	/// <returns>true if generate 5ms interrupt, false if generate interrupt until read of 0x0C register occurs</returns>
	bool get5msInterruptMode(void)
	{
		if (m_rdaWriteRegisters.reg05.regValues.interruptMode) return false;
		else return true;
	}

	/// <summary>
	/// Sets seek mode.
	/// </summary>
	/// <param name="setting">true to set RSSI seek mode, false otherwise</param>
	void setRssiSeekMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg05.regValues.seekMode = static_cast<uint8_t>(2);
		else m_rdaWriteRegisters.reg05.regValues.seekMode = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns seek mode.
	/// </summary>
	/// <returns>true if RSSI seek mode, false otherwise</returns>
	bool getRssiSeekMode(void)
	{
		if (m_rdaWriteRegisters.reg05.regValues.seekMode) return true;
		else return false;
	}

	/// <summary>
	/// Sets seek SNR (signal to noise ratio) threshold value. Min = 0, Max = 0x0F.
	/// </summary>
	/// <param name="threshold">SNR threshold value</param>
	void setSeekSnrThreshold(const uint8_t& threshold) { m_rdaWriteRegisters.reg05.regValues.seekSnrThreshold = threshold; }
	/// <summary>
	/// Returns seek SNR (signal to noise ratio) threshold value.
	/// </summary>
	/// <returns>SNR threshold value</returns>
	uint8_t getSeekSnrThreshold(void) { return m_rdaWriteRegisters.reg05.regValues.seekSnrThreshold; }

	/// <summary>
	/// Sets LNA (low noise amplifier) input source.
	/// </summary>
	/// <param name="setting">source to set as input</param>
	void setLnaInputSource(lnaInputSelection setting = lnaInputSelection::dualInput) { m_rdaWriteRegisters.reg05.regValues.lnaInputPort = getEnumValue(setting); }
	/// <summary>
	/// Returns LNA (low noise amplifier) input source.
	/// </summary>
	/// <returns>source which been setted as input</returns>
	lnaInputSelection getLnaInputSource(void) { return static_cast<lnaInputSelection>(m_rdaWriteRegisters.reg05.regValues.lnaInputPort); }

	/// <summary>
	/// Sets LNA (low noise amplifier) working current.
	/// </summary>
	/// <param name="setting">current to set</param>
	void setLnaWorkingCurrent(lnaWorkingCurrent setting = lnaWorkingCurrent::i3mA) { m_rdaWriteRegisters.reg05.regValues.lnaWorkingCurrent = getEnumValue(setting); }
	/// <summary>
	/// Returns LNA (low noise amplifier) working current.
	/// </summary>
	/// <returns>setted current</returns>
	lnaWorkingCurrent getLnaWorkingCurrent(void) { return static_cast<lnaWorkingCurrent>(m_rdaWriteRegisters.reg05.regValues.lnaWorkingCurrent); }

	/// <summary>
	/// Sets volume level. Volume scale is logarithmic. Min = 0, Max = 0x0F. Output mute enables when setted to min value.
	/// </summary>
	/// <param name="value">volume level</param>
	void setVolume(const uint8_t& value) { m_rdaWriteRegisters.reg05.regValues.volume = value; }
	/// <summary>
	/// Returns setted volume level.
	/// </summary>
	/// <returns>volume level</returns>
	uint8_t getVolume(void) { return m_rdaWriteRegisters.reg05.regValues.volume; }
#pragma endregion
#pragma region reg06
	/// <summary>
	/// Enables open reserved registers mode.
	/// </summary>
	/// <param name="setting">true if open for writing, false if open for reading</param>
	void setOpenReservedRegistersForWriting(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.openMode = static_cast<uint8_t>(3);
		else m_rdaWriteRegisters.reg06.regValues.openMode = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns status of open reserved registers mode.
	/// </summary>
	/// <returns>true if open for writing, false if open for reading</returns>
	bool getOpenReservedRegistersForWriting(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.openMode) return true;
		else return false;
	}

	/// <summary>
	/// Sets work mode of I2S.
	/// </summary>
	/// <param name="setting">true if slave, false if master</param>
	void setI2sSlaveMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.i2sSlaveOrMaster = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.i2sSlaveOrMaster = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns work mode of I2S.
	/// </summary>
	/// <returns>true if slave, false if master</returns>
	bool getI2sSlaveMode(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.i2sSlaveOrMaster) return true;
		else return false;
	}

	/// <summary>
	/// Sets how WS will signal L and R channel.
	/// </summary>
	/// <param name="setting">true if WS=0 -> R and WS=1 -> L, false if WS=0 -> L and WS=1 -> R</param>
	void setInvertWsSignaling(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.invertWs = static_cast<uint8_t>(0);
		else m_rdaWriteRegisters.reg06.regValues.invertWs = static_cast<uint8_t>(1);
	}
	/// <summary>
	/// Returns state of WS signaling.
	/// </summary>
	/// <returns>true if WS=0 -> R and WS=1 -> L, false if WS=0 -> L and WS=1 -> R</returns>
	bool getInvertWsSignaling(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.invertWs) return false;
		else return true;
	}

	/// <summary>
	/// Sets state of SCLK internal signaling.
	/// </summary>
	/// <param name="setting">true if SCLK needs to be inverted internally, false otherwise</param>
	void setInvertInternalSclk(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.normalSclkEdge = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.normalSclkEdge = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of SCLK internal signaling.
	/// </summary>
	/// <returns>true if SCLK will be inverted internally, false otherwise</returns>
	bool getInvertInternalSclk(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.normalSclkEdge) return true;
		else return false;
	}

	/// <summary>
	/// Sets whether I2S audio output data will be signed or unsigned 16 bit integer.
	/// </summary>
	/// <param name="setting">true if signed, false if unsigned</param>
	void setI2sSignedDataOutput(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.i2sSignedDataOutput = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.i2sSignedDataOutput = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether I2S audio output data is signed or unsigned 16 bit integer.
	/// </summary>
	/// <returns>true if signed, false if unsigned</returns>
	bool getI2sSignedDataOutput(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.i2sSignedDataOutput) return true;
		else return false;
	}

	/// <summary>
	/// Sets state of WS internal signaling.
	/// </summary>
	/// <param name="setting">true if WS needs to be inverted internally, false otherwise</param>
	void setInvertInternalWs(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.normalWsEdge = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.normalWsEdge = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of WS internal signaling.
	/// </summary>
	/// <returns>true if WS will be inverted internally, false otherwise</returns>
	bool getInvertInternalWs(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.normalWsEdge) return true;
		else return false;
	}

	/// <summary>
	/// Sets WS step (valid only in master mode).
	/// </summary>
	/// <param name="setting">WS step to set</param>
	void setWsStep(wsStep setting = wsStep::s44_1kbps) { m_rdaWriteRegisters.reg06.regValues.i2sWsStep = getEnumValue(setting); }
	/// <summary>
	/// Returns WS step (valid only in master mode).
	/// </summary>
	/// <returns>setted WS step</returns>
	wsStep getWsStep(void) { return static_cast<wsStep>(m_rdaWriteRegisters.reg06.regValues.i2sWsStep); }

	/// <summary>
	/// Sets whether WS output signal will be inverted (valid only in master mode).
	/// </summary>
	/// <param name="setting">true if it need to be inverted, false otherwise</param>
	void setInvertOutputWs(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.invertWsOutputWhenAsMaster = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.invertWsOutputWhenAsMaster = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether WS output signal will be inverted (valid only in master mode).
	/// </summary>
	/// <returns>true when inverted, false otherwise</returns>
	bool getInvertOutputWs(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.invertWsOutputWhenAsMaster) return true;
		else return false;
	}

	/// <summary>
	/// Sets whether SCLK output signal will be inverted (valid only in master mode).
	/// </summary>
	/// <param name="setting">true if it need to be inverted, false otherwise</param>
	void setInvertOutputSclk(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.invertSclkOutWhenAsMaster = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.invertSclkOutWhenAsMaster = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether SCLK output signal will be inverted (valid only in master mode).
	/// </summary>
	/// <returns>true when inverted, false otherwise</returns>
	bool getInvertOutputSclk(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.invertSclkOutWhenAsMaster) return true;
		else return false;
	}

	/// <summary>
	/// Sets L channel data delay.
	/// </summary>
	/// <param name="setting">true to set data delay of 1T, false otherwise</param>
	void setLChannelDataDelay(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.lChannelDataDelay = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.lChannelDataDelay = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether L channel data is delayed.
	/// </summary>
	/// <returns>true if it is delayed, false otherwise</returns>
	bool getLChannelDataDelay(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.lChannelDataDelay) return true;
		else return false;
	}

	/// <summary>
	/// Sets R channel data delay.
	/// </summary>
	/// <param name="setting">true to set data delay of 1T, false otherwise</param>
	void setRChannelDataDelay(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg06.regValues.rChannelDataDelay = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg06.regValues.rChannelDataDelay = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether R channel data is delayed.
	/// </summary>
	/// <returns>true if it is delayed, false otherwise</returns>
	bool getRChannelDataDelay(void)
	{
		if (m_rdaWriteRegisters.reg06.regValues.rChannelDataDelay) return true;
		else return false;
	}
#pragma endregion
#pragma region reg07
	/// <summary>
	/// Sets threshold for noise soft blend function. Unit 2dB. Min = 0, Max = 0x1F.
	/// </summary>
	/// <param name="threshold">threshold to set</param>
	void setNoiseSoftBlendThreshold(const uint8_t& threshold) { m_rdaWriteRegisters.reg07.regValues.noiseSoftBlendThreshold = threshold; }
	/// <summary>
	/// Returns threshold value for noise soft blend function. Unit 2dB.
	/// </summary>
	/// <returns>threshold value</returns>
	uint8_t getNoiseSoftBlendThreshold(void) { return m_rdaWriteRegisters.reg07.regValues.noiseSoftBlendThreshold; }

	/// <summary>
	/// Sets 65Mhz-76Mhz band or 50Mhz-76Mhz band. Valid only when selected band is East Europe.
	/// </summary>
	/// <param name="setting">true to set 65Mhz-76Mhz band, false to set 50Mhz-76Mhz</param>
	void set65mMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg07.regValues.fm65m50mMode = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg07.regValues.fm65m50mMode = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether selected band is 65Mhz-76Mhz or 50Mhz-76Mhz.
	/// </summary>
	/// <returns>true if 65Mhz-76Mhz band is set, false if 50Mhz-76Mhz band is set</returns>
	bool get65mMode(void)
	{
		if (m_rdaWriteRegisters.reg07.regValues.fm65m50mMode) return true;
		else return false;
	}

	/// <summary>
	/// Sets seek threshold for old seek mode (RSSI seek). Valid only when RSSI seek mode was chosen. Min = 0, Max = 0x3F.
	/// </summary>
	/// <param name="threshold">seek threshold</param>
	void setOldSeekSnrThreshold(const uint8_t& threshold) { m_rdaWriteRegisters.reg07.regValues.seekThresholdOldMode = threshold; }
	/// <summary>
	/// Returns value of seek threshold for old seek mode (RSSI seek).
	/// </summary>
	/// <returns>seek threshold value</returns>
	uint8_t getOldSeekSnrThreshold(void) { return m_rdaWriteRegisters.reg07.regValues.seekThresholdOldMode; }

	/// <summary>
	/// Sets state of soft blend function (mute when signal strength is too low).
	/// </summary>
	/// <param name="setting">true to enable, false to disable</param>
	void setSoftBlend(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg07.regValues.softBlend = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg07.regValues.softBlend = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns state of soft blend function (mute when signal strength is too low).
	/// </summary>
	/// <returns>true if enabled, false if disabled</returns>
	bool getSoftBlend(void)
	{
		if (m_rdaWriteRegisters.reg07.regValues.softBlend) return true;
		else return false;
	}

	/// <summary>
	/// Sets alternative frequency setting mode. Freq = 76000(or 87000)kHz + setFrequencyDirectly(freq)(register 0x08)kHz.
	/// </summary>
	/// <param name="setting">true to change setting mode, false to set default</param>
	void setAlternativeFrequencySettingMode(const bool& setting = true)
	{
		if (setting) m_rdaWriteRegisters.reg07.regValues.frequencySettingMode = static_cast<uint8_t>(1);
		else m_rdaWriteRegisters.reg07.regValues.frequencySettingMode = static_cast<uint8_t>(0);
	}
	/// <summary>
	/// Returns whether alternative frequency setting mode is enabled.
	/// </summary>
	/// <returns>true if alternative setting mode is enabled, false if default mode is enabled</returns>
	bool getAlternativeFrequencySettingMode(void)
	{
		if (m_rdaWriteRegisters.reg07.regValues.frequencySettingMode) return true;
		else return false;
	}
#pragma endregion
#pragma region reg08
	/// <summary>
	/// Sets frequency value in alternative frequency setting mode. Freq = 76000(or 87000)kHz + setFrequencyDirectly(freq)kHz.
	/// </summary>
	/// <param name="freq">frequency value</param>
	void setFrequencyDirectly(const uint16_t& freq) { m_rdaWriteRegisters.reg08.regValues.directFrequencyValue = freq; }
	/// <summary>
	/// Returns setted frequency value for alternative frequency setting mode.
	/// </summary>
	/// <returns>setted frequency value</returns>
	const uint16_t& getFrequencyDirectly(void) { return m_rdaWriteRegisters.reg08.regValues.directFrequencyValue; }
#pragma endregion
#pragma region reg0A
	/// <summary>
	/// Returns whether new RDS/RBDS group is ready.
	/// </summary>
	/// <returns>true if new group is ready, false otherwise</returns>
	bool getRdsGroupState(void)
	{
		if (m_rdaReadRegisters.reg0A.regValues.rdsGroupReady) return true;
		else return false;
	}

	/// <summary>
	/// Returns whether seek/tune operation completed.
	/// </summary>
	/// <returns>true if seek/tune completed, false otherwise</returns>
	bool getSeekTuneComplete(void)
	{
		if (m_rdaReadRegisters.reg0A.regValues.seekTuneComplete) return true;
		else return false;
	}

	/// <summary>
	/// Returns true if seek operation fails to find a channel with an RSSI level greater than setted in setSeekSnrThreshold.
	/// </summary>
	/// <returns>true if seek fails, false otherwise</returns>
	bool getSeekFail(void)
	{
		if (m_rdaReadRegisters.reg0A.regValues.seekFail) return true;
		else return false;
	}

	/// <summary>
	/// Returns synchronization state of RDS decoder.
	/// </summary>
	/// <returns>true if RDS decoder is synchronized, false otherwise</returns>
	bool getRdsSynchronizationState(void)
	{
		if (m_rdaReadRegisters.reg0A.regValues.rdsSynchronization) return true;
		else return false;
	}

	/// <summary>
	/// Returns information if RDS Block E was found.
	/// </summary>
	/// <returns>true if RDS Block E was found, false otherwise</returns>
	bool getBlockEStatus(void)
	{
		if (m_rdaReadRegisters.reg0A.regValues.rdsBlockE) return true;
		else return false;
	}

	/// <summary>
	/// Returns information if station is received in stereo or mono.
	/// </summary>
	/// <returns>true if stereo, false if mono</returns>
	bool getStereoIndicator(void)
	{
		if (m_rdaReadRegisters.reg0A.regValues.stereoIndicator) return true;
		else return false;
	}

	/// <summary>
	/// Returns currently received frequency.
	/// </summary>
	/// <returns>frequency value</returns>
	uint16_t getCurrentFrequency(void) { return m_rdaReadRegisters.reg0A.regValues.currentChannel; }
#pragma endregion
#pragma region reg0B
	/// <summary>
	/// Returns current RSSI (radio signal strength indicator) value. RSSI scale is logarithmic.
	/// </summary>
	/// <returns>RSSI value</returns>
	uint8_t getRssi(void) { return m_rdaReadRegisters.reg0B.regValues.rssi; }

	/// <summary>
	/// Returns state of selected channel.
	/// </summary>
	/// <returns>true if channel is a station, false otherwise</returns>
	bool getFmStationState(void)
	{
		if (m_rdaReadRegisters.reg0B.regValues.fmStation) return true;
		else return false;
	}

	/// <summary>
	/// Returns readiness state of the receiver.
	/// </summary>
	/// <returns>true if it is ready, false otherwise</returns>
	bool getFmReadinessState(void)
	{
		if (m_rdaReadRegisters.reg0B.regValues.fmReady) return true;
		else return false;
	}

	/// <summary>
	/// Returns whether registers 0x0C, 0x0D, 0x0E, 0x0F contain RDS Block E or blocks A, B, C, D.
	/// </summary>
	/// <returns>true if RDS registers contain only Block E, false if these registers contain blocks A, B, C, D</returns>
	bool getRdsBlockERegistersState(void)
	{
		if (m_rdaReadRegisters.reg0B.regValues.rdsBlockId) return true;
		else return false;
	}

	/// <summary>
	/// Returns block errors level of RDS Data 0 which is read as error level of RDS Block A in RDS mode or error level of Block E in RBDS mode.
	/// </summary>
	/// <returns>block errors level</returns>
	blockErrorLevel getBlockErrorsLevelOfRdsData0(void) { return static_cast<blockErrorLevel>(m_rdaReadRegisters.reg0B.regValues.rdsData0BlockErrorsLevel); }

	/// <summary>
	/// Returns block errors level of RDS Data 1 which is read as error level of RDS Block B in RDS mode or error level of Block E in RBDS mode.
	/// </summary>
	/// <returns>block errors level</returns>
	blockErrorLevel getBlockErrorsLevelOfRdsData1(void) { return static_cast<blockErrorLevel>(m_rdaReadRegisters.reg0B.regValues.rdsData1BlockErrorsLevel); }
#pragma endregion
#pragma region RDS data
	/// <summary>
	/// Returns RDS Block A data in RDS mode or RDS Block E in RBDS mode.
	/// </summary>
	/// <returns>RDS data</returns>
	const uint16_t& getRdsData0(void) { return m_rdaReadRegisters.reg0C.regValues.rdsBlockA; }

	/// <summary>
	/// Returns RDS Block B data in RDS mode or RDS Block E in RBDS mode.
	/// </summary>
	/// <returns>RDS data</returns>
	const uint16_t& getRdsData1(void) { return m_rdaReadRegisters.reg0D.regValues.rdsBlockB; }

	/// <summary>
	/// Returns RDS Block C data in RDS mode or RDS Block E in RBDS mode.
	/// </summary>
	/// <returns>RDS data</returns>
	const uint16_t& getRdsData2(void) { return m_rdaReadRegisters.reg0E.regValues.rdsBlockC; }

	/// <summary>
	/// Returns RDS Block D data in RDS mode or RDS Block E in RBDS mode.
	/// </summary>
	/// <returns>RDS data</returns>
	const uint16_t& getRdsData3(void) { return m_rdaReadRegisters.reg0F.regValues.rdsBlockD; }
#pragma endregion
};

#endif

