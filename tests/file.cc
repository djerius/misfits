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
using namespace misFITS;

TEST_F( FiducialTableRWFptr, CopyALL ) {

    // create an odd number of HDU's in file, move file to HDU #2,
    // and copy all (make sure both previous and following HDU's are copied

    misFITS::Table table( "extra" );

    table.copy( file, misFITS::TableCopy::HDU );

    ASSERT_EQ( 3, file->num_hdus() );

    file->move_to( 2 );

    misFITS::FilePtr output( misFITS::open<misFITS::Entity::Memory>() );

    file->copy( output, misFITS::FileCopy::AllHDUs );

    ASSERT_EQ( 3, file->num_hdus() );
    ASSERT_EQ( file->num_hdus(), output->num_hdus() );

}

TEST_F( FiducialTableRWFptr, CopyPrevious ) {

    misFITS::FilePtr output( misFITS::open<misFITS::Entity::Memory>() );

    ASSERT_EQ( 2, file->num_hdus() );

    // create an odd number of HDU's in file, move file to HDU #2,
    // and copy previous (make sure only previous HDU's are copied)

    misFITS::Table table( "extra" );
    table.copy( file, misFITS::TableCopy::HDU );

    file->copy( output, misFITS::FileCopy::PreviousHDUs );

    ASSERT_EQ( 3, file->num_hdus() );

    ASSERT_EQ( 2, output->num_hdus() );

}

TEST_F( FiducialTableROFptr, CopyFollowing ) {

    misFITS::FilePtr output( misFITS::open<misFITS::Entity::Memory>() );

    ASSERT_EQ( 2, file->num_hdus() );

    file->move_to( file->num_hdus() );
    file->copy( output, misFITS::FileCopy::PreviousHDUs );

    ASSERT_EQ( 2, file->num_hdus() );

    ASSERT_EQ( 1, output->num_hdus() );

}

TEST_F( FiducialTableRWFptr, AddTable ) {

    misFITS::Table  events( "EVENTS" );

    int nhdus = file->num_hdus();

    misFITS::TablePtr table = file->add( events );

    ASSERT_EQ( 2, events.file->num_hdus() );

    ASSERT_EQ( nhdus + 1, file->num_hdus() );

}

template <typename T>
struct HDUTableCreateTest : public FiducialTableRWFptr {

    T create( int hdu_num = 0 );
    T create( const std::string& extname, int extver = 1 );

};

template<>
misFITS::TablePtr
HDUTableCreateTest<misFITS::TablePtr>::create( int hdu_num ) {
	return misFITS::TablePtr( this->file->table( hdu_num ) );
}

template<>
misFITS::TablePtr
HDUTableCreateTest<misFITS::TablePtr>::create( const std::string& extname, int extver ) {
    return misFITS::TablePtr( this->file->table( extname, extver ) );
};

template<>
misFITS::HDUPtr
HDUTableCreateTest<misFITS::HDUPtr>::create( int hdu_num  ) {
	return misFITS::HDUPtr( this->file->table( hdu_num ) );
}

template<>
misFITS::HDUPtr
HDUTableCreateTest<misFITS::HDUPtr>::create( const std::string& extname, int extver  ) {
    return misFITS::HDUPtr( this->file->table( extname, extver ) );
};



typedef ::testing::Types<misFITS::TablePtr, misFITS::HDUPtr> HDUTableCreateTypes;
TYPED_TEST_CASE( HDUTableCreateTest, HDUTableCreateTypes );

TYPED_TEST( HDUTableCreateTest, ReturnTable ) {

    // add a second table with the same name as the first but
    // with a different version

    FilePtr file = this->file;

    {
	Table t2( "stuff", 2 );
	file->add( t2 );
    }

    file->move_to( "stuff", 1 );
    int hdu1 = file->hdu_num();

    file->move_to( "stuff", 2 );
    int hdu2 = file->hdu_num();

    ASSERT_NE( hdu1, hdu2 );

    // move to the first HDU, it's not a table
    file->move_to( 1 );

    {
	TypeParam tp = this->create( hdu1 );

	// make sure file is back at its original hdu
	ASSERT_EQ( 1, file->hdu_num() );

	// make sure we picked up the correct table
	ASSERT_EQ( "stuff", tp->extname );
	ASSERT_EQ( hdu1, tp->hdu_num );
	ASSERT_EQ( 1, tp->extver );
    }

    // now try with the second table
    {
	TypeParam tp = this->create( hdu2 );

	// make sure file is back at its original hdu
	ASSERT_EQ( 1, file->hdu_num() );

	// make sure we picked up the correct table
	ASSERT_EQ( "stuff", tp->extname );
	ASSERT_EQ( hdu2, tp->hdu_num );
	ASSERT_EQ( 2, tp->extver );
    }

    // now with extension names and versions
    {
	TypeParam tp = this->create( "stuff", 1 );

	// make sure file is back at its original hdu
	ASSERT_EQ( 1, file->hdu_num() );

	// make sure we picked up the correct table
	ASSERT_EQ( "stuff", tp->extname );
	ASSERT_EQ( hdu1, tp->hdu_num );
	ASSERT_EQ( 1, tp->extver );
    }
    // now try with the second table
    {
	TypeParam tp = this->create( "stuff", 2 );

	// make sure file is back at its original hdu
	ASSERT_EQ( 1, file->hdu_num() );

	// make sure we picked up the correct table
	ASSERT_EQ( "stuff", tp->extname );
	ASSERT_EQ( hdu2, tp->hdu_num );
	ASSERT_EQ( 2, tp->extver );
    }
}
