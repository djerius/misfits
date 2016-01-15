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

namespace Entity = misFITS::Entity;
namespace Mode = misFITS::Mode;

#include "fiducial_data.hpp"

using namespace misFITS_Test;
using misFITS::ColumnType;

TEST_F( FiducialTableRWFptr, CreateFromFPtr ) {

    // add a second table with the same name as the first but
    // with a different version

    {
	misFITS::Table t2( "stuff", 2 );
	file->add( t2 );
    }

    file->move_to( "stuff", 1 );
    int hdu1 = file->hdu_num();

    file->move_to( "stuff", 2 );
    int hdu2 = file->hdu_num();

    ASSERT_NE( hdu1, hdu2 );



    {
	file->move_to( hdu1 );
	misFITS::Row row( file );
	misFITS::TablePtr table = row.table();

	ASSERT_EQ( hdu1, table->hdu_num );
	ASSERT_EQ( hdu1, file->hdu_num() );

	ASSERT_EQ( "stuff", table->extname );
	ASSERT_EQ( 1, table->extver );

    }

    {
	file->move_to( 1 );
	misFITS::Row row( file, "stuff" );
	misFITS::TablePtr table = row.table();

	ASSERT_EQ( hdu1, table->hdu_num );
	ASSERT_EQ( 1, file->hdu_num() );

	ASSERT_EQ( "stuff", table->extname );
	ASSERT_EQ( 1, table->extver );

    }

    {
	file->move_to( 1 );
	misFITS::Row row( file, "stuff", 1 );
	misFITS::TablePtr table = row.table();

	ASSERT_EQ( hdu1, table->hdu_num );
	ASSERT_EQ( 1, file->hdu_num() );

	ASSERT_EQ( "stuff", table->extname );
	ASSERT_EQ( 1, table->extver );
    }

    {
	file->move_to( 1 );
	misFITS::Row row( file, "stuff", 2 );
	misFITS::TablePtr table = row.table();

	ASSERT_EQ( hdu2, table->hdu_num );
	ASSERT_EQ( 1, file->hdu_num() );

	ASSERT_EQ( "stuff", table->extname );
	ASSERT_EQ( 2, table->extver );
    }

    {
	file->move_to( 1 );
	misFITS::Row row( file, hdu2 );
	misFITS::TablePtr table = row.table();

	ASSERT_EQ( hdu2, table->hdu_num );
	ASSERT_EQ( 1, file->hdu_num() );

	ASSERT_EQ( "stuff", table->extname );
	ASSERT_EQ( 2, table->extver );
    }


}

