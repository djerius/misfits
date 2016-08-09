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
using namespace misFITS::ColumnType;
using misFITS::Extent;
using misFITS::ColumnInfo;
using misFITS::Keyword;

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
	misFITS::Table table( file );
	ASSERT_EQ( hdu1, table.hdu_num() );
	ASSERT_EQ( hdu1, file->hdu_num() );

	ASSERT_EQ( "stuff", table.extname() );
	ASSERT_EQ( 1, table.extver() );

    }

    {
	file->move_to( 1 );
	misFITS::Table table( file, "stuff" );
	ASSERT_EQ( hdu1, table.hdu_num() );

	ASSERT_EQ( "stuff", table.extname() );
	ASSERT_EQ( 1, table.extver() );

    }

    {
	file->move_to( 1 );
	misFITS::Table table( file, "stuff", 1 );
	ASSERT_EQ( hdu1, table.hdu_num() );

	ASSERT_EQ( "stuff", table.extname() );
	ASSERT_EQ( 1, table.extver() );
    }

    {
	file->move_to( 1 );
	misFITS::Table table( file, "stuff", 2 );
	ASSERT_EQ( hdu2, table.hdu_num() );

	ASSERT_EQ( "stuff", table.extname() );
	ASSERT_EQ( 2, table.extver() );
    }

    {
	file->move_to( 1 );
	misFITS::Table table( file, hdu2 );
	ASSERT_EQ( hdu2, table.hdu_num() );

	ASSERT_EQ( "stuff", table.extname() );
	ASSERT_EQ( 2, table.extver() );
    }


}

TEST_F( FiducialTableROFptr, MetaData ) {

    EXPECT_STREQ( TEST_FITS_QFILENAME, file->file.c_str());

    misFITS::Table table( file );

    // move file to different HDU to make sure we've isolated the
    // table from the file
    file->move_to( 1 );

    Fiducial::Data data;

    ASSERT_EQ( data.columns.size(), table.num_columns() );
    ASSERT_EQ( data.nrows, table.num_rows() );

    Fiducial::Data::Cols::const_iterator col( data.columns.begin() );
    Fiducial::Data::Cols::const_iterator end( data.columns.end() );

    for ( ; col < end ; ++ col ) {

	SCOPED_TRACE( (*col)->ttype );
	ASSERT_TRUE( table.has_column( (*col)->ttype ) );
	ASSERT_EQ( (*col)->colnum, table.colinfo( (*col)->ttype ).colnum );

    }
}

TEST( TableTest, CopyHeader ) {

    misFITS::Table table( "MYEXTENT" );

    EXPECT_EQ( "MYEXTENT",  table.extname() );
    EXPECT_EQ( 1,  table.extver() );

    EXPECT_EQ( 0, table.num_columns() );

    table.add( "col2", ID::Double, "", 1 );
    table.add( "col1", ID::Double, "", 1, 1 );
    table.add( "col3", ID::Double, "", 1, 0 );

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

    table2.add( "col4", ID::Double, "", 1, 0 );

    EXPECT_EQ( 4, table.num_columns() );
    EXPECT_EQ( 5, table2.num_columns() );

}

TEST_F( FiducialTableROFptr, CopyHDU ) {

    misFITS::FilePtr file2( misFITS::open<Entity::Memory>() );

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
    row.add( "i1", &i1 );

    while ( row.read() )
	EXPECT_EQ( fid.i1.data[ static_cast<std::size_t>(row.idx()) - 2 ], i1 );

}

TEST( TableTest, ResizeColumn ) {

    misFITS::Table table( "MYEXTENT" );

    EXPECT_EQ( "MYEXTENT",  table.extname() );
    EXPECT_EQ( 1,  table.extver() );

    EXPECT_EQ( 0, table.num_columns() );

    table.add( "col1", ID::Double, "", 1, 1 );

    // single dimension cells have no TDIM keywords
    // to ensure CIAO don't barf
    ASSERT_FALSE( table.has_keyword( "TDIM1" ) );

    Extent ex0 = table.colinfo( 1 ).extent;

    table.resize( "col1", Extent( 3, 2 ) );

    ColumnInfo c1 = table.colinfo( 1 );
    Extent ex1 = c1.extent;

    ASSERT_TRUE ( ex0 != ex1 ) << "Extents differ";

    ASSERT_EQ( 6, c1.extent.nelem() );
    ASSERT_EQ( 3, c1.extent[0] );
    ASSERT_EQ( 2, c1.extent[1] );

    ASSERT_EQ( "(3,2)", table.get_keyword<std::string>( "TDIM1" ).value );


}

