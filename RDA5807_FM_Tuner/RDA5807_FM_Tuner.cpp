/*
 Name:		RDA5807_FM_Tuner.cpp
 Created:	02/03/2019 2:49:05 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#include "RDA5807_FM_Tuner.h"

void RDA5807::i2cWriteShort(const uint16_t& data)
{
	Wire.write(static_cast<uint8_t>((data & 0xFF00) >> 8));
	Wire.write(static_cast<uint8_t>(data & 0x00FF));
}

uint16_t RDA5807::i2cReadShort(void)
{
	uint16_t data = Wire.read();
	data <<= 8;
	data |= Wire.read();
	return data;
}

void RDA5807::i2cWriteRegister(const uint8_t& reg, const uint16_t& value)
{
	Wire.beginTransmission(0x11);
	Wire.write(reg);
	i2cWriteShort(value);
	Wire.endTransmission();
}

uint16_t RDA5807::i2cReadRegister(const uint8_t& reg)
{
	Wire.beginTransmission(0x11);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(0x11, 2);
	return i2cReadShort();
}

void RDA5807::writeSettingsToReceiver(void)
{
	Wire.beginTransmission(0x10);
	i2cWriteShort(m_rdaWriteRegisters.reg02.regValue);
	i2cWriteShort(m_rdaWriteRegisters.reg03.regValue);
	i2cWriteShort(m_rdaWriteRegisters.reg04.regValue);
	i2cWriteShort(m_rdaWriteRegisters.reg05.regValue);
	i2cWriteShort(m_rdaWriteRegisters.reg06.regValue);
	i2cWriteShort(m_rdaWriteRegisters.reg07.regValue);
	i2cWriteShort(m_rdaWriteRegisters.reg08.regValue);
	Wire.endTransmission();
}

void RDA5807::writeModifiedRegistersToReceiver(void)
{
	uint16_t* const writeRegs[] =
	{
		&m_rdaWriteRegisters.reg02.regValue,
		&m_rdaWriteRegisters.reg03.regValue,
		&m_rdaWriteRegisters.reg04.regValue,
		&m_rdaWriteRegisters.reg05.regValue,
		&m_rdaWriteRegisters.reg06.regValue,
		&m_rdaWriteRegisters.reg07.regValue,
		&m_rdaWriteRegisters.reg08.regValue
	};
	uint16_t* const writeRegsCheck[] =
	{
		&m_rdaWriteRegistersCheck.reg02,
		&m_rdaWriteRegistersCheck.reg03,
		&m_rdaWriteRegistersCheck.reg04,
		&m_rdaWriteRegistersCheck.reg05,
		&m_rdaWriteRegistersCheck.reg06,
		&m_rdaWriteRegistersCheck.reg07,
		&m_rdaWriteRegistersCheck.reg08
	};

	for (uint8_t i = 0; i < 7; i++)
	{
		if (*writeRegsCheck[i] != *writeRegs[i])//check if value has changed
		{
			i2cWriteRegister(i, *writeRegs[i]);//update value in receiver
			*writeRegsCheck[i] = *writeRegs[i];//update value in check struct
		}
	}
}

bool RDA5807::readSettingsFromReceiver(void)
{
	Wire.requestFrom(0x10, 12);//6 registers, two bytes each
	if (Wire.available() != 12) return false;
	m_rdaReadRegisters.reg0A.regValue = i2cReadShort();
	m_rdaReadRegisters.reg0B.regValue = i2cReadShort();
	m_rdaReadRegisters.reg0C.regValue = i2cReadShort();
	m_rdaReadRegisters.reg0D.regValue = i2cReadShort();
	m_rdaReadRegisters.reg0E.regValue = i2cReadShort();
	m_rdaReadRegisters.reg0F.regValue = i2cReadShort();
	return true;
}

void RDA5807::updateMute(const bool& setting)
{
	setMute(setting);

	i2cWriteRegister(0x02, m_rdaWriteRegisters.reg02.regValue);
}

void RDA5807::updateVolumeLevel(const uint8_t& value)
{
	uint8_t level = 0;

	if (value) level = static_cast<uint8_t>(value / 0x10);
	setVolume(level);

	i2cWriteRegister(0x05, m_rdaWriteRegisters.reg05.regValue);
}

bool RDA5807::updateReceivedFrequency(const uint16_t& freq)
{
	uint8_t channelSpacingValue = 0;
	uint16_t channel = 0;
	bool otherEastEuropeBand = !get65mMode();

	if (otherEastEuropeBand && (freq < 500)) return false;
	if (freq < 650 || freq > 1080) return false;//freq value can't be set outside working range of receiver

	switch (getBand())
	{
	case band::usEurope:
		if (freq == 870) break;
		channel = (freq - 870) * 100;
		if (channel > 21000) return false;
		break;
	case band::japan:
		if (freq == 760) break;
		channel = (freq - 760) * 100;
		if (channel > 15000) return false;
		break;
	case band::worldWide:
		if (channel == 760) break;
		channel = (freq - 760) * 100;
		if (channel > 32000) return false;
		break;
	case band::eastEurope:
		if (otherEastEuropeBand) { if (freq == 500) break; channel = (freq - 500) * 100; if (channel > 26000) return false; }
		else { if (freq == 650) break; channel = (freq - 650) * 100; if (channel > 11000) return false; }
		break;
	}

	switch (getChannelSpacing())
	{
	case channelSpacing::spc25kHz:
		channelSpacingValue = 25;
		break;
	case channelSpacing::spc50kHz:
		channelSpacingValue = 50;
		break;
	case channelSpacing::spc100kHz:
		channelSpacingValue = 100;
		break;
	case channelSpacing::spc200kHz:
		channelSpacingValue = 200;
		break;
	}

	if (getAlternativeFrequencySettingMode())
	{//freq = min band freq kHz + freq direct kHz
		setFrequencyDirectly(channel);

		i2cWriteRegister(0x07, m_rdaWriteRegisters.reg07.regValue);//write this again to be able to receive music instead of hum
		i2cWriteRegister(0x08, m_rdaWriteRegisters.reg08.regValue);
	}
	else
	{//standard freq setting mode
		if (channel) channel /= channelSpacingValue;
		setChannel(channel);

		i2cWriteRegister(0x02, m_rdaWriteRegisters.reg02.regValue);//receiving will start working after sending this register second time here
		i2cWriteRegister(0x03, m_rdaWriteRegisters.reg03.regValue);

		do { m_rdaReadRegisters.reg0A.regValue = i2cReadRegister(0x0A); updateTune(); } while (!getSeekTuneComplete());//wait for receiver to tune
	}

	return true;
}

void RDA5807::updateRssi(void)
{
	m_rdaReadRegisters.reg0B.regValue = i2cReadRegister(0x0B);
}

void RDA5807::updateSeek(void)
{
	setSeek();

	i2cWriteRegister(0x02, m_rdaWriteRegisters.reg02.regValue);
}

void RDA5807::updateTune(void)
{
	setTune();

	i2cWriteRegister(0x03, m_rdaWriteRegisters.reg03.regValue);
}

bool RDA5807::checkIfNewRdsDataIsReady(void)
{
	m_rdaReadRegisters.reg0A.regValue = i2cReadRegister(0x0A);
	return getRdsGroupState();
}

void RDA5807::updateRdsData(void)
{
	m_rdaReadRegisters.reg0C.regValue = i2cReadRegister(0x0C);
	m_rdaReadRegisters.reg0D.regValue = i2cReadRegister(0x0D);
	m_rdaReadRegisters.reg0E.regValue = i2cReadRegister(0x0E);
	m_rdaReadRegisters.reg0F.regValue = i2cReadRegister(0x0F);
}

RdsDecoder::groupType RDA5807::updateDecodedRdsData(void)
{
	if (m_rdsDecoder != nullptr) return m_rdsDecoder->decodeReceivedData();
	return RdsDecoder::groupType::none;
}

const RdsDecoder* const RDA5807::getDecodedRdsData(void)
{
	if (m_rdsDecoder != nullptr) return m_rdsDecoder;
	return nullptr;
}
