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

#include <misfits/config.hpp>
#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "gtest/gtest.h"

#include "row_utils.hpp"

#include "fiducial_data.hpp"

using namespace misFITS_Test;

void
test_fiducial( misFITS::Row &row ) {

    Fiducial::Data fid;
    fid.normalize_data();

    #define NBYTES 3
    if ( NBYTES != fid.nbytes )
	throw misFITS::Exception::Assert( "internal error: NBYTES != fid.nbytes" );

    struct A {
	int I1;
	short J1;
    };

    struct B {
	float E1;
	double D1;
    };

    struct C {
	misFITS::BitSet X1_bitset;
	misFITS::byte_t X1_array[NBYTES];
	vector<misFITS::byte_t> X1_vector;
    };

    struct Row {
	struct A a;
	struct B b;
	struct C c;
    } storage;

    int I1;
    short J1;
    float E1;
    double D1;
    misFITS::BitSet X1_bitset;

    misFITS::byte_t X1_array[NBYTES];
    vector<misFITS::byte_t> X1_vector;

    struct  {
	string str;
	vector<string> vec;
    } As[6];

    row
    	.column( "I1", &I1 )
    	.column( "J1", &J1 )
        .column( "E1", &E1 )
        .column( "D1", &D1 )
	.column( "X1", &X1_bitset )
	.column( "X1", X1_array )
	.column( "X1", &X1_vector )


    	.memblock( &storage)
	  .memblock( offsetof( Row, a ) )
	    .column<int>( "I1", offsetof( A, I1 ) )
	    .column<short>( "J1", offsetof( A, J1 ) )
	  .end_memblock()

	  .memblock( offsetof( Row, b ) )
	    .column<float>( "E1", offsetof( B, E1 ) )
	    .column<double>( "D1", offsetof( B, D1 ) )
	  .end_memblock()

	  .memblock( offsetof( Row, c ) )
	    .column<misFITS::BitSet>(  "X1", offsetof( C, X1_bitset ) )
	    .column< misFITS::byte_t > ( "X1", offsetof( C, X1_array ) )
	    .column< vector<misFITS::byte_t> > ( "X1", offsetof( C, X1_vector ) )
	  .end_memblock()
	.end_memblock()
    	;

    for ( int ia = 0 ; ia < 6 ; ++ia ) {

	ostringstream name;
	name << "A" << ia+1;

	row
	    .column( name.str(), &As[ia].str )
	    .column( name.str(), &As[ia].vec )
	    ;
    }


    while( row.read() ) {

	SCOPED_TRACE( row.idx() - 1 );

	// row.idx() returns next row to be read
	LONGLONG zidx = row.idx() - 2;

    	LONGLONG i = row.idx() - 1;

    	EXPECT_EQ( fid.i1.data[zidx], I1 );
    	EXPECT_EQ( fid.i1.data[zidx], storage.a.I1 );

    	EXPECT_EQ( fid.j1.data[zidx], J1 );
    	EXPECT_EQ( fid.j1.data[zidx], storage.a.J1 );

    	EXPECT_FLOAT_EQ( fid.e1.data[zidx], E1 );
    	EXPECT_FLOAT_EQ( fid.e1.data[zidx], storage.b.E1 );

    	EXPECT_DOUBLE_EQ( fid.d1.data[zidx], D1 );
    	EXPECT_DOUBLE_EQ( fid.d1.data[zidx], storage.b.D1 );

	EXPECT_TRUE( X1_bitset[Fiducial::Data::nbits - i] );
	EXPECT_TRUE( storage.c.X1_bitset[Fiducial::Data::nbits - i] );

	EXPECT_EQ( fid.x1.data[zidx], X1_vector );
	EXPECT_EQ( fid.x1.data[zidx], storage.c.X1_vector );

	vector<misFITS::byte_t> X1( X1_array, X1_array + fid.nbytes  );
	EXPECT_EQ( fid.x1.data[zidx], X1 );

	X1.assign( storage.c.X1_array, storage.c.X1_array + fid.nbytes );
	EXPECT_EQ( fid.x1.data[zidx], X1 );

	EXPECT_EQ( fid.a1.data[zidx], As[0].str );
	EXPECT_EQ( fid.a1.data[zidx], As[0].vec[0] );

	EXPECT_EQ( fid.a1.data[zidx], As[1].str );
	EXPECT_EQ( fid.a2.data[zidx], As[1].vec );

	EXPECT_EQ( fid.a1.data[zidx], As[2].str );
	EXPECT_EQ( fid.a3.data[zidx], As[2].vec );

	EXPECT_EQ( fid.a4.data[zidx], As[3].str );
	EXPECT_EQ( fid.a4.data[zidx], As[3].vec[0] );

	EXPECT_EQ( fid.a4.data[zidx], As[4].str );
	EXPECT_EQ( fid.a5.data[zidx], As[4].vec );

	EXPECT_EQ( fid.a4.data[zidx], As[5].str );
	EXPECT_EQ( fid.a6.data[zidx], As[5].vec );

    }

    ASSERT_EQ( fid.nrows + 1, row.idx() );
}

