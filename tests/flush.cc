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

#include "gtest/gtest.h"

#include "misfits/fits.hpp"
#include "misfits/table.hpp"
#include "misfits/row.hpp"

using namespace misFITS;

// See the FlushTest in cfitsio.cc for the tests which support this
// means of detecting if the flush method is mapping correctly to the
// CFITSIO flush routines.
TEST( FlushTest, FlushAwayRay ) {

    misFITS::Table table( "MyEXTENT" );

    table.add( "col1", ColumnType::Double );

    misFITS::Row row( table );
    double col1 = 33.5;
    row.add( "col1", &col1 );
    row.write();

    Keyword<long> nrows = table.file->read_key<long>( "NAXIS2" );
    ASSERT_EQ( 0, nrows.value );

    // explicit buffer flush, doesn't affect NAXIS2 keyword
    table.flush( FlushMode::Buffer );

    nrows = table.file->read_key<long>( "NAXIS2" );
    ASSERT_EQ( 0, nrows.value );

    // explicit file flush, updates NAXIS2 keyword
    table.flush( FlushMode::File );

    nrows = table.file->read_key<long>( "NAXIS2" );
    ASSERT_EQ( 1, nrows.value );

    row.write();

    nrows = table.file->read_key<long>( "NAXIS2" );
    ASSERT_EQ( 1, nrows.value );

    // default flush (which is a file flush), updates NAXIS2 keyword
    table.flush( );

    nrows = table.file->read_key<long>( "NAXIS2" );
    ASSERT_EQ( 2, nrows.value );

}

