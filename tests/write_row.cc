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
#include <vector>

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
	    .add( "I1",  ColumnType::ID::Short     )
	    .add( "IV1", ColumnType::ID::Short, 10 )
	    .add( "J1",  ColumnType::ID::Long )
	    .add( "JV1", ColumnType::ID::Long, 10 )
	    .add( "L1",  ColumnType::ID::Logical )
	    .add( "LV1", ColumnType::ID::Logical, 10 )
	    .add( "E1",  ColumnType::ID::Float    )
	    .add( "EV1", ColumnType::ID::Float, 10    )
	    .add( "D1",  ColumnType::ID::Double   )
	    .add( "DV1", ColumnType::ID::Double, 10   )
	    .add( "X1",  ColumnType::ID::Bit	    , static_cast<LONGLONG>(Fiducial::Data::nbits) )
	    .add( "A1",  ColumnType::ID::String  , 60 )
	    .add( "A2",  ColumnType::ID::String  , Extent( 12, 5 ) )
	    .add( "A3",  ColumnType::ID::String  , Extent( 12, 5 ) )
	    .add( "A4",  ColumnType::ID::String  , 60 )
	    .add( "A5",  ColumnType::ID::String  , Extent( 12, 5 ) )
	    .add( "A6",  ColumnType::ID::String  , Extent( 12, 5 ) )
	    ;

    }

    misFITS::TablePtr otable( ttable.copy( output, TableCopy::Header ) );

    struct Row {
	Fiducial::Data::I_TYPE I1;
	std::vector<Fiducial::Data::I_TYPE> IV1;

	Fiducial::Data::J_TYPE J1;
	std::vector<Fiducial::Data::J_TYPE> JV1;

	Fiducial::Data::L_TYPE L1;
	std::vector<Fiducial::Data::L_TYPE> LV1;

	Fiducial::Data::E_TYPE E1;
	std::vector<Fiducial::Data::E_TYPE> EV1;

	Fiducial::Data::D_TYPE D1;
	std::vector<Fiducial::Data::D_TYPE> DV1;

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
    	.add( "IV1", &data.IV1 )

    	.add( "J1", &data.J1 )
    	.add( "JV1", &data.JV1 )

    	.add( "L1", &data.L1 )
    	.add( "LV1", &data.LV1 )

        .add( "E1", &data.E1 )
        .add( "EV1", &data.EV1 )

        .add( "D1", &data.D1 )
        .add( "DV1", &data.DV1 )

        .add( "X1", &data.X1 )
        .add( "A1", &data.A1 )
        .add( "A2", &data.A2 )
        .add( "A3", &data.A3 )
        .add( "A4", &data.A4 )
        .add( "A5", &data.A5 )
        .add( "A6", &data.A6 )
	;


    for( size_t row = 0 ; row < fid.nrows ; ++row ) {

	data.I1 = fid.i1.data[row];
	data.IV1 =  fid.iv1.data[row];

	data.J1 = fid.j1.data[row];
	data.JV1 = fid.jv1.data[row];

	data.L1 = fid.l1.data[row];
	data.LV1 = fid.lv1.data[row];

	data.E1 = fid.e1.data[row];
	data.EV1 = fid.ev1.data[row];

	data.D1 = fid.d1.data[row];
	data.DV1 = fid.dv1.data[row];

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
