/*
 Name:		RdsDecoder.cpp
 Created:	11/03/2019 2:06:52 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#include "RdsDecoder.h"

RdsDecoder::groupType RdsDecoder::decodeReceivedData(void)
{
	setCountryCode();
	setProgrammeAreaCoverage();
	setProgrammeReferenceNumber();
	setTrafficProgramme();
	setProgrammeTypeCode();
	//above are common to all groups
	switch (getGroupTypeCode())
	{
	case groupType::g0A:
		setProgrammeServiceName();
		return groupType::g0A;

	case groupType::g0B:
		setProgrammeServiceName();
		return groupType::g0B;

	case groupType::g1A:
		setExtendedCountryCode();
		setProgrammeItemNumberCode();
		setLanguageCode();
		return groupType::g1A;

	case groupType::g1B:
		setProgrammeItemNumberCode();
		return groupType::g1B;

	case groupType::g2A:
		prepareRadioText();
		setRadioText2A();
		return groupType::g2A;

	case groupType::g2B:
		prepareRadioText();
		setRadioText2B();
		return groupType::g2B;

	case groupType::g4A:
		prepareTimeAndDate();
		setDate();
		return groupType::g4A;

	case groupType::g10A:
		setProgrammeTypeName();
		return groupType::g10A;
	}
}

void RdsDecoder::setProgrammeServiceName(void)
{
	uint8_t segmentAddress = static_cast<uint8_t>(*m_rdsDataBlocks.blockB & 0x0003);
	if (segmentAddress == 0)
	{
		m_group0.decoderControlBits = 0;//reset decoder identification
		memset(m_group0.programmeServiceName, 0, 9);//reset current content in PS
	}
	m_group0.programmeServiceName[((segmentAddress + 1) * 2) - 2] = static_cast<char>((*m_rdsDataBlocks.blockD & 0xFF00) >> 8);//get first char
	m_group0.programmeServiceName[((segmentAddress + 1) * 2) - 1] = static_cast<char>(*m_rdsDataBlocks.blockD & 0x00FF);//get second char
	m_group0.decoderControlBits |= static_cast<uint8_t>(((*m_rdsDataBlocks.blockB & 0x0004) >> 2) << ~segmentAddress);//get bit of decoder identification and shift it to the right position
}

void RdsDecoder::prepareRadioText(void)
{
	if ((*m_rdsDataBlocks.blockB & 0x0010) != m_group2.textAbFlag)//check if flag has changed
	{
		m_group2.textAbFlag = static_cast<bool>(*m_rdsDataBlocks.blockB & 0x0010);//set new flag value
		memset(m_group2.radioText, 0, 65);//zero array with radiotext
	}
}

void RdsDecoder::setRadioText2A(void)
{
	uint8_t startPosition = 0;

	prepareRadioText();
	if (*m_rdsDataBlocks.blockB & 0x000F) startPosition = static_cast<uint8_t>((*m_rdsDataBlocks.blockB & 0x000F) * 4);//calculate text position
	m_group2.radioText[startPosition] = static_cast<char>((*m_rdsDataBlocks.blockC & 0xFF00) >> 8);
	m_group2.radioText[startPosition + 1] = static_cast<char>(*m_rdsDataBlocks.blockC & 0x00FF);
	m_group2.radioText[startPosition + 2] = static_cast<char>((*m_rdsDataBlocks.blockD & 0xFF00) >> 8);
	m_group2.radioText[startPosition + 3] = static_cast<char>(*m_rdsDataBlocks.blockD & 0x00FF);
}

void RdsDecoder::setRadioText2B(void)
{
	uint8_t startPosition = 0;

	prepareRadioText();
	if (*m_rdsDataBlocks.blockB & 0x000F) startPosition = static_cast<uint8_t>((*m_rdsDataBlocks.blockB & 0x000F) * 2);//calculate text position
	m_group2.radioText[startPosition] = static_cast<char>((*m_rdsDataBlocks.blockD & 0xFF00) >> 8);
	m_group2.radioText[startPosition + 1] = static_cast<char>(*m_rdsDataBlocks.blockD & 0x00FF);
}

void RdsDecoder::prepareTimeAndDate(void)
{
	m_group4A.modifiedJulianDay = static_cast<unsigned int>(((*m_rdsDataBlocks.blockB & 0x0003) << 14) | (*m_rdsDataBlocks.blockC & 0xFFFE));
	m_group4A.hour = static_cast<unsigned short>(((*m_rdsDataBlocks.blockC & 0x0001) << 5) | ((*m_rdsDataBlocks.blockD & 0xF000) >> 12));
	m_group4A.minute = static_cast<unsigned short>((*m_rdsDataBlocks.blockD & 0x0FC0) >> 6);
	m_group4A.timeOffsetType = static_cast<bool>(*m_rdsDataBlocks.blockD & 0x0020);
	m_group4A.localTimeOffset = static_cast<unsigned short>(*m_rdsDataBlocks.blockD & 0x001F);

	if (m_group4A.hour > 23) m_group4A.hour = 23;
	if (m_group4A.minute > 59) m_group4A.minute = 59;
}

void RdsDecoder::setDate(void)
{
	uint8_t _year = static_cast<uint8_t>((m_group4A.modifiedJulianDay - 15078.2) / 365.25);
	uint8_t _month = static_cast<uint8_t>(m_group4A.modifiedJulianDay - 14956.1 - static_cast<uint16_t>(_year*365.25) / 30.6001);
	m_date.day = static_cast<uint8_t>(m_group4A.modifiedJulianDay - 14956 - static_cast<uint16_t>(_year*365.25) - static_cast<uint8_t>(_month*30.6001));
	if (_month == 14 || _month == 15)
	{
		m_date.year = _year + 1;
		m_date.month = (_month - 2) * 12;
	}
	else
	{
		m_date.year = _year;
		m_date.month = (_month - 1) * 12;
	}

	m_date.weekDay = static_cast<uint8_t>(((m_group4A.modifiedJulianDay + 2) % 7) + 1);

	uint16_t _week = static_cast<uint16_t>((m_group4A.modifiedJulianDay / 7) - 2144.64);
	uint8_t _weekYear = static_cast<uint8_t>(((_week * 28) / 1461) - 0.0079);
	m_date.weekNumber = static_cast<uint8_t>(_week - static_cast<uint16_t>(((_weekYear * 1461) / 28) + 0.41));

	if (m_date.day > 31) m_date.day = 31; else if (m_date.day == 0) m_date.day = 1;
	if (m_date.month > 12) m_date.month = 12; else if (m_date.month == 0) m_date.month = 1;
	if (m_date.weekDay > 7) m_date.weekDay = 7; else if (m_date.weekDay == 0) m_date.weekDay = 1;
	if (m_date.weekNumber > 53) m_date.weekNumber = 53; else if (m_date.weekNumber == 0) m_date.weekNumber = 1;
}

void RdsDecoder::setProgrammeTypeName(void)
{
	if (*m_rdsDataBlocks.blockB & 0x000E) return;//check if bits have expected value, if not then don't do nothing
	if ((*m_rdsDataBlocks.blockB & 0x0010) != m_group10A.nameAbFlag)//check if flag has changed
	{
		m_group10A.nameAbFlag = static_cast<bool>(*m_rdsDataBlocks.blockB & 0x0010);//update flag value
		memset(m_group10A.programmeTypeName, 0, 9);//reset memory with programme type name data
	}
	if (*m_rdsDataBlocks.blockB & 0x0001)
	{
		m_group10A.programmeTypeName[4] = static_cast<char>((*m_rdsDataBlocks.blockC & 0xFF00) >> 8);
		m_group10A.programmeTypeName[5] = static_cast<char>(*m_rdsDataBlocks.blockC & 0x00FF);
		m_group10A.programmeTypeName[6] = static_cast<char>((*m_rdsDataBlocks.blockD & 0xFF00) >> 8);
		m_group10A.programmeTypeName[7] = static_cast<char>(*m_rdsDataBlocks.blockD & 0x00FF);
	}
	else
	{
		m_group10A.programmeTypeName[0] = static_cast<char>((*m_rdsDataBlocks.blockC & 0xFF00) >> 8);
		m_group10A.programmeTypeName[1] = static_cast<char>(*m_rdsDataBlocks.blockC & 0x00FF);
		m_group10A.programmeTypeName[2] = static_cast<char>((*m_rdsDataBlocks.blockD & 0xFF00) >> 8);
		m_group10A.programmeTypeName[3] = static_cast<char>(*m_rdsDataBlocks.blockD & 0x00FF);
	}
}
