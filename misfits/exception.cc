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
#include <cstring>

namespace misFITS {

    Exception::CFITSIO::CFITSIO( int status ) : status_( status ) {

	fits_get_errstatus( status_, error );

	char* ptr = error + std::strlen( error );
	ptr += strlen( ptr );
	*ptr = '\n';
	*(++ptr) = '\0';

	bool more;

	do {
	    more = fits_read_errmsg( ptr ) ;
	    ptr += strlen( ptr );
	    *ptr = '\n';
	    *(++ptr) = '\0';
 	} while ( more );

    }

    const char*
    Exception::CFITSIO::what() const throw() {
	return error;
    }

}
