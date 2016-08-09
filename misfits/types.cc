// --8<--8<--8<--8<--
//
// Copyright (C) 2015 Smithsonian Astrophysical Observatory
//
// This file is part of misfits
//
// misfits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -->8-->8-->8-->8--

#include <misfits/fits.hpp>
#include "misfits/fits_p.hpp"
#include <misfits/types.hpp>

#include <climits>

namespace misFITS {

    namespace ColumnType {

	// width depends upon field type; this seems to be the only
	// means to get it from CFITSIO. note that for strings, width
	// will be the number of charactes in a string, not the width
	// of the storage type
	size_t Spec::width( size_t repeat ) {

	    if ( ColumnType::ID::Bit == id() ) {

		// Bit fields are special.  The repeat count is the number
		// of bits; figure out number of the chars.

		size_t width = repeat / CHAR_BIT;
		if ( width * CHAR_BIT  < repeat ) width += 1;

		return width;

	    }
	    else {

		char tform_t[2];

		tform_t[0] = code();
		tform_t[1] = '\0';

		long width;

		misFITS_CHECK_CFITSIO_EXPR
		    ( fits_binary_tformll( tform_t, NULL, NULL, &width, &status )
		      );

		return width * repeat;
	    }

	}

	template <> char Impl<ID::Byte>::code() 	 { return 'B'; }
	template <> char Impl<ID::String>::code() 	 { return 'A'; }
	template <> char Impl<ID::Short>::code() 	 { return 'I'; }
	template <> char Impl<ID::Long>::code() 	 { return 'J'; }
	template <> char Impl<ID::LongLong>::code() 	 { return 'K'; }
	template <> char Impl<ID::Float>::code() 	 { return 'E'; }
	template <> char Impl<ID::Double>::code() 	 { return 'D'; }
	template <> char Impl<ID::UShort>::code() 	 { return 'U'; }
	template <> char Impl<ID::ULong>::code()  	 { return 'V'; }

	// no support for complex as of yet
	// template <> char Impl<ID::Complex>::code() 	 { return 'C'; }
	// template <> char Impl<ID::DoubleComplex>::code() { return 'M'; }

	SpecPtr
	spec_from_id( ID::type id ) {

	    switch (  id ) {

	    case ID::Bit:
		return make_shared< Impl<ID::Bit> >();
		break;


	    case ID::Byte:
		return make_shared< Impl<ID::Byte> >();
		break;

	    case ID::Logical:
		return make_shared< Impl<ID::Logical> >();
		break;

	    case ID::String:
		return make_shared< Impl<ID::String> >();
		break;

	    case ID::Short:
		return make_shared< Impl<ID::Short> >();
		break;

	    case ID::Long:
		return make_shared< Impl<ID::Long> >();
		break;

	    case ID::LongLong:
		return make_shared< Impl<ID::LongLong> >();
		break;

	    case ID::Float:
		return make_shared< Impl<ID::Float> >();
		break;

	    case ID::Double:
		return make_shared< Impl<ID::Double> >();
		break;

	    // no support for complex as of yet
	    // case ID::Complex:
	    // 	return make_shared< Impl<ID::Complex> >();
	    // 	break;

	    // case ID::DoubleComplex:
	    // 	return make_shared< Impl<ID::DoubleComplex> >();
	    // 	break;

	    case ID::UShort:
		return make_shared< Impl<ID::UShort> >();
		break;

	    case ID::ULong:
		return make_shared< Impl<ID::ULong> >();
		break;

	    default:
		throw Exception::Assert( "unknown type id" );

	    };

	}

    }


}
