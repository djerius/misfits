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

#include "util.hpp"

#include "misfits/fits.hpp"
#include "misfits/table.hpp"

namespace Entity = misFITS::Entity;
namespace Mode = misFITS::Mode;


TEST( FITSTempTable, CreateTable ) {

    misFITS::Table table( "MYEXTENT" );

    EXPECT_EQ( "MYEXTENT",  table.extname );

    EXPECT_EQ( 0, table.num_columns() );

    table.add( "col2", misFITS::CT_DOUBLE, 1 );
    table.add( "col1", misFITS::CT_DOUBLE, 1, 1 );
    table.add( "col3", misFITS::CT_DOUBLE, 1, 0 );

    EXPECT_EQ( 3, table.num_columns() );
    EXPECT_EQ( "col1", table.column(1).ttype );
    EXPECT_EQ( "col2", table.column(2).ttype );
    EXPECT_EQ( "col3", table.column(3).ttype );

    misFITS::FilePtr file( misFITS::open<Entity::Memory>() );
    table.copy( file );


    misFITS::Table table2( file );

    EXPECT_EQ( 3, table2.num_columns() );
    EXPECT_EQ( "col1", table2.column(1).ttype );
    EXPECT_EQ( "col2", table2.column(2).ttype );
    EXPECT_EQ( "col3", table2.column(3).ttype );

    table2.add( "col4", misFITS::CT_DOUBLE, 1, 0 );

    EXPECT_EQ( 3, table.num_columns() );
    EXPECT_EQ( 4, table2.num_columns() );

}

