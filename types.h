#pragma once

typedef struct patch {
	const unsigned long int offset;
	const unsigned char[] value;
	const size_t size;
} patch;