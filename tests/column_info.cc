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

#include "misfits/columninfo.hpp"
#include "misfits/extent.hpp"

typedef misFITS::ColumnInfo ColumnInfo;
typedef misFITS::ColumnType ColumnType;
typedef misFITS::Extent     Extent;


TEST( ColumnInfo, Compare ) {

    Extent e1;
    Extent e2( 1, 2 );

    {
	ColumnInfo c1( "c1",  ColumnType::Long, "", e1 );
	ColumnInfo c2( "c2",  ColumnType::Long, "", e1 );

	EXPECT_TRUE( c1 == c2 ) << "same type, same extent";
    }

    {
	ColumnInfo c1( "c1",  ColumnType::Long, "", e1 );
	ColumnInfo c2( "c2",  ColumnType::Long, "", e2 );

	EXPECT_FALSE( c1 == c2 ) << "same type, different extent";
    }

    {
	ColumnInfo c1( "c1",  ColumnType::Short, "", e1 );
	ColumnInfo c2( "c2",  ColumnType::Long, "", e1 );

	EXPECT_FALSE( c1 == c2 ) << "different type, same extent";
    }

    {
	ColumnInfo c1( "c1",  ColumnType::Short, "", e1 );
	ColumnInfo c2( "c2",  ColumnType::Long, "", e2 );

	EXPECT_FALSE( c1 == c2 ) << "different type, different extent";
    }

}

