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

	const ColumnInfo&
	Group::column_info( const std::string& column_name ) {

	    return row_->table->column( column_name );

	}

	template<class T>
	void
	Group::column( const std::string& column_name, size_t offset ) {

	    const misFITS::ColumnInfo& ci ( column_info( column_name ) );
	    entries.push_back( std::make_shared< Entry::OffsetColumn<T> >( ci.colnum,
									   ci.nelem(),
									   offset ) );
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
	    entries.push_back( std::make_shared<GroupOffset>( row_, offset ) );
	    return static_cast<GroupOffset*>( entries.back().get() );
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
	    entries.push_back( std::make_shared<GroupOffset>( row_, offset ) );
	    return static_cast<GroupOffset*>( entries.back().get() );
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

    //////////////////////
    // Row Constructors //
    //////////////////////

    void
    Row::init( ) {
	idx(1);
	auto_advance( true );
    }

    Row::Row( own_or_observe::rptr<Table>* table_ )  {
	init();
	table.set( table_ );
    }

    Row::Row( TablePtr& table_ )  {
	init();
	table.set<own_or_observe::observed>( table_ );
    }

    Row::Row( Table& table_ ) {
	init();
	table.set<own_or_observe::observed>( &table_ );
    }

    Row::Row( FilePtr& file ) {
	init();
	TablePtr fp( file->table() );
	table.set<own_or_observe::owned>( fp );
    }

    Row::Row( File& file ) {
	init();
	TablePtr tp( file.table() );
	table.set<own_or_observe::owned>( tp );
    }



    bool
    Row::read(){

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to read" );

	if ( idx() > table->num_rows() )
	    return false;

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::Absolute::read, _1, boost::ref(*table->file()),
			       idx() )
		  );

	if ( auto_advance() )
	    advance();

	return true;
    }

    void
    Row::write() {

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to write" );

	for_each( entries.begin(), entries.end(),
		  boost::bind( &Entry::Absolute::write, _1,
			       boost::ref(*table->file()),
			       idx() )
		  );

	if ( auto_advance() )
	    advance();
    }


    Entry::GroupDSL<Row>
    Row::group( void* base ) {

	entries.push_back( std::make_shared<Entry::GroupAbsolute>( this, base ) );

	return Entry::GroupDSL<Row>( this, static_cast<Entry::GroupAbsolute *>( entries.back().get() ) );
    }



}
