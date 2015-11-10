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


TEST( FITSFileTest, DeleteColumn ) {

    gen_fits();

    {
	misFITS::FilePtr file( misFITS::open<Entity::Data,Mode::ReadWrite>( TEST_FITS_QFILENAME ));

	EXPECT_STREQ( TEST_FITS_QFILENAME, file->file.c_str());

	misFITS::Table table( file );

	ASSERT_EQ( 5, table.num_columns() );

	table.delete_column( "Ecol" );

	EXPECT_EQ( 4, table.num_columns() );
	ASSERT_EQ( 3, table.colinfo( "Dcol" ).colnum );
    }

    // make sure changes were flushed and the file was closed
    {
	misFITS::File file( TEST_FITS_QFILENAME );

	misFITS::TablePtr table( file.table() );

	EXPECT_EQ( 4, table->num_columns() );
	ASSERT_EQ( 3, table->colinfo( "Dcol" ).colnum );
    }

}

