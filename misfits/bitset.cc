#include <iterator>

#include "fits.hpp"

namespace misFITS {

    // reverse bits; see http://stackoverflow.com/a/2602885
    byte_t reverse_bits(byte_t b) {

	b = static_cast<byte_t>( (b & 0xF0) >> 4 | (b & 0x0F) << 4 );
	b = static_cast<byte_t>( (b & 0xCC) >> 2 | (b & 0x33) << 2 );
	b = static_cast<byte_t>( (b & 0xAA) >> 1 | (b & 0x55) << 1 );

	return b;
    }

}
