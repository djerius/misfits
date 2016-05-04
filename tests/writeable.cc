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

#include "fiducial_data.hpp"

namespace Entity = misFITS::Entity;
namespace Mode = misFITS::Mode;

using namespace misFITS_Test;


TEST( FITSFileTest, DeleteColumn ) {

    gen_fits();

    Fiducial::Data fid;
    fid.normalize_data();

    {
	misFITS::FilePtr file = misFITS::open<Entity::Data,Mode::ReadWrite>( TEST_FITS_QFILENAME );

	EXPECT_STREQ( TEST_FITS_QFILENAME, file->file.c_str());

	misFITS::Table table( file );

	ASSERT_EQ( fid.columns.size(), table.num_columns() );

	table.delete_column( "E1" );

	EXPECT_EQ( fid.columns.size() - 1, table.num_columns() );

	ASSERT_FALSE( table.has_column( "E1" ) );
    }

    // make sure changes were flushed and the file was closed
    {
	misFITS::FilePtr file = misFITS::open<Entity::Data,Mode::ReadOnly>( TEST_FITS_QFILENAME );

	misFITS::TablePtr table( file->table() );

	EXPECT_EQ( fid.columns.size() - 1, table->num_columns() );
	ASSERT_FALSE( table->has_column( "E1" ) );
    }

}

