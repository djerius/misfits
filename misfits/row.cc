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

#include <boost/bind.hpp>
#include <boost/core/ref.hpp>

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

namespace misFITS {

    namespace Entry {

	void
	Column::write( const File& file, LONGLONG firstrow, void* data ) {
	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_col( file.fptr(),
				 storage_type_,
				 colnum_,
				 firstrow, 1,
				 nelem_,
				 data, &status)
		 );
	}

	void
	Column::read( const File& file, LONGLONG firstrow, void* data ) {

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_read_col( file.fptr(),
				storage_type_,
				colnum_,
				firstrow, 1,
				nelem_,
				NULL,
				data, NULL, &status)
		 );
	}


	void
	AbsoluteColumn::read( const File& file, LONGLONG firstrow ) {

	    Column::read( file, firstrow, base_ );

	}

	void
	AbsoluteColumn::write( const File& file, LONGLONG firstrow ) {

	    Column::write( file, firstrow, base_ );

	}

	void
	OffsetColumn:: write( const File& file, LONGLONG firstrow, void *base  ) {
	    Column::write( file, firstrow, static_cast<char*>(base) + offset_ );
	}

	void
	OffsetColumn:: read( const File& file, LONGLONG firstrow, void *base  ) {
	    Column::read( file, firstrow, static_cast<char*>(base) + offset_ );
	}


	const ColumnInfo&
	Group::column_info( const std::string& column_name ) {

	    return row_->table->column( column_name );

	}

	void
	Group::column( const std::string& column_name, misFITS::StorageType type, size_t offset ) {

	    const misFITS::ColumnInfo& ci ( column_info( column_name ) );
	    entries.push_back( new Entry::OffsetColumn( ci.colnum,
							ci.nelem(),
							type,
							offset ) );
	}

	void
	Group::column( const char* column_name, misFITS::StorageType type, size_t offset ) {
	    column( std::string(column_name), type, offset );
	}

	void
	Group::read( const File& file, LONGLONG firstrow, void* base ) {
	    for_each( entries.begin(), entries.end(),
		      boost::bind( &Offset::read, _1, boost::ref(file), firstrow, base )
		      );
	}

	void
	Group::write( const File& file, LONGLONG firstrow, void* base ) {
	    for_each( entries.begin(), entries.end(),
		      boost::bind( &Offset::write, _1, boost::ref(file), firstrow, base )
		      );
	}

	GroupOffset*
	GroupOffset::group( size_t offset ) {
	    GroupOffset* grp = new GroupOffset( row_, offset );
	    entries.push_back( grp );
	    return grp;
	}

	void
	GroupOffset::read( const File& file, LONGLONG firstrow, void* base ) {
	    Group::read( file, firstrow, static_cast<char*>(base) + offset_ );
	}

	void
	GroupOffset::write( const File& file, LONGLONG firstrow, void* base ) {
	    Group::write( file, firstrow, static_cast<char*>(base) + offset_ );
	}

	GroupOffset*
	GroupAbsolute::group( size_t offset ) {
	    GroupOffset* grp = new GroupOffset( row_, offset );
	    entries.push_back( grp );
	    return grp;
	}

	void
	GroupAbsolute::read( const File& file, LONGLONG firstrow ) {
	    Group::read( file, firstrow, base_ );
	}

	void
	GroupAbsolute::write( const File& file, LONGLONG firstrow ) {
	    Group::write( file, firstrow, base_ );
	}

    }

    bool
    Row::read(){

	if ( table->row_idx() > table->num_rows() )
	    return false;

	const File& fref(  );

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::Absolute::read, _1, boost::ref(*table->file()), table->row_idx() ) );

	if (table->auto_advance)
	    table->advance_row();

	return true;
    }

    void
    Row::write() {

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::Absolute::write, _1, boost::ref(*table->file()), table->row_idx() ) );

	if (table->auto_advance)
	    table->advance_row();
    }


    Entry::GroupDSL<Row>
    Row::group( void* base ) {

	Entry::GroupAbsolute* grp = new Entry::GroupAbsolute( this, base );
	entries.push_back( grp );

	return Entry::GroupDSL<Row>( this, grp );
    }



}
