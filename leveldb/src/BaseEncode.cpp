#include "BaseEncode.h"

int base32_encode(const char *data, int length, char *result, int bufSize) {
	if (length < 0 || length >(1 << 28)) {
		return -1;
	}
	int count = 0;
	if (length > 0) {
		int buffer = data[0];
		int next = 1;
		int bitsLeft = 8;
		while (count < bufSize && (bitsLeft > 0 || next < length)) {
			if (bitsLeft < 5) {
				if (next < length) {
					buffer <<= 8;
					buffer |= data[next++] & 0xFF;
					bitsLeft += 8;
				}
				else {
					int pad = 5 - bitsLeft;
					buffer <<= pad;
					bitsLeft += pad;
				}
			}
			int index = 0x1F & (buffer >> (bitsLeft - 5));
			bitsLeft -= 5;
			result[count++] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"[index];
		}
	}
	if (count < bufSize) {
		result[count] = '\000';
	}
	return count;
}