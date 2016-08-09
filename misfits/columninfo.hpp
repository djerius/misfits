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

#include <fitsio.h>

#include <misfits/fits.hpp>
#include <misfits/extent.hpp>

namespace misFITS {

    class ColumnInfo {

    public:

	// FITS column information
	std::string ttype;
	std::string tunit;
	ColumnType::SpecPtr  column_type;

	// offset of first byte from start of row. for use in directly
	// accessing raw data.  This is *not* initialized in the constructor for
	// this column
	LONGLONG offset;
	LONGLONG nbytes;

	// the shape of the data in a cell
	Extent extent;

	// this may be the only unique id for the column
	TableColumnsType::size_type colnum;

	ColumnInfo( const std::string& name, ColumnType::ID::type column_type,
		    const std::string& unit, const Extent& extent,
		    TableColumnsType::size_type = 0);

	// initialize from the CHDU in a fits file. requires byte offset into table storage for each column to handle
	// string ('A') columns.
	ColumnInfo( const misFITS::File& file, const std::string& name, LONGLONG offset );
	ColumnInfo( const misFITS::File& file, TableColumnsType::size_type, LONGLONG offset );

	void insert( const misFITS::File& file );

	bool operator == (const ColumnInfo& col ) const;

	bool operator != ( const ColumnInfo& col ) const { return ! operator==( col ) ; }

	// number of elements written to FITS header. may not be same read/written
	LONGLONG nelem() const { return extent.nelem(); }

	std::string tform () const;

    private:
	void init( const misFITS::File& file );

    };


}

#endif // ! misFITS_COLUMNINFO_H
