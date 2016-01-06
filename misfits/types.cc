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
	t[ColumnType::Bit] 	     = "X";
	t[ColumnType::Byte]          = "B";
	t[ColumnType::Logical]       = "L";
	t[ColumnType::String]        = "A";
	t[ColumnType::Short]         = "I";
	t[ColumnType::Int32bit]      = "J";
	t[ColumnType::Long]          = "J";
	t[ColumnType::LongLong]      = "K";
	t[ColumnType::Float]         = "E";
	t[ColumnType::Double]        = "D";
	t[ColumnType::Complex]       = "C";
	t[ColumnType::DoubleComplex] = "M";
	t[ColumnType::UnsignedShort] = "U";
	t[ColumnType::UnsignedLong]  = "V";
	return t;
    }

    ColumnCode::Map ColumnCode::code = create_columncode();

}
