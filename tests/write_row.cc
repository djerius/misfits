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
    	return misFITS::Row( *file() );
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
	    .add( "I1", CT_LONG	    )
	    .add( "J1", CT_LONGLONG )
	    .add( "E1", CT_FLOAT    )
	    .add( "D1", CT_DOUBLE   )
	    .add( "X1", CT_BIT	   , "", Extent( Fiducial::Data::nbits ) )
	    .add( "A1", CT_STRING  , "", Extent( 60 ) )
	    .add( "A2", CT_STRING  , "", Extent( 12, 5 ) )
	    .add( "A3", CT_STRING  , "", Extent( 12, 5 ) )
	    .add( "A4", CT_STRING  , "", Extent( 60 ) )
	    .add( "A5", CT_STRING  , "", Extent( 12, 5 ) )
	    .add( "A6", CT_STRING  , "", Extent( 12, 5 ) )
	    ;

    }

    misFITS::TablePtr otable( ttable.copy( output ) );

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
    	.column( "I1", &data.I1 )
    	.column( "J1", &data.J1 )
        .column( "E1", &data.E1 )
        .column( "D1", &data.D1 )
        .column( "X1", &data.X1 )
        .column( "A1", &data.A1 )
        .column( "A2", &data.A2 )
        .column( "A3", &data.A3 )
        .column( "A4", &data.A4 )
        .column( "A5", &data.A5 )
        .column( "A6", &data.A6 )
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
