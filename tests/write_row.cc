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

#include <iostream>
#include <string>

using namespace std;

#include <boost/typeof/typeof.hpp>
#include <boost/typeof/std/string.hpp>
#define misFITS_OFFSET_COL( fieldname, structure, structurename )			\
    column<BOOST_TYPEOF_TPL(structure.structurename)>(fieldname, offsetof( structure, structurename ))

#include "gtest/gtest.h"

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "fiducial_data.hpp"
#include "row_test.hpp"

using namespace misFITS;
using namespace misFITS_Test;


struct from_fileptr : public read_row {

    from_fileptr( misFITS::FilePtr& fp  ) :
	read_row( "from_fileptr", fp )
    {}

    misFITS::Row
    row () {
	misFITS::FilePtr fp = file();
    	return misFITS::Row( fp );
    }
};

class WriteTest : public GenFits {};


TEST_F( WriteTest, Create ) {

    Fiducial::Data fid;

    // open for writing
    misFITS::FilePtr output( misFITS::open<misFITS::Entity::Memory>() );

    misFITS::Table ttable( "stuff" );

    {
	using namespace misFITS;

	ttable
	    .add( "I1", ColumnType::Long     )
	    .add( "J1", ColumnType::LongLong )
	    .add( "E1", ColumnType::Float    )
	    .add( "D1", ColumnType::Double   )
	    .add( "X1", ColumnType::Bit	    , "", Extent( Fiducial::Data::nbits ) )
	    .add( "A1", ColumnType::String  , "", Extent( 60 ) )
	    .add( "A2", ColumnType::String  , "", Extent( 12, 5 ) )
	    .add( "A3", ColumnType::String  , "", Extent( 12, 5 ) )
	    .add( "A4", ColumnType::String  , "", Extent( 60 ) )
	    .add( "A5", ColumnType::String  , "", Extent( 12, 5 ) )
	    .add( "A6", ColumnType::String  , "", Extent( 12, 5 ) )
	    ;

    }

    misFITS::TablePtr otable( ttable.copy( output, TableCopy::Header ) );

    struct Row {
	int I1;
	short J1;
	float E1;
	double D1;
	misFITS::BitSet X1;
	string A1;
	vector<string> A2;
	vector<string> A3;

	string A4;
	vector<string> A5;
	vector<string> A6;

    } data;


    misFITS::Row orow( otable );

    orow
    	.add( "I1", &data.I1 )
    	.add( "J1", &data.J1 )
        .add( "E1", &data.E1 )
        .add( "D1", &data.D1 )
        .add( "X1", &data.X1 )
        .add( "A1", &data.A1 )
        .add( "A2", &data.A2 )
        .add( "A3", &data.A3 )
        .add( "A4", &data.A4 )
        .add( "A5", &data.A5 )
        .add( "A6", &data.A6 )
	;


    for( int row = 0 ; row < fid.nrows ; ++row ) {

	data.I1 = fid.i1.data[row];
	data.J1 = fid.j1.data[row];
	data.E1 = fid.e1.data[row];
	data.D1 = fid.d1.data[row];
	data.X1 = fid.x2.data[row];

	data.A1 = fid.a1.data[row];
	data.A2 = fid.a2.data[row];
	data.A3 = fid.a3.data[row];
	data.A4 = fid.a4.data[row];
	data.A5 = fid.a5.data[row];
	data.A6 = fid.a6.data[row];

	orow.write();
    }


    misFITS::Row test( otable );
    test_fiducial( test );

}
