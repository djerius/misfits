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

    #define NBYTES 3

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

struct D {
    struct A a;
    struct B b;

};

struct Row {
    struct A a;
    struct B b;
    struct C c;
    struct D d;
} storage;


void
test_fiducial( misFITS::Row &row ) {

    Fiducial::Data fid;
    fid.normalize_data();

    if ( NBYTES != fid.nbytes )
	throw misFITS::Exception::Assert( "internal error: NBYTES != fid.nbytes" );

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

    A atmp;
    B btmp;

    using misFITS::Entry::memblock;

    misFITS::Entry::MemBlockAddr<struct A> mb_a
	= memblock( &atmp )
	.add( "I1", &atmp.I1 )
	.add( "J1", &atmp.J1 );


    row
    	.add( "I1", &I1 )
    	.add( "J1", &J1 )
        .add( "E1", &E1 )
        .add( "D1", &D1 )
	.add( "X1", &X1_bitset )
	.add( "X1", X1_array )
	.add( "X1", &X1_vector )

	.add( &storage.a, mb_a )

	.add( &storage.b,
	      memblock( &btmp )
	      .add( "E1", &btmp.E1 )
	      .add( "D1", &btmp.D1 )
	      )


	.add( &storage.c,
	      memblock<C>()
	      .add<misFITS::BitSet>(  "X1", offsetof( C, X1_bitset ) )
	      .add< misFITS::byte_t > ( "X1", offsetof( C, X1_array ) )
	      .add< vector<misFITS::byte_t> > ( "X1", offsetof( C, X1_vector ) )
	      )

	.add( &storage.d,
	      memblock<D>()
	      .add( offsetof( D, a ), mb_a )
	      .add( offsetof( D, b ),
		    memblock( &btmp )
		    .add( "E1", &btmp.E1 )
		    .add( "D1", &btmp.D1 )
		    )
	      );
    	;

    for ( int ia = 0 ; ia < 6 ; ++ia ) {

	ostringstream name;
	name << "A" << ia+1;

	row
	    .add( name.str(), &As[ia].str )
	    .add( name.str(), &As[ia].vec )
	    ;
    }


    while( row.read() ) {

	SCOPED_TRACE( row.idx() - 1 );

	// row.idx() returns next row to be read
	LONGLONG zidx = row.idx() - 2;

    	EXPECT_EQ( fid.i1.data[zidx], I1 );
    	EXPECT_EQ( fid.i1.data[zidx], storage.a.I1 );
    	EXPECT_EQ( fid.i1.data[zidx], storage.d.a.I1 );

    	EXPECT_EQ( fid.j1.data[zidx], J1 );
    	EXPECT_EQ( fid.j1.data[zidx], storage.a.J1 );
    	EXPECT_EQ( fid.j1.data[zidx], storage.d.a.J1 );

    	EXPECT_FLOAT_EQ( fid.e1.data[zidx], E1 );
    	EXPECT_FLOAT_EQ( fid.e1.data[zidx], storage.b.E1 );
    	EXPECT_FLOAT_EQ( fid.e1.data[zidx], storage.d.b.E1 );

    	EXPECT_DOUBLE_EQ( fid.d1.data[zidx], D1 );
    	EXPECT_DOUBLE_EQ( fid.d1.data[zidx], storage.b.D1 );
    	EXPECT_DOUBLE_EQ( fid.d1.data[zidx], storage.d.b.D1 );

	EXPECT_EQ( fid.x2.data[zidx], X1_bitset );
	EXPECT_EQ( fid.x2.data[zidx], storage.c.X1_bitset );

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

