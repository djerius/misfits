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

using namespace std;

#include "gtest/gtest.h"

#include "util.hpp"

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

class FITSFileTest : public ::testing::Test {

protected :

    void SetUp() {
	gen_fits();
	file.reset( new misFITS::File( TEST_FITS_QFILENAME ) );
    }

    misFITS::FilePtr file;
};


TEST_F( FITSFileTest, MetaData ) {

    EXPECT_STREQ( TEST_FITS_QFILENAME, file->file.c_str());

    misFITS::Table table( file );

    ASSERT_EQ( 4, table.num_columns() );

    ASSERT_TRUE( table.exists_column( "Icol" ) );
    ASSERT_TRUE( table.exists_column( "Jcol" ) );
    ASSERT_TRUE( table.exists_column( "Ecol" ) );
    ASSERT_TRUE( table.exists_column( "Dcol" ) );

    ASSERT_EQ( 1, table.column("Icol" ).colnum );
    ASSERT_EQ( 2, table.column( "Jcol" ).colnum );
    ASSERT_EQ( 3, table.column( "Ecol" ).colnum );
    ASSERT_EQ( 4, table.column( "Dcol" ).colnum );
}


TEST_F( FITSFileTest, ReadRow ) {

    misFITS::Table table( file );

    struct A {
	int Icol;
	short Jcol;
    };

    struct B {
	float Ecol;
	double Dcol;
    };

    struct Row {
	struct A a;
	struct B b;
    } storage;

    int Icol;
    short Jcol;
    float Ecol;
    double Dcol;

    misFITS::Row row( table.row() );

    row
	.column( "Icol", &Icol )
	.column( "Jcol", &Jcol )
        .column( "Ecol", &Ecol )
        .column( "Dcol", &Dcol )
	.group( &storage)
	.group( offsetof( Row, a ) )
	.column<int>( "Icol", offsetof( A, Icol ) )
	.column<short>( "Jcol", offsetof( A, Jcol ) )
	.endgroup()
	.group( offsetof( Row, b ) )
        .column<float>( "Ecol", offsetof( B, Ecol ) )
        .column<double>( "Dcol", offsetof( B, Dcol ) )
	.endgroup()
	;


    while( row.read() ) {

	int i = table.row_idx() - 1;

	double D = 1.0 / i;
	float  E = 2.0 / i;
	short  I = i + 1;
	int    J = i + 2;

	ASSERT_EQ( I, storage.a.Icol );
	ASSERT_EQ( J, storage.a.Jcol );
	ASSERT_FLOAT_EQ( D, storage.b.Dcol );
	ASSERT_DOUBLE_EQ( E, storage.b.Ecol );

	ASSERT_EQ( I, Icol );
	ASSERT_EQ( J, Jcol );

	ASSERT_FLOAT_EQ( D, Dcol );
	ASSERT_DOUBLE_EQ( E, Ecol );

    }

    ASSERT_EQ( 21, table.row_idx() );
}
