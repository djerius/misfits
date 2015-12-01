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

#include <misfits/types.hpp>


namespace misFITS {

    static ColumnCode::Map
    create_columncode() {

	ColumnCode::Map t;
	t[CT_BIT] 	   = "X";
	t[CT_BYTE]         = "B";
	t[CT_LOGICAL]      = "L";
	t[CT_STRING]       = "A";
	t[CT_SHORT]        = "I";
	t[CT_INT32BIT]     = "J";
	t[CT_LONG]         = "J";
	t[CT_LONGLONG]     = "K";
	t[CT_FLOAT]        = "E";
	t[CT_DOUBLE]       = "D";
	t[CT_COMPLEX]      = "C";
	t[CT_DBL_COMPLEX]  = "M";
	t[CT_USHORT]       = "U";
	t[CT_ULONG]        = "V";
	return t;
    }

    ColumnCode::Map ColumnCode::code = create_columncode();

}
