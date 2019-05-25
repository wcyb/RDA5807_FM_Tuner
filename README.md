# RDA5807 FM Tuner
* Full support for all functions of RDA5807 FM tuner IC family
* Contains module for decoding RDS data (currently supports most non-ODA groups)

#### Known issues with RDA5807M
* It seems that only RDS blocks A and B are checked for errors and corrected, so we never know if blocks C and D were received correctly
* When using direct frequency setting, first you need to write all writable registers once. After that you can update only ones that you will change. Failing to do so result in no signal reception
* When using normal frequency setting, you don't need to do the above operation

### Only for non-commercial use.
