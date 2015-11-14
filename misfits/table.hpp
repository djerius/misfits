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

#ifndef misFITS_TABLE_H
#define misFITS_TABLE_H

#include <string>
#include <vector>

#include <misfits/types.hpp>
#include <misfits/fits.hpp>
#include <misfits/hdu.hpp>
#include <misfits/extent.hpp>
#include <misfits/columninfo.hpp>

namespace misFITS {

    class Row;

    class Table : public HDU {

    public:

	Table( const std::string&, int extver = 1 );
	Table( WeakFilePtr file );
	Table( File& file );

	const ColumnInfo& colinfo( int colnum );
	const ColumnInfo& colinfo( const std::string& name );

	// copy table header to other file
	TablePtr copy_header( FilePtr& file_ ) const;

	// copy table to other file
	TablePtr copy( FilePtr& file, int morekeys = 0 ) const;

	Table& add( const ColumnInfo& );

	Table& add( const std::string& ttype,
		    ColumnType typecode,
		    const Extent& extent = 1,
		    int colnum = 0);

	int num_columns() const;
	LONGLONG num_rows() const;

	bool exists_column( const std::string& templt )  const;
	void delete_column( int colnum );
	void delete_column( const std::string& name );

	misFITS::Row row();

    protected:

	// disable default copy constructors
	Table( const Table& );
	Table& operator= (const Table&);
	void refresh ();
	LONGLONG row_idx_;
	std::vector<ColumnInfo> columns;

    };

}


#endif // ! misFITS_TABLE_H
