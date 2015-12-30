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

#ifndef ROW_TEST_H
#define ROW_TEST_H

#include "gtest/gtest.h"

#include <misfits/fits.hpp>
#include <misfits/table.hpp>
#include <misfits/row.hpp>

#include "util.hpp"
#include "row_utils.hpp"

struct generate_fits {

    generate_fits() {
	gen_fits();
    }

};

struct read_row {

    misFITS::FilePtr file_;
    std::string file_name_;
    bool init;
    std::string name_;

    read_row( const std::string& name, const std::string& file_name ) : file_name_( file_name ), init( false ), name_(name) {}
    read_row( const std::string& name, misFITS::FilePtr file ) : file_ ( file ), init( true ), name_(name) {}

    misFITS::FilePtr file() {

    	if ( ! init ) {
    	    file_ = misFITS::FilePtr( new misFITS::File( file_name_ ) );
    	    init = true;
    	}

    	return file_;
    }

    virtual ~read_row() { }
    virtual misFITS::Row row() = 0;

};

class ReadRowTest : public ::testing::TestWithParam< misFITS::shared_ptr<read_row> > {};


TEST_P( ReadRowTest, ReadRow ) {

    misFITS::Row row = GetParam()->row();

    SCOPED_TRACE( GetParam()->name_ );
    test_fiducial( row  );
}

#endif // ! ROW_TEST_H
