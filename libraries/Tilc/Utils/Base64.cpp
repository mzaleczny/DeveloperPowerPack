#include "Tilc/Utils/Base64.h"
#include "Tilc/Utils/FileUtils.h"

#define BYTE unsigned char

char Tilc::TBase64::CHAR_63 = '+';
char Tilc::TBase64::CHAR_64 = '/';
char Tilc::TBase64::CHAR_PAD = '=';

long Tilc::TBase64::Encode(const unsigned char*inData, long dataLength, TExtString& outCode) {
	outCode = "";

	// output buffer which holds code during conversation
	long len = GetCodeLength(dataLength);
	outCode.reserve(len + 1);

	// charachers used by Base64
	static const char alpha[] = 
	{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', Tilc::TBase64::CHAR_63, Tilc::TBase64::CHAR_64
	};

	// mask - first six bits
	const long mask = 0x3F;
    
	// used as temp 24-bits buffer
	union
	{
		unsigned char bytes[4];
		unsigned long block;
	} buffer;

	// coversation is done by taking three bytes at time of input data long temp
	// then four six-bits values are extracted, converted to base64 characters
	// and at the end they are written to output buffer
    long i, j, left, real_len = 0;
	for (i = 0, j = 0, left = dataLength; i < dataLength; i += 3, j += 4, left -= 3) {
		//------------------------
		// filling temp buffer

		// get first byte and puts it at MSB position in temp buffer
		buffer.bytes[2] = inData[i];

		// more data left?
		if (left > 1) {
			// get second byte and puts it at middle position in temp buffer
			buffer.bytes[1] = inData[i + 1];
			// more data left?
			if (left > 2) {
				// get third byte and puts it at LSB position in temp buffer
				buffer.bytes[0] = inData[i + 2];
            } else {
				// zero-padding of input data (last bytes)
				buffer.bytes[0] = 0;
            }
		} else {
			// zero-padding of input data (last two bytes)
			buffer.bytes[1] = 0;
			buffer.bytes[0] = 0;
		}

		//------------------------
		// constructing code from temp buffer
		// and putting it in output buffer

		// extract first and second six-bit value from temp buffer
		// and convert is to base64 character
		outCode[j] = alpha[(buffer.block >> 18) & mask];
		outCode[j + 1] = alpha[(buffer.block >> 12) & mask];
        real_len = j + 1 + 1;
		// more data left?
		if (left > 1) {
			// extract third six-bit value from temp buffer
			// and convert it to base64 character
			outCode[j + 2] = alpha[(buffer.block >> 6) & mask];
            real_len = j + 2 + 1;
			// more data left?
			if (left > 2) {
				// extract forth six-bit value from temp buffer
				// and convert it to base64 character
				outCode[j + 3] = alpha[buffer.block & mask];
                real_len = j + 3 + 1;
            } else {
				// pad output code
				outCode[j + 3] = CHAR_PAD;
                real_len = j + 3 + 1;
            }
		} else {
			// pad output code
			outCode[j + 2] = CHAR_PAD;
			outCode[j + 3] = CHAR_PAD;
            real_len = j + 3 + 1;
		}
	}

	outCode[real_len] = '0';
	return real_len;
}

long Tilc::TBase64::Decode(const TExtString& inCode, TExtString& outData) {
    size_t codeLength = inCode.length();

	outData.clear();
	outData.reserve(inCode.size());

	// used as temp 24-bits buffer
	union
	{
		unsigned char bytes[4];
		unsigned long block;
	} buffer;
	buffer.block = 0;

	// number of decoded bytes
	long j = 0;

    size_t m;
    char x;
    long val;
	for (size_t i = 0; i < codeLength; i++) {
		// position in temp buffer
		m = i % 4;

        x = inCode[i];
		val = 0;

		// converts base64 character to six-bit value
		if (x >= 'A' && x <= 'Z') {
			val = x - 'A';
        } else if (x >= 'a' && x <= 'z') {
			val = x - 'a' + 'Z' - 'A' + 1;
        } else if (x >= '0' && x <= '9') {
			val = x - '0' + ('Z' - 'A' + 1) * 2;
        } else if (x == Tilc::TBase64::CHAR_63) {
			val = 62;
        } else if (x == Tilc::TBase64::CHAR_64) {
			val = 63;
        }

		// padding chars are not decoded and written to output buffer
		if (x != Tilc::TBase64::CHAR_PAD) {
			buffer.block |= val << (3 - m) * 6;
        } else {
			m--;
        }

		// temp buffer is full or end of code is reached
		// flushing temp buffer
		if (m == 3 || x == Tilc::TBase64::CHAR_PAD) {
			// writes byte from temp buffer (combined from two six-bit values) to output buffer
			outData.push_back(buffer.bytes[2]);
			++j;
			// more data left?
			if (x != Tilc::TBase64::CHAR_PAD || m > 1) {
				// writes byte from temp buffer (combined from two six-bit values) to output buffer
				outData.push_back(buffer.bytes[1]);
				++j;
				// more data left?
				if (x != Tilc::TBase64::CHAR_PAD || m > 2) {
					// writes byte from temp buffer (combined from two six-bit values) to output buffer
					outData.push_back(buffer.bytes[0]);
					++j;
                }
			}

			// restarts temp buffer
			buffer.block = 0;
		}

		// when padding CHAR is reached it is the end of code
		if (x == CHAR_PAD) {
			break;
        }
	}

	return j;
}

// Returns maximum size of decoded data based on size of Base64 code.
long Tilc::TBase64::GetDataLength(long codeLength) {
	return codeLength - codeLength / 4;
}

// Returns maximum length of Base64 code based on size of uncoded data.
long Tilc::TBase64::GetCodeLength(long dataLength) {
	long len = dataLength + dataLength / 3 + (long)(dataLength % 3 != 0);

	// output code size must be multiple of 4 bytes
	if (len % 4) {
		len += 4 - len % 4;
    }

	return len;
}

long Tilc::TBase64::EncodeFileToVar(const TExtString& fname, TExtString& outCode) {
    size_t fsize;
    unsigned char* buffer;
    long len = 0;
	FileGetContents(fname.c_str(), reinterpret_cast<char**>(&buffer), &fsize);
    if (buffer)
    {
        len = Tilc::TBase64::Encode(buffer, fsize, outCode);
        delete[] buffer;
    }
    return len;
}

long Tilc::TBase64::EncodeFile(const TExtString& fname, TExtString& outputFName) {
    if (outputFName.length() < 1) {
        outputFName = fname;
    }
    TExtString outCode;
    long len = Tilc::TBase64::EncodeFileToVar(fname, outCode);
    FilePutContentsRaw(outputFName, outCode);
    return len;
}

long Tilc::TBase64::DecodeFileToVar(const TExtString& fname, TExtString& outData) {
    TExtString inCode = FileGetContents(fname);
    long len = Tilc::TBase64::Decode(inCode, outData);
    return len;
}

long Tilc::TBase64::DecodeFile(const TExtString& fname, TExtString& outputFName) {
    if (outputFName.length() < 1)
	{
        outputFName = fname;
    }
    TExtString inCode = FileGetContents(fname);
	TExtString outData;
    long len = Tilc::TBase64::Decode(inCode, outData);
	FilePutContentsRaw(outputFName, (char*)outData.data(), len);
    return len;
}