TEST( TableTest, DISABLED_TableObjectSynchronization ) {

    misFITS::Table table0( "MYEXTENT" );

    misFITS::Table table1( table0.file() );

    table0.add( "col1", ID::Double );

    ASSERT_EQ( 1, table0.num_columns() );
    ASSERT_EQ( 1, table1.num_columns() );

    ColumnInfo c0 = table0.colinfo( 1 );
    ColumnInfo c1 = table1.colinfo( 1 );

    EXPECT_EQ( c0, c1 );

}

TEST( TableTest, CopyColumnNoDuplicates ) {

    misFITS::Table table0( "MyEXTENT" );

    misFITS::Table table1( "MyEXTENT" );

    table0.add( "col1", ID::Double );
    table1.add( "col1", ID::Double );

    ASSERT_THROW( { table0.copy_column( table1, "col1" ); }, misFITS::Exception::Assert );

}

TEST( TableTest, CopyColumnNoColumns ) {

    double col1;
    misFITS::Table table0( "MyEXTENT" );
    table0.add( "col1", ID::Double );
    misFITS::Row r0( table0 );
    r0.add( "col1", &col1 );
    col1 = 2;
    r0.write();
    table0.flush();

    misFITS::Table table1( "MyEXTENT" );

    // copying nothing used to trigger a CFITSIO exception
    std::vector<std::string> names;
    ASSERT_NO_THROW( table0.copy_columns( table1, names ) );
    ASSERT_EQ( 0, table1.num_rows() );
}


TEST( TableTest, CopyColumnOverWrite ) {

    misFITS::Table table0( "MyEXTENT" );

    table0.add( "col1", ID::Double );

    misFITS::Row r0( table0 );
    double col1 = 33.5;
    r0.add( "col1", &col1 );
    r0.write();
    table0.flush();

    misFITS::Table table1( "MyEXTENT" );
    table1.add( "col1", ID::Long );
    table1.add( "col2", ID::Double );
    misFITS::Row r1( table1 );
    r1.add( "col1", &col1 );

    col1 = 22;
    r1.write();
    table1.flush();

    table0.copy_column( table1, "col1", misFITS::ColumnCopy::OverWrite );

    ColumnInfo ci = table1.colinfo( "col1" );
    ASSERT_EQ( 1, ci.colnum );
    r1.read( 1 );

    ASSERT_EQ( 33, col1 );

}

TEST( TableTest, CopyColumnReplace ) {

    misFITS::Table table0( "MyEXTENT" );

    table0.add( "col1", ID::Double );

    misFITS::Row r0( table0 );
    double col1 = 33.5;
    r0.add( "col1", &col1 );
    r0.write();
    table0.flush();

    misFITS::Table table1( "MyEXTENT" );
    table1.add( "col1", ID::Long );
    table1.add( "col2", ID::Double );
    misFITS::Row r1( table1 );
    r1.add( "col1", &col1 );

    col1 = 22;
    r1.write();
    table1.flush();

    table0.copy_column( table1, "col1", misFITS::ColumnCopy::Replace );

    ColumnInfo ci = table1.colinfo( "col1" );
    ASSERT_EQ( 2, ci.colnum );

    // can't use old row, as it doesn't know about changes to table
    misFITS::Row r2( table1 );
    r2.add( "col1", &col1 );
    r2.read( );

    ASSERT_EQ( 33.5, col1 );

}

