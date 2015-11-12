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

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "row_test.hpp"
#include "util.hpp"


class ReadTest : public ::testing::Test {

public:
    void SetUp() {
      gen_fits();
    }
};

struct from_table_row : public read_row, public generate_fits {

    misFITS::TablePtr table_;
    bool init;

    from_table_row( std::string file_name ) :
	read_row( file_name ),
	init( false )
    {}

    misFITS::Row
    row () {
	if ( ! init ) {
	    table_ =  file()->table();
	    init = true;
	}
	return table_->row();
    }
};

struct from_row_table : public read_row, public generate_fits {

    misFITS::TablePtr table_;
    bool init;

    from_row_table( std::string file_name ) :
	read_row( file_name ),
	init( false )
    {}


    misFITS::Row
    row () {
	if ( ! init ) {
	    table_ =  file()->table();
	    init = true;
	}
	return misFITS::Row( table_ );
    }
};

struct from_row_file : public read_row, public generate_fits {

    from_row_file( std::string file_name ) :
	read_row( file_name )
    {}

    misFITS::Row
    row () {
    	return misFITS::Row( *file() );
    }
};



INSTANTIATE_TEST_CASE_P( ReadRow,
			 ReadRowTest,
			 ::testing::Values(
					   std::shared_ptr<read_row>( new from_table_row( TEST_FITS_QFILENAME ) ),
					   std::shared_ptr<read_row>( new from_row_table( TEST_FITS_QFILENAME ) ),
					   std::shared_ptr<read_row>( new from_row_file(  TEST_FITS_QFILENAME ) )
					   )
			 );



TEST_F( ReadTest, Cursors ) {


    misFITS::File file( TEST_FITS_QFILENAME );

    misFITS::Table table( file );

    misFITS::Row r1( table );

    int Icol;

    r1.column( "Icol", &Icol );

    r1.read();
    ASSERT_EQ( 2, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    r1.read();
    ASSERT_EQ( 3, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    misFITS::Row r2( table );

    r2.column( "Icol", &Icol );

    r2.read();
    ASSERT_EQ( 2, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );

    r2.read();
    ASSERT_EQ( 3, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );

    r1.read();
    r1.read();
    ASSERT_EQ( 5, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    r2.read();
    ASSERT_EQ( 4, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );

    // read specific rows
    r1.read( 1 );
    ASSERT_EQ( 2, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    r2.read( 3 );
    ASSERT_EQ( 4, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );


}

TEST_F( ReadTest, CopyRow ) {

    misFITS::File file( TEST_FITS_QFILENAME );

    misFITS::Table table( file );

    misFITS::Row r1( table );

    int Icol;

    r1.column( "Icol", &Icol );

    r1.read();
    ASSERT_EQ( 2, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    r1.read();
    ASSERT_EQ( 3, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    misFITS::Row r2( r1 );
    ASSERT_EQ( 3, r2.idx() );

    r2.read();
    ASSERT_EQ( 4, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );

    r2.read();
    ASSERT_EQ( 5, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );

    r1.read();
    r1.read();
    ASSERT_EQ( 5, r1.idx() );
    EXPECT_EQ( r1.idx(), Icol );

    r2.read();
    ASSERT_EQ( 6, r2.idx() );
    EXPECT_EQ( r2.idx(), Icol );
}
