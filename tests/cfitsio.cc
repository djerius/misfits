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

#include "util.hpp"
#include "columns.hpp"

using namespace misFITS_Test;

TEST( CFITSIO, A_120A_NULL ) {

    Columns cols;

    cols.add( "Acol", "120A" );

    TestFitsPtr fp = cols.create( "mem://" );

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

    TestFitsPtr fp = cols.create( "mem://" );

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

    TestFitsPtr fp = cols.create( "mem://" );

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

    TestFitsPtr fp = cols.create( "mem://" );

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

    TestFitsPtr fp = cols.create( "mem://" );

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

    TestFitsPtr fp = cols.create( "mem://" );

    ColInfo info( fp, 1 );

    EXPECT_STREQ( "(12,10)", info.tdim );
    EXPECT_EQ( 120, info.repeat );
    EXPECT_EQ(  12, info.width );
    EXPECT_EQ(   2, info.naxis );
    EXPECT_EQ( 12, info.naxes[0] );
    EXPECT_EQ( 10, info.naxes[1] );

}
