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

#include "util.hpp"
#include "fiducial_data.hpp"

using namespace misFITS_Test;
using misFITS::ColumnType;

TEST_F( FiducialTableFptr, CopyALL ) {

    misFITS::FilePtr output( misFITS::open<misFITS::Entity::Memory>() );

    ASSERT_EQ( 2, file->num_hdus() );

    file->copy( output, misFITS::FileCopy::AllHDUs );

    ASSERT_EQ( 2, file->num_hdus() );

    ASSERT_EQ( file->num_hdus(), output->num_hdus() );

}
