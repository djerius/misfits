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


TEST_F( FiducialTableROFptr, Table ) {

    misFITS::Table table( file );

    EXPECT_EQ( "stuff",  table.extname );

    // duplicate (reopen) file
    misFITS::FilePtr rfile = file->reopen();

    // we're at the same HDU
    EXPECT_EQ( file->hdu_num(), rfile->hdu_num() );

    // move to another HDU in file
    int hdu_num = file->hdu_num();
    file->move_to( 1 );

    // we'd better not be at the last one, it should have been a
    // table.
    ASSERT_NE( hdu_num, file->hdu_num() );

    // we are were we think we are
    EXPECT_EQ( 1, file->hdu_num() );

    // rfile hasn't been touched.
    EXPECT_EQ( hdu_num, rfile->hdu_num() );

}

