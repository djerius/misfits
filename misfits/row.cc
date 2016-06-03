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

#include <cmath>

#include <boost/bind.hpp>
#include <boost/core/ref.hpp>

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "fits_p.hpp"

namespace misFITS {

    //////////////////////
    // Row Constructors //
    //////////////////////

    void
    Row::init( ) {
	idx(1);
	auto_advance( true );
    }

    Row::Row( own_or_observe::rptr<Table>* table )  {
	init();
	table_.set( table );
    }

    Row::Row( TablePtr& table )  {
	init();
	table_.set<own_or_observe::observed>( table );
    }

    Row::Row( Table& table ) {
	init();
	table_.set<own_or_observe::observed>( &table );
    }

    Row::Row( FilePtr& file, int hdu_num ) {
	init();
	TablePtr fp = TablePtr( new Table( file, hdu_num ) );
	table_.set<own_or_observe::owned>( fp );
    }

    Row::Row( FilePtr& file, const std::string& extname, int extver ) {
	init();
	TablePtr fp = TablePtr( new Table( file, extname, extver ) );
	table_.set<own_or_observe::owned>( fp );
    }

    bool
    Row::read(){

	table_->set_as_chdu();

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to read" );

	if ( idx() > table_->num_rows() )
	    return false;

	for_each( entries.begin(), entries.end(),
		  boost::bind( &RowEntry::ColumnBase::read, _1, boost::ref(*table_.get()), idx() )
		  );

	if ( auto_advance() )
	    advance();

	return true;
    }

    void
    Row::write() {

	table_->set_as_chdu();

	if ( ! entries.size() )
	    throw Exception::Assert( "row object was not assigned any columns to write" );

	for_each( entries.begin(), entries.end(),
		  boost::bind( &RowEntry::ColumnBase::write, _1, boost::ref(*table_.get()), idx() )
		  );

	if ( auto_advance() )
	    advance();
    }



}
