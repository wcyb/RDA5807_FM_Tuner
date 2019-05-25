/*
 Name:		RDA5807_Utilities.h
 Created:	11/03/2019 2:06:52 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#ifndef _RDA5807_UTILITIES_h
#define _RDA5807_UTILITIES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "RDA5807_FM_Tuner.h"

class RDA5807_Utilities
{
private:
	RDA5807_Utilities() = delete;
	RDA5807_Utilities(const RDA5807_Utilities&) = delete;
	RDA5807_Utilities& operator=(const RDA5807_Utilities&) = delete;

public:
	/// <summary>
	/// Returns current volume level converted to percentage level.
	/// </summary>
	/// <param name="volLevel">current volume level</param>
	/// <returns>percentage volume level</returns>
	static uint8_t getVolumePercentage(const uint8_t& volLevel)
	{
		if (volLevel) return static_cast<uint8_t>((volLevel / 15.0f) * 100);
		return volLevel;
	}

	/// <summary>
	/// Returns RSSI in dB.
	/// </summary>
	/// <param name="rssi">raw RSSI value</param>
	/// <returns>RSSI value in dB</returns>
	static uint16_t getRssiDb(const uint8_t& rssi)
	{
		return static_cast<uint16_t>(10 * log10(rssi));
	}

	/// <summary>
	/// Returns current frequency value.
	/// </summary>
	/// <param name="freq">selected chan value</param>
	/// <param name="chanSpac">channel spacing</param>
	/// <param name="selBand">selected band</param>
	/// <param name="altEurBand">information if alternative East Europe band was selected</param>
	/// <returns>frequency value</returns>
	static float getFrequencyValue(
		const uint16_t& freq,
		const RDA5807::channelSpacing& chanSpac = RDA5807::channelSpacing::spc100kHz,
		const RDA5807::band& selBand = RDA5807::band::usEurope,
		const bool& altEurBand = false);

	/// <summary>
	/// Returns current frequency value when using alternative frequency setting mode.
	/// </summary>
	/// <param name="freq">selected chan value</param>
	/// <param name="selBand">selected band</param>
	/// <param name="altEurBand">information if alternative East Europe band was selected</param>
	/// <returns></returns>
	static float getAlternativeFrequencyValue(
		const uint16_t& freq,
		const RDA5807::band& selBand = RDA5807::band::usEurope,
		const bool& altEurBand = false);
};

#endif
