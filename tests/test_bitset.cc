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
#include "misfits/bitset.hpp"

#include <vector>

#define STATUS_COLUMN "nstatus"
#define STATUS_BIT (37 - 1)
#define STATUS_SIZE 72


// at least as of Boost 1.60, if bitset.size() is not an integral
// multiple of the number of bits in a block, reading from blocks corrupts
// the bitset object, leading to errors when it is destroyed.
TEST( BitSet, BoostOddBitSetSize ) {

    ::testing::FLAGS_gtest_death_test_style = "threadsafe";

    typedef boost::dynamic_bitset<unsigned char> BitSet;

    EXPECT_DEATH( {

	    BitSet bits( 37 );

	    typedef std::vector<BitSet::block_type> Blocks;

	    Blocks buffer;
	    buffer.resize( bits.num_blocks(), 0xff );

	    boost::from_block_range(buffer.begin(),buffer.end(), bits );
	}, ".*Assertion.*m_check_invariants.*" );

}


// This is a high level test that we've worked around the above
// feature
TEST( BitSet,  misFITSOddBitSetSize ) {

    ::testing::FLAGS_gtest_death_test_style = "threadsafe";

    EXPECT_EXIT( 
		{
		    {
			misFITS::Table table( "EVENTS" );

			misFITS::BitSet alt_status;

			misFITS::Row w_row( table );

			table.add( STATUS_COLUMN,
				   misFITS::ColumnType::ID::Bit,
				   STATUS_BIT + 1 );

			w_row.add( STATUS_COLUMN, &alt_status );
			alt_status.resize( STATUS_SIZE );
			alt_status.set(STATUS_BIT + 1);
			w_row.write( 1 );

			alt_status.clear();

			misFITS::Row r_row( table );
			r_row.add( STATUS_COLUMN, &alt_status );

			r_row.read( 1 );

			// expect the destructor to assert() if we haven't fixed it
		    }
		    exit(0);
		},
		::testing::ExitedWithCode(0), ""
		 );

}


