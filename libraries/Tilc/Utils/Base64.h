#pragma once

#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
	/*
	Tabela znaków w kodowaniu base64:

	Value Encoding  Value Encoding  Value Encoding  Value Encoding
	   0 A            17 R            34 i            51 z
	   1 B            18 S            35 j            52 0
	   2 C            19 T            36 k            53 1
	   3 D            20 U            37 l            54 2
	   4 E            21 V            38 m            55 3
	   5 F            22 W            39 n            56 4
	   6 G            23 X            40 o            57 5
	   7 H            24 Y            41 p            58 6
	   8 I            25 Z            42 q            59 7
	   9 J            26 a            43 r            60 8
	  10 K            27 b            44 s            61 9
	  11 L            28 c            45 t            62 +
	  12 M            29 d            46 u            63 /
	  13 N            30 e            47 v
	  14 O            31 f            48 w         (pad) =
	  15 P            32 g            49 x
	  16 Q            33 h            50 y
	*/

	// Encoding and decoding Base64 code
	class DECLSPEC TBase64 {
	public:
		// 63rd CHAR used for Base64 code
		static char CHAR_63;
		// 64th CHAR used for Base64 code
		static char CHAR_64;
		// Char used for padding
		static char CHAR_PAD;

		TBase64() = default;
		~TBase64() = default;

		// Encodes binary data to Base64 code. Returns size of encoded data.
		static long Encode(const unsigned char* inData, long dataLength, TExtString& outCode);
		// Decodes Base64 code to binary data. Returns size of decoded data.
		static long Decode(const TExtString& inCode, TExtString& outData);
		// Returns maximum size of decoded data based on size of Base64 code.
		static long GetDataLength(long codeLength);
		// Returns maximum length of Base64 code based on size of uncoded data.
		static long GetCodeLength(long dataLength);
		// koduje plik do zmiennej
		static long EncodeFileToVar(const TExtString& fname, TExtString& outCode);
		// koduje plik w miejscu
		static long EncodeFile(const TExtString& fname, TExtString& outputFName);
		// dekoduje plik do zmiennej
		static long DecodeFileToVar(const TExtString& fname, TExtString& outData);
		// dekoduje plik w miejscu
		static long DecodeFile(const TExtString& fname, TExtString& outputFName);
	};

}
