/*
 Name:		RdsDecoder.h
 Created:	11/03/2019 2:06:52 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#ifndef _RdsDecoder_h
#define _RdsDecoder_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class RDA5807;
class RdsDecoder final
{
private:
	friend class RDA5807;//declare this class a friend so it can access private method in this class
public:
#pragma region RDS enums
	/// <summary>
	/// Possible RDS group types.
	/// </summary>
	enum class groupType : uint8_t
	{
		g0A, g0B, g1A, g1B, g2A, g2B, g3A, g3B,
		g4A, g4B, g5A, g5B, g6A, g6B, g7A, g7B,
		g8A, g8B, g9A, g9B, g10A, g10B, g11A, g11B,
		g12A, g12B, g13A, g13B, g14A, g14B, g15A, g15B,
		none
	};

	/// <summary>
	/// Possible extended country codes.
	/// </summary>
	enum class extendedCountryCode : uint8_t
	{
		reserved,
		deAlternative, dz, ad, il, it, be, ru, ps, al, at, hu, mt, de, eg = 0x0F, gr,
		cy, sm, ch, jo, fi, lu, bg, dk, gi, iq, gb, ly, ro, fr, ma,
		cz, pl, va, sk, sy, tn, li = 0x29, is, mc, lt, yu, es, no, ie = 0x33,
		tr, mk, nl = 0x38, lv, lb, hr = 0x3C, se = 0x3E, by, md, ee, ua = 0x46, pt = 0x48, si, ba = 0x4F
	};

	/// <summary>
	/// Possible coverage area values.
	/// </summary>
	enum class coverageArea : uint8_t { local, international, national, supraRegional, regional };

	/// <summary>
	/// Possible programme type values.
	/// </summary>
	enum class programmeType : uint8_t
	{
		none, news, affairs, info, sport, educate, drama, culture, science, varied,
		pop, rock, easy, light, classics, other, weather, finance, children, social,
		religion, phoneIn, travel, leisure, jazz, country, national, oldies, folk, document,
		test, alarm
	};
#pragma endregion
private:
#pragma region RDS groups
	/// <summary>
	/// RDS data block pointers.
	/// </summary>
	struct
	{
		uint16_t* blockA;
		uint16_t* blockB;
		uint16_t* blockC;
		uint16_t* blockD;
	} m_rdsDataBlocks;

	/// <summary>
	/// PI (Programme Identification) block, always in block A.
	/// </summary>
	struct
	{
		unsigned short
			countryCode : 4,
			programmeAreaCoverage : 4;
		uint8_t programmeReferenceNumber;
	} m_programmeIdentification = { 0 };

	/// <summary>
	/// Data from block B.
	/// </summary>
	struct
	{
		bool trafficProgramme : 1;//true if Traffic Programme is carried by this station, false otherwise
		programmeType progType;
	} m_blockBData = { 0 };

	/// <summary>
	/// RDS data group 0A and 0B.
	/// </summary>
	struct
	{
		//uint8_t altFrequency0;//TODO, if needed
		//uint8_t altFrequency1;//TODO, if needed
		bool trafficAnnouncement : 1;
		bool musicSpeech : 1;
		unsigned short decoderControlBits : 4;
		char programmeServiceName[9];//8 chars for station name and one 0 as end mark
	} m_group0 = { 0 };

	/// <summary>
	/// RDS data group 1A and 1B.
	/// </summary>
	struct
	{
		uint8_t extendedCountryCode;
		unsigned short languageCode : 12;

		union
		{
			uint16_t rawBroadcastTimeData;
			struct
			{
				unsigned short
					minute : 6,
					hour : 5,
					day : 5;
			} broadcastTimeValues;
		} broadcastTime;
	} m_group1 = { 0 };

	/// <summary>
	/// RDS data group 2A and 2B.
	/// </summary>
	struct
	{
		bool textAbFlag : 1;
		char radioText[65];//64 chars for text and one 0 as end mark
	} m_group2 = { 0 };

	/// <summary>
	/// RDS data group 4A.
	/// </summary>
	struct
	{
		unsigned int modifiedJulianDay : 17;
		unsigned short
			hour : 5,
			minute : 6,
			timeOffsetType : 1,
			localTimeOffset : 5;
	} m_group4A = { 0 };

	/// <summary>
	/// RDS data group 10A.
	/// </summary>
	struct
	{
		bool nameAbFlag : 1;
		char programmeTypeName[9];//8 chars for programme type name and one 0 as end mark
	} m_group10A = { 0 };
#pragma endregion
	/// <summary>
	/// Date data.
	/// </summary>
	struct
	{
		unsigned short
			year : 7,
			month : 4,
			day : 5,
			weekDay : 4,
			weekNumber : 7;
	} m_date = { 0 };

private:
	/// <summary>
	/// Creates RDS data decoder object. To decode data use decodeReceivedData(),
	/// and then use getX methods to get decoded data, according to type of received group returned by decodeReceivedData().
	/// </summary>
	/// <param name="blockA">pointer to block A of RDS data</param>
	/// <param name="blockB">pointer to block B of RDS data</param>
	/// <param name="blockC">pointer to block C of RDS data</param>
	/// <param name="blockD">pointer to block D of RDS data</param>
	RdsDecoder(uint16_t* blockA, uint16_t* blockB, uint16_t* blockC, uint16_t* blockD) :
		m_rdsDataBlocks{ blockA, blockB, blockC, blockD } {};

	RdsDecoder(const RdsDecoder&) = delete;
	RdsDecoder& operator=(const RdsDecoder&) = delete;

public:
#pragma region block A
	/// <summary>
	/// Returns country code. Country codes are not unique. To make use of them, one needs to know where receiver is located.
	/// </summary>
	/// <returns>country code value</returns>
	uint8_t getCountryCode(void) const { return m_programmeIdentification.countryCode; }

	/// <summary>
	/// Returns programme area coverage. Values from 0x4 to 0xF specify a regional programme.
	/// </summary>
	/// <returns>programme area coverage value</returns>
	uint8_t getProgrammeAreaCoverage(void) const { return m_programmeIdentification.programmeAreaCoverage; }

	/// <summary>
	/// Returns programme reference number. This number is used to differentiate between programme families.
	/// </summary>
	/// <returns>programme reference number value</returns>
	uint8_t getProgrammeReferenceNumber(void) const { return m_programmeIdentification.programmeReferenceNumber; }
#pragma endregion

#pragma region block B
	/// <summary>
	/// Returns information if (TP) Traffic Programme information is carried by received station.
	/// </summary>
	/// <returns>true if Traffic Programme information is carried, false otherwise</returns>
	bool getTrafficProgramme(void) const { return m_blockBData.trafficProgramme; }

	/// <summary>
	/// Returns received (PTY) Programme Type.
	/// </summary>
	/// <returns>received Programme Type</returns>
	const programmeType& getProgrammeType(void) const { return m_blockBData.progType; }
#pragma endregion

#pragma region group 0
	/// <summary>
	/// Returns information if (TA) Traffic Announcement flag is set, which may suggest that a traffic announcement is being broadcasted on received programme at present.
	/// </summary>
	/// <returns>true is set, false otherwise</returns>
	bool getTrafficAnnouncement(void) const { return m_group0.trafficAnnouncement; }

	/// <summary>
	/// Returns information about what is broadcasted (music or speech).
	/// </summary>
	/// <returns>true if music, false if speech</returns>
	bool getMusicOrSpeech(void) const { return m_group0.musicSpeech; }

#pragma region decoder control bits
	/// <summary>
	/// Returns information if received station is in mono.
	/// </summary>
	/// <returns>true if mono, false if stereo</returns>
	bool getDecoderMono(void) const { if (m_group0.decoderControlBits & 0x1) return false; else return true; }

	/// <summary>
	/// Returns information about artificial head.
	/// </summary>
	/// <returns>true if not artificial head, false otherwise</returns>
	bool getDecoderNotArtificialHead(void) const { if (m_group0.decoderControlBits & 0x2) return false; else return true; }

	/// <summary>
	/// Returns information about compression.
	/// </summary>
	/// <returns>true if not compressed, false otherwise</returns>
	bool getDecoderNotCompressed(void) const { if (m_group0.decoderControlBits & 0x4) return false; else return true; }

	/// <summary>
	/// Returns information if Programme Type code is static.
	/// </summary>
	/// <returns>true if it is static, false otherwise</returns>
	bool getDecoderStaticProgrammeTypeCode(void) const { if (m_group0.decoderControlBits & 0x8) return false; else return true; }
#pragma endregion

	/// <summary>
	/// Returns pointer to 8 char array containing programme name.
	/// </summary>
	/// <returns>pointer to 8 char array</returns>
	const char* getProgrammeServiceName(void) const { return m_group0.programmeServiceName; }
#pragma endregion
#pragma region group 1A and 1B
	/// <summary>
	/// Returns (ECC) Extended Country Code value.
	/// </summary>
	/// <returns>Extended Country Code value</returns>
	uint8_t getExtendedCountryCode(void) const { return m_group1.extendedCountryCode; }

	/// <summary>
	/// Returns language code of received station.
	/// </summary>
	/// <returns>language code value</returns>
	uint16_t getLanguageCode(void) const { return m_group1.languageCode; }

	/// <summary>
	/// Returns broadcast start day value (from 1 to 31).
	/// </summary>
	/// <returns>broadcast start day value</returns>
	uint8_t getBroadcastStartDay(void) const { return m_group1.broadcastTime.broadcastTimeValues.day; }

	/// <summary>
	/// Returns broadcast start hour value (from 0 to 23).
	/// </summary>
	/// <returns>broadcast start hour value</returns>
	uint8_t getBroadcastStartHour(void) const { return m_group1.broadcastTime.broadcastTimeValues.hour; }

	/// <summary>
	/// Returns broadcast start minute value (from 0 to 59).
	/// </summary>
	/// <returns>broadcast start minute value</returns>
	uint8_t getBroadcastStartMinute(void) const { return m_group1.broadcastTime.broadcastTimeValues.minute; }
#pragma endregion
#pragma region group 2A and 2B
	/// <summary>
	/// Returns pointer to 64 char array containing radio text.
	/// </summary>
	/// <returns>pointer to 64 char array</returns>
	const char* getRadioText(void) const { return m_group2.radioText; }
#pragma endregion
#pragma region group 4A
	/// <summary>
	/// Returns decoded year as value from 00 to 99.
	/// </summary>
	/// <returns>year value</returns>
	uint8_t getYear(void) const { return m_date.year; }

	/// <summary>
	/// Returns decoded month as value from 1 to 12.
	/// </summary>
	/// <returns>month value</returns>
	uint8_t getMonth(void) const { return m_date.month; }

	/// <summary>
	/// Returns decoded day as value from 1 to 31.
	/// </summary>
	/// <returns>day value</returns>
	uint8_t getDay(void) const { return m_date.day; }

	/// <summary>
	/// Returns decoded week day as value from 1(Monday) to 7(Sunday).
	/// </summary>
	/// <returns>week day value</returns>
	uint8_t getWeekDay(void) const { return m_date.weekDay; }

	/// <summary>
	/// Returns decoded week number as value according to ISO 2015.
	/// </summary>
	/// <returns>week number value</returns>
	uint8_t getWeekNumber(void) const { return m_date.weekNumber; }

	/// <summary>
	/// Returns decoded hours (UTC) as value from 0 to 23.
	/// </summary>
	/// <returns>hours value</returns>
	uint8_t getHour(void) const { return m_group4A.hour; }

	/// <summary>
	/// Returns decoded minutes (UTC) as value from 0 to 59.
	/// </summary>
	/// <returns>minutes value</returns>
	uint8_t getMinute(void) const { return m_group4A.minute; }

	/// <summary>
	/// Returns sign of local time offset.
	/// </summary>
	/// <returns>true if negative, false otherwise</returns>
	bool getLocalTimeOffsetSign(void) const { return m_group4A.timeOffsetType; }

	/// <summary>
	/// Returns local time offset expressed in multiples of half hours within the range -12h to +12h.
	/// Sign of this offset can be obtained using getLocalTimeOffsetSign().
	/// </summary>
	/// <returns>local time offset</returns>
	uint8_t getLocalTimeOffset(void) const { return m_group4A.localTimeOffset; }
#pragma endregion
#pragma region group 10A
	/// <summary>
	/// Returns pointer to 8 char array containing programme type name.
	/// </summary>
	/// <returns>pointer to 8 char array</returns>
	const char* getProgrammeTypeName(void) const { return m_group10A.programmeTypeName; }
#pragma endregion

private:
	/// <summary>
	/// Decodes received RDS data and sets result in appropriate structure.
	/// </summary>
	/// <returns>type of decoded group. Returns group type 'none' if received group is not supported</returns>
	groupType decodeReceivedData(void);

#pragma region block A
	/// <summary>
	/// Returns group type code. This code specifies what type of information were received.
	/// </summary>
	/// <returns>group type code</returns>
	groupType getGroupTypeCode(void) const { return static_cast<groupType>(((*m_rdsDataBlocks.blockB & 0xF000) >> 12) | getVersion()); }

	/// <summary>
	/// Sets country code. Country codes are not unique. To make use of them, one needs to know where receiver is located.
	/// </summary>
	void setCountryCode(void) { m_programmeIdentification.countryCode = static_cast<uint8_t>((*m_rdsDataBlocks.blockA & 0xF000) >> 12); }

	/// <summary>
	/// Sets programme area coverage. Values from 0x4 to 0xF specify a regional programme.
	/// </summary>
	void setProgrammeAreaCoverage(void) { m_programmeIdentification.programmeAreaCoverage = static_cast<uint8_t>((*m_rdsDataBlocks.blockA & 0x0F00) >> 8); }

	/// <summary>
	/// Sets programme reference number. This number is used to differentiate between programme families.
	/// </summary>
	void setProgrammeReferenceNumber(void) { m_programmeIdentification.programmeReferenceNumber = static_cast<uint8_t>(*m_rdsDataBlocks.blockA & 0x00FF); }
#pragma endregion
#pragma region block B
	/// <summary>
	/// Returns groups version. If true, (PI) Programme Identification code is inserted in blocks A and C.
	/// </summary>
	/// <returns>true if version B, false if version A</returns>
	bool getVersion(void) const { return static_cast<bool>(*m_rdsDataBlocks.blockB & 0x0800); }

	/// <summary>
	/// Sets information whether received station broadcasts (TP) Traffic Programme.
	/// </summary>
	void setTrafficProgramme(void) { m_blockBData.trafficProgramme = static_cast<bool>(*m_rdsDataBlocks.blockB & 0x0400); }

	/// <summary>
	/// Sets (PTY) Programme Type code. This code specifies what type of music is broadcasted.
	/// </summary>
	void setProgrammeTypeCode(void) { m_blockBData.progType = static_cast<programmeType>((*m_rdsDataBlocks.blockB & 0x03E0) >> 5); }
#pragma endregion
#pragma region group 0
	/// <summary>
	/// Decodes information about (PS) Programme Service name and (DI) decoder identification control code.
	/// </summary>
	void setProgrammeServiceName(void);
#pragma endregion
#pragma region group 1A and 1B
	/// <summary>
	/// Sets (ECC) Extended Country Code. This can be used only when received group was 1A.
	/// </summary>
	void setExtendedCountryCode(void) { m_group1.extendedCountryCode = static_cast<uint8_t>(*m_rdsDataBlocks.blockC & 0x00FF); }

	/// <summary>
	/// Sets (PIN) Programme Item Number code. This information contains day of the month, hour and minute of scheduled broadcast start time.
	/// </summary>
	void setProgrammeItemNumberCode(void) { m_group1.broadcastTime.rawBroadcastTimeData = *m_rdsDataBlocks.blockD; }

	/// <summary>
	/// Sets language code of received station.
	/// </summary>
	void setLanguageCode(void) { if (*m_rdsDataBlocks.blockC & 0xB000) m_group1.languageCode = static_cast<unsigned short>(*m_rdsDataBlocks.blockC & 0x0FFF); }//check if correct code is transmitted and save it
#pragma endregion
#pragma region group 2A and 2B
	/// <summary>
	/// Prepares data structure for new RadioText data.
	/// </summary>
	void prepareRadioText(void);

	/// <summary>
	/// Decodes RadioText from group 2A.
	/// </summary>
	void setRadioText2A(void);

	/// <summary>
	/// Decodes RadioText from group 2B.
	/// </summary>
	void setRadioText2B(void);
#pragma endregion
#pragma region group 4a
	/// <summary>
	/// Prepares data structure for new time and date data.
	/// </summary>
	void prepareTimeAndDate(void);

	/// <summary>
	/// Decodes RDS date and sets result values in date structure.
	/// </summary>
	void setDate(void);
#pragma endregion
#pragma region group 10a
	/// <summary>
	/// Decodes (PTYN) Programme Type Name.
	/// </summary>
	void setProgrammeTypeName(void);
#pragma endregion
};

#endif
