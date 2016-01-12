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

#ifndef TEST_FITS_UTIL_H
#define TEST_FITS_UTIL_H

#define squote(s) #s
#define quote(s) squote(s)
#define TEST_FITS_QFILENAME quote(TEST_FITS_FILENAME)

#include "gtest/gtest.h"
#include <misfits/fits.hpp>

void die_if( int status, const char* prefix );
void gen_fits( const std::string& = TEST_FITS_QFILENAME );

class GenFits : public ::testing::Test {

protected:
    void SetUp();
    void TearDown();
};

template< class Mode >
class FiducialTableFptr : public GenFits {

protected:

    void SetUp() {
	GenFits::SetUp();
	file = misFITS::open<misFITS::Entity::File, Mode>( TEST_FITS_QFILENAME );
    }

    void TearDown() {
	file.reset();
	GenFits::TearDown();
    }

    misFITS::FilePtr file;
};

typedef FiducialTableFptr<misFITS::Mode::ReadOnly> FiducialTableROFptr;
typedef FiducialTableFptr<misFITS::Mode::ReadWrite> FiducialTableRWFptr;

namespace misFITS_Test {


    typedef misFITS::unique_ptr<fitsfile,void(*)(fitsfile*)> TestFitsPtr;

    void closeTestFitsPtr( fitsfile* fitsptr );

    TestFitsPtr  createFitsFile( const std::string& filename );

    void
    createFitsTable( TestFitsPtr& fpp, const std::string& name, int extver = 1);

    void writeFidData( TestFitsPtr& fpp, const std::string& name, int extver = 1 );

}




#endif // ! TEST_FITS_UTIL_H