TEST( TableTest, CopyColumnExtend ) {

    misFITS::Table table0( "MyEXTENT" );

    table0.add( "col1", ID::Double );

    // source table has two rows
    misFITS::Row r0( table0 );
    double col1 = 33.5;
    r0.add( "col1", &col1 );
    r0.write();
    col1 = 44.5;
    r0.write();
    table0.flush();

    misFITS::Table table1( "MyEXTENT" );
    table1.add( "col1", ID::Long );
    table1.add( "col2", ID::Double );

    // destination table has one row
    misFITS::Row r1( table1 );
    r1.add( "col1", &col1 );
    col1 = 22;
    r1.write();

    r1.read( 1 );
    ASSERT_EQ( 22, col1 );

    table0.copy_column( table1, "col1", static_cast<misFITS::ColumnCopy::Flag>(misFITS::ColumnCopy::OverWrite  | misFITS::ColumnCopy::ExtendTable ) );

    ColumnInfo ci = table1.colinfo( "col1" );
    ASSERT_EQ( 1, ci.colnum );

    ASSERT_EQ( 2, table1.num_rows() );

    // can't use old row, as it doesn't know about changes to table
    misFITS::Row r2( table1 );
    r2.add( "col1", &col1 );

    r2.read( );
    ASSERT_EQ( 33, col1 );

    r2.read( );
    ASSERT_EQ( 44, col1 );

}

TEST( TableTest, CopyColumnExtendIfEmpty ) {

    misFITS::Table table0( "MyEXTENT" );

    table0.add( "col1", ID::Double );

    // source table has two rows
    misFITS::Row r0( table0 );
    double col1 = 33.5;
    r0.add( "col1", &col1 );
    r0.write();
    col1 = 44.5;
    r0.write();
    table0.flush();

    {
	misFITS::Table table1( "MyEXTENT" );
	table1.add( "col1", ID::Long );
	table1.add( "col2", ID::Double );

	// destination table has one row
	misFITS::Row r1( table1 );
	r1.add( "col1", &col1 );
	col1 = 22;
	r1.write();

	r1.read( 1 );
	ASSERT_EQ( 22, col1 );

	table0.copy_column( table1, "col1", static_cast<misFITS::ColumnCopy::Flag>(misFITS::ColumnCopy::OverWrite  | misFITS::ColumnCopy::ExtendTableIfEmpty ) );

	ColumnInfo ci = table1.colinfo( "col1" );
	ASSERT_EQ( 1, ci.colnum );

	ASSERT_EQ( 1, table1.num_rows() );

	// can't use old row, as it doesn't know about changes to table
	misFITS::Row r2( table1 );
	r2.add( "col1", &col1 );
    }

    {
	misFITS::Table table1( "MyEXTENT" );
	table1.add( "col1", ID::Long );
	table1.add( "col2", ID::Double );

	// destination table has no rows
	misFITS::Row r1( table1 );

	table0.copy_column( table1, "col1", static_cast<misFITS::ColumnCopy::Flag>(misFITS::ColumnCopy::OverWrite  | misFITS::ColumnCopy::ExtendTableIfEmpty ) );

	ColumnInfo ci = table1.colinfo( "col1" );
	ASSERT_EQ( 1, ci.colnum );

	ASSERT_EQ( 2, table1.num_rows() );

	// can't use old row, as it doesn't know about changes to table
	misFITS::Row r2( table1 );
	r2.add( "col1", &col1 );

	r2.read( );
	ASSERT_EQ( 33, col1 );

	r2.read( );
	ASSERT_EQ( 44, col1 );
    }

}

TEST( TableTest, CopyAllColumns ) {

    misFITS::Table table0( "MyEXTENT" );

    table0.add( "col1", ID::Double );
    table0.add( "col2", ID::Double );
    table0.add( "col3", ID::Double );

    misFITS::Table table1( "MyEXTENT" );
    table1.add( "col1", ID::Long );
    table1.add( "col2", ID::Double );

    table0.copy_columns( table1, static_cast<misFITS::ColumnCopy::Flag>(misFITS::ColumnCopy::OverWrite | misFITS::ColumnCopy::ExtendTableIfEmpty) );

    ColumnInfo ci = table1.colinfo( "col1" );
    ASSERT_EQ( 1, ci.colnum );
    ci = table1.colinfo( "col2" );
    ASSERT_EQ( 2, ci.colnum );
    ci = table1.colinfo( "col3" );
    ASSERT_EQ( 3, ci.colnum );



}

