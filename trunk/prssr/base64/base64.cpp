//
// base64.cpp
//
//

#include "..\stdafx.h"
#include "base64.h"

//
// Encoding
//

static const char encodeTab[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void encodeBlock(unsigned char in[3], unsigned char out[4], int len) {
    out[0] = encodeTab[in[0] >> 2];
    out[1] = encodeTab[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
    out[2] = (unsigned char) (len > 1 ? encodeTab[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] : '=');
    out[3] = (unsigned char) (len > 2 ? encodeTab[in[2] & 0x3f] : '=');
}

CString base64encode(const CString &input) {
    unsigned char in[3], out[4];
    int i, len;
	int pos = 0;
	CString output;

    while (pos < input.GetLength()) {
        len = 0;
        for (i = 0; i < 3; i++) {
            if (pos < input.GetLength()) {
	            in[i] = (unsigned char) input.GetAt(pos);
				pos++;
                len++;
            }
            else {
                in[i] = 0;
            }
        }
        if (len) {
            encodeBlock(in, out, len);
            for (i = 0; i < 4; i++) {
				output += (TCHAR) out[i];
            }
        }
    }

	return output;
}

//
// Decoding
//

static const char decodeTab[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

void decodeBlock(unsigned char in[4], unsigned char out[3]) {   
    out[0] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}


CString base64decode(const CString &input) {
    unsigned char in[4], out[3], v;
    int i, len;
	int pos = 0;
	CString output;

    while (pos < input.GetLength()) {
        for (len = 0, i = 0; i < 4 && pos < input.GetLength(); i++) {
            v = 0;
            while (pos < input.GetLength() && v == 0) {
                v = (unsigned char) input[pos];
				pos++;
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : decodeTab[v - 43]);
                if (v) {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
            if (pos < input.GetLength()) {
                len++;
                if (v) {
                    in[i] = (unsigned char) (v - 1);
                }
            }
            else {
                in[i] = 0;
            }
        }
        if (len) {
            decodeBlock(in, out);
            for (i = 0; i < len - 1; i++)
                output += (TCHAR) out[i];
        }
    }

	return output;
}
