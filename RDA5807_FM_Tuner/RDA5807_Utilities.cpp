/*
 Name:		RDA5807_Utilities.cpp
 Created:	11/03/2019 2:06:52 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#include "RDA5807_Utilities.h"

float RDA5807_Utilities::getFrequencyValue(
	const uint16_t& freq,
	const RDA5807::channelSpacing& chanSpac,
	const RDA5807::band& selBand,
	const bool& altEurBand)
{
	float band = 0;
	uint8_t spc = 0;

	switch (chanSpac)
	{
	case RDA5807::channelSpacing::spc25kHz:
		spc = 25;
		break;
	case RDA5807::channelSpacing::spc50kHz:
		spc = 50;
		break;
	case RDA5807::channelSpacing::spc100kHz:
		spc = 100;
		break;
	case RDA5807::channelSpacing::spc200kHz:
		spc = 200;
		break;
	}

	switch (selBand)
	{
	case RDA5807::band::usEurope:
		band = 87.0f;
		break;
	case RDA5807::band::japan:
	case RDA5807::band::worldWide:
		band = 76.0f;
		break;
	case RDA5807::band::eastEurope:
		if (altEurBand) band = 50.0f;
		else band = 65.0f;
		break;
	}

	return static_cast<float>(((freq * spc) / 1000.0f) + band);
}

float RDA5807_Utilities::getAlternativeFrequencyValue(
	const uint16_t& freq,
	const RDA5807::band& selBand,
	const bool& altEurBand)
{
	float band = 0;

	switch (selBand)
	{
	case RDA5807::band::usEurope:
		band = 87.0f;
		break;
	case RDA5807::band::japan:
	case RDA5807::band::worldWide:
		band = 76.0f;
		break;
	case RDA5807::band::eastEurope:
		if (altEurBand) band = 50.0f;
		else band = 65.0f;
		break;
	}

	return static_cast<float>((freq / 1000.0f) + band);
}
