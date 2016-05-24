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
#include "misfits/keyword.hpp"

namespace Entity = misFITS::Entity;
namespace Mode = misFITS::Mode;


TEST_F( FiducialTableROFptr, read ) {

    misFITS::Table table( file );

    EXPECT_EQ( "stuff", table.get_keyword<std::string>( "EXTNAME" ).value );
    EXPECT_EQ( 1, table.get_keyword<int>( "EXTVER" ).value );

    EXPECT_DOUBLE_EQ( 3.14159, table.get_keyword<double>( "PIE" ).value );
    EXPECT_DOUBLE_EQ( 3.14159, table.get_keyword<double>( "PIESTR" ).value );

    EXPECT_FLOAT_EQ( 3.14159f, table.get_keyword<float>( "PIE" ).value );
    EXPECT_FLOAT_EQ( 3.14159f, table.get_keyword<float>( "PIESTR" ).value );

    EXPECT_EQ( 3, table.get_keyword<int>( "PIE" ).value );
    EXPECT_EQ( 3, table.get_keyword<int>( "PIESTR" ).value );

    EXPECT_EQ( "3.14159", table.get_keyword<std::string>( "PIE" ).value );
    EXPECT_EQ( "3.14159", table.get_keyword<std::string>( "PIESTR" ).value );

}


TEST( Keywords, write ) {

    misFITS::Table table( "MYEXTENT" );

    EXPECT_EQ( "MYEXTENT", table.get_keyword<std::string>( "EXTNAME" ).value );
    EXPECT_EQ( 1, table.get_keyword<int>( "EXTVER" ).value );

    table.set_keyword( misFITS::Keyword<double>( "PIE", 3.14159 ) );
    EXPECT_DOUBLE_EQ( 3.14159, table.get_keyword<double>( "PIE" ).value );

    table.set_keyword( misFITS::Keyword<std::string>( "PIE", "Apple" ) );
    EXPECT_EQ( "Apple", table.get_keyword<std::string>( "PIE" ).value );

    std::string value(1000, 'X' );
    table.set_keyword( misFITS::Keyword<std::string>( "LONG", value ) );
    EXPECT_EQ( value, table.get_keyword<std::string>( "LONG" ).value );

}

TEST_F( FiducialTableRWFptr, deleteKeyword ) {

    misFITS::Table table( file );

    ASSERT_TRUE( table.has_keyword( "PIE" ) );

    EXPECT_DOUBLE_EQ( 3.14159, table.get_keyword<double>( "PIE" ).value );
    ASSERT_NO_THROW( table.delete_keyword( "PIE" ) );
    EXPECT_FALSE( table.has_keyword( "PIE" ) );

}


// TODO:

// check that comments are correctly written

// check that things are actually written to the file (not just read
// write from a temporary memory one)
