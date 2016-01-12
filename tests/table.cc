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

TEST_F( FiducialTableROFptr, MetaData ) {

    EXPECT_STREQ( TEST_FITS_QFILENAME, file->file.c_str());

    file->move_to( 2 );

    misFITS::Table table( file );

    Fiducial::Data data;

    ASSERT_EQ( data.columns.size(), table.num_columns() );

    Fiducial::Data::Cols::const_iterator col( data.columns.begin() );
    Fiducial::Data::Cols::const_iterator end( data.columns.end() );

    for ( ; col < end ; ++ col ) {

	SCOPED_TRACE( (*col)->ttype );
	ASSERT_TRUE( table.exists_column( (*col)->ttype ) );
	ASSERT_EQ( (*col)->colnum, table.colinfo( (*col)->ttype ).colnum );

    }
}

TEST( TableTest, CopyHeader ) {

    misFITS::Table table( "MYEXTENT" );

    EXPECT_EQ( "MYEXTENT",  table.extname );
    EXPECT_EQ( 1,  table.extver );

    EXPECT_EQ( 0, table.num_columns() );

    table.add( "col2", ColumnType::Double, "", 1 );
    table.add( "col1", ColumnType::Double, "", 1, 1 );
    table.add( "col3", ColumnType::Double, "", 1, 0 );

    EXPECT_EQ( 3, table.num_columns() );
    EXPECT_EQ( "col1", table.colinfo(1).ttype );
    EXPECT_EQ( "col2", table.colinfo(2).ttype );
    EXPECT_EQ( "col3", table.colinfo(3).ttype );

    misFITS::ColumnInfo c3( table.colinfo(3) );
    c3.ttype = "col3a";
    table.add( c3 );
    EXPECT_EQ( 4, table.num_columns() );
    EXPECT_EQ( "col3a", table.colinfo(3).ttype );
    EXPECT_EQ( "col3", table.colinfo(4).ttype );


    misFITS::FilePtr file( misFITS::open<Entity::Memory>() );
    table.copy( file, misFITS::TableCopy::Header );

    misFITS::Table table2( file );

    EXPECT_EQ( 4, table2.num_columns() );
    EXPECT_EQ( "col1", table2.colinfo(1).ttype );
    EXPECT_EQ( "col2", table2.colinfo(2).ttype );
    EXPECT_EQ( "col3a", table2.colinfo(3).ttype );
    EXPECT_EQ( "col3", table2.colinfo(4).ttype );

    table2.add( "col4", ColumnType::Double, "", 1, 0 );

    EXPECT_EQ( 4, table.num_columns() );
    EXPECT_EQ( 5, table2.num_columns() );

}

TEST_F( FiducialTableROFptr, CopyHDU ) {

    misFITS::FilePtr file2( misFITS::open<Entity::Memory>() );

    file->move_to( "stuff" );
    misFITS::Table table( file );


    table.copy( file2, misFITS::TableCopy::HDU );

    file->close();

    file2->move_to("stuff");

    Fiducial::Data fid;

    misFITS::Table table2( file2 );


    misFITS::Row row( table2 );

    // test that we've really copied the whole HDU
    // if just the header is copied, CFITSIO will copy
    // NAXIS2, so we can't check if the number of rows
    // is correct.  It'll also happily read rows that don't
    // exist (up to the bogus NAXIS2).  So, read and compare
    // some actual data.

    int i1;
    row.column( "i1", &i1 );

    while ( row.read() )
	EXPECT_EQ( fid.i1.data[ row.idx() - 2 ], i1 );

}

