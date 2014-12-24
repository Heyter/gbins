#pragma once

// pass string, strlen, result buffer, size of buffer
// returns length of string on the buffer that was used
int base32_encode(const char *data, int length, char *result, int bufSize);