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

#ifndef misFITS_EXCEPTION_H
#define misFITS_EXCEPTION_H

#include <stdexcept>

namespace misFITS {


    class Exception : public std::exception {

    public:
	class CFITSIO;
	class Assert : public std::runtime_error {
	public:
	    Assert( const char* what_arg ) : runtime_error( what_arg ) { }
	};

    };

    class Exception::CFITSIO : public Exception {

    public:
	CFITSIO( int status);
	const char* what() const  throw () ;

	int status () { return status_; }

    private:
	char error[26*82];
	int status_;
    };

}

#endif // ! misFITS_EXCEPTION_H
