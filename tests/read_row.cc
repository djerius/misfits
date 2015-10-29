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


class ReadTest : public ::testing::Test {

public:
    void SetUp() {
	gen_fits();
    }
};



struct read_row {

    misFITS::FilePtr file_;
    std::string file_name_;
    bool init;

    read_row( std::string file_name ) : file_name_( file_name ), init( false ) {}

    misFITS::FilePtr file() {

    	if ( ! init ) {
    	    file_ = misFITS::FilePtr( new misFITS::File( file_name_ ) );
    	    init = true;
    	}

    	return file_;
    }

    virtual ~read_row() { }
    virtual misFITS::Row row() = 0;

};

struct from_table_row : public read_row {

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

struct from_row_table : public read_row {

    misFITS::TablePtr table_;
    bool init;

    from_row_table( std::string file_name ) :
	read_row( file_name )
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

struct from_row_file : public read_row {

    from_row_file( std::string file_name ) :
	read_row( file_name )
    {}

    misFITS::Row
    row () {
    	return misFITS::Row( *file() );
    }
};

class ReadRowTest : public ReadTest, public ::testing::WithParamInterface< shared_ptr<read_row> > {};


TEST_P( ReadRowTest, ReadRow ) {

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


    misFITS::Row row ( GetParam()->row() );

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

    	int i = row.idx() - 1;

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

    ASSERT_EQ( 21, row.idx() );
}

INSTANTIATE_TEST_CASE_P( ReadRow,
			 ReadRowTest,
			 ::testing::Values(
					   std::shared_ptr<read_row>( new from_table_row( TEST_FITS_QFILENAME ) ),
					   std::shared_ptr<read_row>( new from_row_table( TEST_FITS_QFILENAME ) ),
					   std::shared_ptr<read_row>( new from_row_file(  TEST_FITS_QFILENAME ) )
					   )
			 );
