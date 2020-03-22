/*  CRC8 library
	*/
	#ifndef CRC8_H
	#define CRC8_H

	#include "Arduino.h"


	typedef uint8_t crc;
	#define POLYNOMIAL 0x07  /* CRC8_CCITT */
	#define WIDTH  (8 * sizeof(crc))
	#define TOPBIT (1 << (WIDTH - 1))

	class CRC8 {
	  public:
	   CRC8();
	   void begin();
	   crc get_crc8(uint8_t const message[], int nBytes);
	 
	 private:
	  uint8_t crcTable[256];
	  

	};

	#endif