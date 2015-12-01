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

#ifndef misFITS_COLUMNINFO_H
#define misFITS_COLUMNINFO_H

#include <misfits/table.hpp>

namespace misFITS {

    class ColumnInfo {

    public:

	// FITS column information
	std::string ttype;
	std::string tunit;
	ColumnType  column_type;
	long width;
	LONGLONG repeat;

	LONGLONG nbytes;

	// the shape of the data in a cell
	Extent extent;

	// this may be the only unique id for the column
	int colnum;

	// ColumnInfo( const std::string& name,
	// 	    ColumnType column_type,
	// 	    const std::string& unit,
	// 	    const Extent& extent,
	// 	    int colnum = 0
	// 	    ) :
	// 		tunit( unit ),
	// 		column_type(column_type),
	// 		colnum( colnum )
	// { }

	// initialize from the CHDU in a fits file
	ColumnInfo( const misFITS::File& file, const std::string& name );
	ColumnInfo( const misFITS::File& file, int colnum );

	bool operator == (const ColumnInfo& col ) const;

	bool operator != ( const ColumnInfo& col ) const { return ! operator==( col ) ; }

	LONGLONG nelem() const { return nelem_; }

    private:
	void init( const misFITS::File& file );
	LONGLONG nelem_; // number of elements written to FITS header. may not be same read/written


    };


}

#endif // ! misFITS_COLUMNINFO_H
