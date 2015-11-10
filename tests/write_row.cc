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

#include <boost/typeof/typeof.hpp>
#include <boost/typeof/std/string.hpp>
#define misFITS_OFFSET_COL( fieldname, structure, structurename )			\
    column<BOOST_TYPEOF_TPL(structure.structurename)>(fieldname, offsetof( structure, structurename ))

#include "gtest/gtest.h"

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "row_test.hpp"


struct from_fileptr : public read_row {

    from_fileptr( misFITS::FilePtr& fp  ) :
	read_row( fp )
    {}

    misFITS::Row
    row () {
    	return misFITS::Row( *file() );
    }
};


TEST( WriteRow, Copy ) {


    // open for reading
    misFITS::File input( TEST_FITS_QFILENAME );

    // open for writing
    misFITS::FilePtr output( misFITS::open<misFITS::Entity::Memory>() );

    input.copy_hdu( output );

    misFITS::TablePtr otable( output->table() );

    struct Row {
	int Icol;
	short Jcol;
	float Ecol;
	double Dcol;
	misFITS::BitStore Xcol;
    } data;


    misFITS::Row irow( input );
    misFITS::Row orow( otable );

    irow.memblock( &data )
    	.column<int>( "Icol", offsetof( Row, Icol ) )
    	.column<short>( "Jcol", offsetof( Row, Jcol ) )
        .column<float>( "Ecol", offsetof( Row, Ecol ) )
        .column<double>( "Dcol", offsetof( Row, Dcol ) )
        .column<misFITS::BitStore>( "Xcol", offsetof( Row, Xcol ) )
	.end_memblock();

    orow.memblock( &data )
    	.column<int>( "Icol", offsetof( Row, Icol ) )
    	.column<short>( "Jcol", offsetof( Row, Jcol ) )
        .column<float>( "Ecol", offsetof( Row, Ecol ) )
        .column<double>( "Dcol", offsetof( Row, Dcol ) )
        .column<misFITS::BitStore>( "Xcol", offsetof( Row, Xcol ) )
	.end_memblock();

    while( irow.read() )
	orow.write();

    misFITS::Row test( otable );
    test_fiducial( test );

}

