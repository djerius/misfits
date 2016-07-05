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


// Test assumptions about CFITSIO

#include "gtest/gtest.h"

#include <fitsio.h>

#include "misfits/fits_p.hpp"

#include "util.hpp"
#include "columns.hpp"

using namespace misFITS_Test;

TEST( CFITSIO, A_120A_NULL ) {

    Columns cols;

    cols.add( "Acol", "120A" );

    TestFitsPtr fp = cols.create();

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ( 120, info.width );
    EXPECT_EQ(   1, info.naxis );
    EXPECT_EQ( 120, info.naxes[0] );

}

TEST( CFITSIO, A_120A_120 ) {

    Columns cols;

    cols.add( "Acol", "120A", "", "(120)" );

    TestFitsPtr fp = cols.create();

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "(120)", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ( 120, info.width );
    EXPECT_EQ(   1, info.naxis );
    EXPECT_EQ( 120, info.naxes[0] );

}

TEST( CFITSIO, A_120A_12_10 ) {

    Columns cols;

    cols.add( "Acol", "120A", "", "(12,10)" );

    TestFitsPtr fp = cols.create();

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "(12,10)", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ(  12, info.width );
    EXPECT_EQ(   2, info.naxis );
    EXPECT_EQ( 12, info.naxes[0] );
    EXPECT_EQ( 10, info.naxes[1] );

}

TEST( CFITSIO, A_120A12_NULL ) {

    Columns cols;

    cols.add( "Acol", "120A12" );

    TestFitsPtr fp = cols.create();

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ(  12, info.width );
    EXPECT_EQ(   1, info.naxis );
    EXPECT_EQ( 120, info.naxes[0] );

}

TEST( CFITSIO, A_120A12_120 ) {

    Columns cols;

    cols.add( "Acol", "120A12", "", "(120)" );

    TestFitsPtr fp = cols.create();

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "(120)", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ( 120, info.width );
    EXPECT_EQ(   1, info.naxis );
    EXPECT_EQ( 120, info.naxes[0] );

}

TEST( CFITSIO, A_120A12_12_10 ) {

    Columns cols;

    cols.add( "Acol", "120A12", "", "(12,10)" );

    TestFitsPtr fp = cols.create();

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "(12,10)", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ(  12, info.width );
    EXPECT_EQ(   2, info.naxis );
    EXPECT_EQ( 12, info.naxes[0] );
    EXPECT_EQ( 10, info.naxes[1] );

}

TEST( CFITSIO, B_34 ) {

    Columns cols;

    cols.add( "Bcol", "34B" );

    TestFitsPtr fp = cols.create();

    {
	ColInfo info( fp, 1 );

	EXPECT_EQ( 34, info.repeat );
	EXPECT_EQ( 1, info.width );
	EXPECT_EQ( 1, info.naxis );
	EXPECT_EQ( 34, info.naxes[0] );
    }

    {
    misFITS_CHECK_CFITSIO_EXPR
      (
       fits_modify_vector_len( fp.get(), 1, 39, &status )
       );

	ColInfo info( fp, 1 );

	EXPECT_EQ( 39, info.repeat );
	EXPECT_EQ( 1, info.width );
	EXPECT_EQ( 1, info.naxis );
	EXPECT_EQ( 39, info.naxes[0] );

    }

}

// the flush tests rely upon the non-updating of the number of rows,
// so make sure the CFITSIO behavior is deterministic

TEST( CFITSIO, Flush ) {

    Columns cols;
    long c0 = 33;
    cols.add( "c0", "J" );

    TestFitsPtr fp0 = cols.create();

    LONGLONG nrows = 99999;
    misFITS_CHECK_CFITSIO_EXPR( fits_get_num_rowsll( fp0.get(), &nrows, &status ) );
    ASSERT_EQ( 0, nrows );

    misFITS_CHECK_CFITSIO_EXPR( fits_write_col( fp0.get(), TLONG, 1, 1, 1, 1, &c0, &status ) );

    // flush everything to get a consistent state, as fp0 is not the test subject
    misFITS_CHECK_CFITSIO_EXPR( fits_flush_file( fp0.get(), &status ) );

    TestFitsPtr fp1 = cols.create();
    c0 = 22;
    misFITS_CHECK_CFITSIO_EXPR( fits_write_col( fp1.get(), TLONG, 1, 1, 1, 1, &c0, &status ) );

    // if fp1 isn't flushed, it's NAXIS2 value isn't updated and
    // fits_copy_col sees num_rows(fp1) = 0.
    //
    // as it writes min( num_rows(fp0), num_rows(fp1)) rows (which is
    // 0), the row in fp1 does not get overwritten.
    misFITS_CHECK_CFITSIO_EXPR( fits_copy_col( fp0.get(), fp1.get(), 1, 1, 0, &status ) );
    misFITS_CHECK_CFITSIO_EXPR( fits_read_col( fp1.get(), TLONG, 1, 1, 1, 1, NULL, &c0, NULL, &status ) );
    ASSERT_EQ( 22, c0 );

    // Note however, that the _internal_ count of the number of rows is correct
    misFITS_CHECK_CFITSIO_EXPR( fits_get_num_rowsll( fp0.get(), &nrows, &status ) );
    ASSERT_EQ( 1, nrows );

    // even if the buffer is flushed, NAXIS2 is not updated
    misFITS_CHECK_CFITSIO_EXPR( fits_flush_buffer( fp1.get(), 0, &status ) );
    misFITS_CHECK_CFITSIO_EXPR( fits_copy_col( fp0.get(), fp1.get(), 1, 1, 0, &status ) );
    misFITS_CHECK_CFITSIO_EXPR( fits_read_col( fp1.get(), TLONG, 1, 1, 1, 1, NULL, &c0, NULL, &status ) );
    ASSERT_EQ( 22, c0 );

    // need to flush the whole file to get NAXIS2 updated
    misFITS_CHECK_CFITSIO_EXPR( fits_flush_file( fp1.get(), &status ) );
    misFITS_CHECK_CFITSIO_EXPR( fits_copy_col( fp0.get(), fp1.get(), 1, 1, 0, &status ) );
    misFITS_CHECK_CFITSIO_EXPR( fits_read_col( fp1.get(), TLONG, 1, 1, 1, 1, NULL, &c0, NULL, &status ) );
    ASSERT_EQ( 33, c0 );

}
