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


TEST_F( ExistingTable, read ) {

    ASSERT_EQ( "stuff", file->read_key<std::string>( "EXTNAME" ).value );
    ASSERT_EQ( 1, file->read_key<int>( "EXTVER" ).value );

    ASSERT_DOUBLE_EQ( 3.14159, file->read_key<double>( "PIE" ).value );
    ASSERT_DOUBLE_EQ( 3.14159, file->read_key<double>( "PIESTR" ).value );

    ASSERT_FLOAT_EQ( 3.14159f, file->read_key<float>( "PIE" ).value );
    ASSERT_FLOAT_EQ( 3.14159f, file->read_key<float>( "PIESTR" ).value );

    ASSERT_EQ( 3, file->read_key<int>( "PIE" ).value );
    ASSERT_EQ( 3, file->read_key<int>( "PIESTR" ).value );

    ASSERT_EQ( "3.14159", file->read_key<std::string>( "PIE" ).value );
    ASSERT_EQ( "3.14159", file->read_key<std::string>( "PIESTR" ).value );

}


TEST( Keywords, write ) {

    misFITS::Table table( "MYEXTENT" );

    ASSERT_EQ( "MYEXTENT", table.read_key<std::string>( "EXTNAME" ).value );
    ASSERT_EQ( 1, table.read_key<int>( "EXTVER" ).value );

    table.write_key( misFITS::Keyword<double>( "PIE", 3.14159 ) );
    ASSERT_DOUBLE_EQ( 3.14159, table.read_key<double>( "PIE" ).value );

    table.update_key( misFITS::Keyword<std::string>( "PIE", "Apple" ) );
    ASSERT_EQ( "Apple", table.read_key<std::string>( "PIE" ).value );

}
