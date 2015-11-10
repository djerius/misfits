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

#include <misfits/config.hpp>
#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "gtest/gtest.h"

#include "row_utils.hpp"

void
test_fiducial( misFITS::Row &row ) {

#define NROWS 20
#define NBITS 23
#define NBYTES 3
    struct A {
	int Icol;
	short Jcol;
    };

    struct B {
	float Ecol;
	double Dcol;
    };

    struct C {
	misFITS::BitStore Xcol_bitstore;
	misFITS::byte_t Xcol_array[NBYTES];
	vector<misFITS::byte_t> Xcol_vector;
    };

    struct Row {
	struct A a;
	struct B b;
	struct C c;
    } storage;

    int Icol;
    short Jcol;
    float Ecol;
    double Dcol;
    misFITS::BitStore Xcol_bitstore;
    misFITS::byte_t Xcol_array[NBYTES];
    vector<misFITS::byte_t> Xcol_vector;


    row
    	.column( "Icol", &Icol )
    	.column( "Jcol", &Jcol )
        .column( "Ecol", &Ecol )
        .column( "Dcol", &Dcol )
	.column( "Xcol", &Xcol_bitstore )
	.column( "Xcol", Xcol_array )
	.column( "Xcol", &Xcol_vector )

    	.memblock( &storage)
	  .memblock( offsetof( Row, a ) )
	    .column<int>( "Icol", offsetof( A, Icol ) )
	    .column<short>( "Jcol", offsetof( A, Jcol ) )
	  .end_memblock()

	  .memblock( offsetof( Row, b ) )
	    .column<float>( "Ecol", offsetof( B, Ecol ) )
	    .column<double>( "Dcol", offsetof( B, Dcol ) )
	  .end_memblock()

	  .memblock( offsetof( Row, c ) )
	    .column<misFITS::BitStore>(  "Xcol", offsetof( C, Xcol_bitstore ) )
	    .column< misFITS::byte_t > ( "Xcol", offsetof( C, Xcol_array ) )
	    .column< vector<misFITS::byte_t> > ( "Xcol", offsetof( C, Xcol_vector ) )
	  .end_memblock()
	.end_memblock()
    	;


    while( row.read() ) {

    	int i = row.idx() - 1;

    	double D = 1.0 / i;
    	float  E = 2.0 / i;
    	short  I = i + 1;
    	int    J = i + 2;

    	ASSERT_EQ( I, Icol );
    	ASSERT_EQ( I, storage.a.Icol );

    	ASSERT_EQ( J, Jcol );
    	ASSERT_EQ( J, storage.a.Jcol );

    	ASSERT_FLOAT_EQ( E, Ecol );
    	ASSERT_FLOAT_EQ( E, storage.b.Ecol );

    	ASSERT_DOUBLE_EQ( D, Dcol );
    	ASSERT_DOUBLE_EQ( D, storage.b.Dcol );

	ASSERT_TRUE( Xcol_bitstore[NBITS - i] );
	ASSERT_TRUE( storage.c.Xcol_bitstore[NBITS - i] );

	int byte = (i - 1) / 8;
	int shift = ( 8 - (i - byte * 8) );

	for( int ibyt = 0 ; ibyt < NBYTES ; ++ibyt ) {

	    misFITS::byte_t value = ibyt == byte ? 1 << shift : 0;

	    ASSERT_EQ( value, Xcol_array[ibyt] );
	    ASSERT_EQ( value, Xcol_vector[ibyt] );

	    ASSERT_EQ( value, storage.c.Xcol_array[ibyt] );
	    ASSERT_EQ( value, storage.c.Xcol_vector[ibyt] );
	}

    }

    ASSERT_EQ( 21, row.idx() );
}

