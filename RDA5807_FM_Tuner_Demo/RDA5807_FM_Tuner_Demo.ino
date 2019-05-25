/*
 Name:		RDA5807_FM_Tuner_Demo.ino
 Created:	02/03/2019 2:50:44 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/

#include "RDA5807_FM_Tuner.h"
#include "RDA5807_Utilities.h"

RDA5807* rda = nullptr;
const RdsDecoder* rdsDecode = nullptr;

// the setup function runs once when you press reset or power the board
void setup() {
	Wire.setClock(400000);//for RDA5807 400kHz clock is max
	Wire.begin();
	Serial.begin(115200);

	rda = new RDA5807(true);

	rda->setEnable();//enable receiver
	rda->setNewDemodulationMethod();//enable alternative demodulation method
	rda->setRds();//enable RDS
	rda->setAudioOutput();//enable audio output
	rda->setMute(false);//unmute
	rda->setSoftBlend(false);//disable softblend
	rda->writeModifiedRegistersToReceiver();//send changes to receiver
	//rda->writeSettingsToReceiver();//when using alternative frequency setting method, use this method once to set all correctly, after that you can use writeModifiedSettings...
	rda->updateVolumeLevel(0xFF);//set max volume
	rda->updateReceivedFrequency(1009);//set received frequency to 100.9Mhz
	Serial.println("...RDA5807 FM Tuner Demo started...");
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (!(millis() % 2000))//do this every two seconds
	{
		Serial.println("----------");
		//print stats about received station
		Serial.print("Volume: "); Serial.print(RDA5807_Utilities::getVolumePercentage(rda->getVolume())); Serial.println("%");
		Serial.print("Freq: "); Serial.print(RDA5807_Utilities::getFrequencyValue(rda->getChannel())); Serial.println("MHz");
		rda->updateRssi();
		Serial.print("RSSI: "); Serial.print(RDA5807_Utilities::getRssiDb(rda->getRssi())); Serial.println("dB");
		if (rda->getRds())//if RDS is enabled then display received informations
		{//RDA5807 seems to check for errors and correcting only RDS blocks A and B, so we never know if blocks C and D were received correctly
			if (rda->checkIfNewRdsDataIsReady())
			{
				rda->updateRdsData();
				rda->updateDecodedRdsData();//here you can check what RDS group was received to display or update only received informations
				rdsDecode = rda->getDecodedRdsData();//if you don't know if RDS was enabled, check returned pointer (it can be nullptr if RDS decoding was disabled)
			}
			Serial.print("Time: "); Serial.print(rdsDecode->getHour()); Serial.print(":"); Serial.println(rdsDecode->getMinute());//to display local time instead of UTC, one need to add time offset
			Serial.print("Station name: "); Serial.println(rdsDecode->getProgrammeServiceName());
			Serial.print("Station type: "); Serial.println(rdsDecode->getProgrammeTypeName());
			Serial.print("Radio text: "); Serial.println(rdsDecode->getRadioText());
		}
		Serial.println("----------");
	}
}
