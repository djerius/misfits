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

#include <misfits/fits.hpp>

#include "fiducial_data.hpp"

using namespace std;

namespace misFITS_Test {


    namespace Fiducial {


	const int Data::nrows = 20;
	const int Data::nbits = 23;
	int Data::nbytes = 0;

	std::string Data::Xform() {
	    ostringstream os;
	    os << nbits << 'X';
	    return os.str();
	}

	template< typename T >
	std::vector<T>
	fill( int start, typename std::vector<T>::size_type nelem ) {

	    std::vector<T> v;
	    v.resize(nelem);
	    for ( typename std::vector<T>::size_type i = 0 ; i < nelem ; i++ )
		v[i] = static_cast<T>(start + i - 1);

	    return v;
	}


	Data::Data () :
	    i1( "I1" , "I"    	      	      ),
	    iv1( "IV1", "10I"		      ),

	    j1( "J1" , "J"    	      	      ),
	    jv1( "JV1" , "10J"    	      ),

	    e1( "E1" , "E"    	      	      ),
	    ev1( "EV1" , "10E"    	      ),

	    d1( "D1" , "D"    	      	      ),
	    dv1( "DV1" , "10D"    	      ),

	    l1( "L1" , "L"    	      	      ),
	    lv1( "LV1" , "10L"    	      ),

	    x1( "X1" , Xform() 	      	      ),
	    x2( "X2" , Xform() 	      	      ),

	    a1( "A1" , "60A"                  ),
	    a2( "A2" , "60A12"  	      ),
	    a3( "A3" , "60A12", "(12,5)"      ),

	    // these will have embedded NUL characters
	    a4( "A4" , "60A"                  ),
	    a5( "A5" , "60A12"  	      ),
	    a6( "A6" , "60A12", "(12,5)"      )
	{

	    columns.push_back( &i1 );
	    columns.push_back( &iv1 );

	    columns.push_back( &j1 );
	    columns.push_back( &jv1 );

	    columns.push_back( &e1 );
	    columns.push_back( &ev1 );

	    columns.push_back( &d1 );
	    columns.push_back( &dv1 );

	    columns.push_back( &l1 );
	    columns.push_back( &lv1 );

	    columns.push_back( &x1 );

	    columns.push_back( &a1 );
	    columns.push_back( &a2 );
	    columns.push_back( &a3 );
	    columns.push_back( &a4 );
	    columns.push_back( &a5 );
	    columns.push_back( &a6 );

	    {
		Cols::iterator col  = columns.begin();
		Cols::iterator end  = columns.end();

		for( int col_num = 1 ; col < end ; ++col, ++col_num )
		    (*col)->colnum = col_num;
	    }

	    nbytes = nbits / 8;
	    if ( nbytes * 8 < nbits ) nbytes++;

	    enum {
		STRLEN=12,
		NSTR=5,
		BUFLEN=STRLEN*NSTR
	    };


	    for ( int i = 1 ; i <= nrows ; i++ ) {

		d1.push_back( 1.0 / i );
		dv1.push_back( fill<double>(i + 1, 10 ) );

		e1.push_back( 2.0f / i );
		ev1.push_back( fill<float>(i + 11, 10 ) );

		i1.push_back( i + 1 );
		iv1.push_back( fill<short>(i + 21, 10 ) );

		j1.push_back( i + 2 );
		jv1.push_back( fill<int>(i + 31, 10 ) );

		l1.push_back( true );
		std::vector<bool> b1(10);
		b1[0] = b1[2] = b1[4] = b1[6] = b1[8] = true;
		b1[1] = b1[3] = b1[5] = b1[7] = b1[9] = true;
		lv1.push_back( b1 );

		x1.data.resize(i);
		x1.data[i-1].resize(nbytes);

		// set the bit s.t. i == 1 => MSB, i == NROWS => LSB
		// this is left justified in the bit array, as specified
		// by the FITS standard.

		int byte = (i - 1) / 8;
		int shift = ( 8 - (i - byte * 8) );
		x1.data[i-1][byte] |= 1<<shift;

		x2.data.resize(i);
		x2.data[i-1].resize( nbits );
		x2.data[i-1][i-1] = true;

		// right justify with leading zeroes
		{
		    ostringstream os;
		    os.setf( std::ios::right, std::ios::adjustfield );
		    os.fill( '0' );

		    for ( int i = 0; i < NSTR ; ++i )
			os << std::setw(STRLEN) << i + i;

		    string A = os.str();

		    a1.push_back( A );

		    vector<string> A2;
		    vector<string> A3;

		    for ( int i = 0 ; i < BUFLEN ; i += STRLEN ) {
			A2.push_back( A.substr( i, STRLEN ) );
			A3.push_back( A.substr( i, STRLEN ) );
		    }

		    a2.push_back( A2 );
		    a3.push_back( A3 );

		}

		// left justify; fill with NULL's.
		{
		    ostringstream os;
		    os.setf( std::ios::left, std::ios::adjustfield );
		    os.fill( '\0' );

		    for ( int i = 0; i < NSTR ; ++i )
			os << std::setw(STRLEN) << i + i;

		    string A = os.str();

		    a4.push_back( A );

		    vector<string> A5;
		    vector<string> A6;

		    for ( int i = 0 ; i < BUFLEN ; i += STRLEN ) {
			A5.push_back( A.substr( i , STRLEN ) );
			A6.push_back( A5.back() );
		    }
		    a5.push_back( A5 );
		    a6.push_back( A6 );
		}
	    }

	}


	void
	Data::insert_columns( TestFitsPtr&fpp ) {

	    Cols::iterator col  = columns.begin();
	    Cols::iterator end  = columns.end();

	    long offset = 1;
	    for( ; col < end ; ++col ) {
		(*col)->insert( fpp, offset );
		offset += (*col)->nbytes;
	    }

	}

	void
	Data::write( TestFitsPtr &fpp ) const {

	    Cols::const_iterator col  = columns.begin();
	    Cols::const_iterator end  = columns.end();

	    for( col = columns.begin() ; col < end ;  ++col )
		(*col)->write( fpp );
	}

	void
	Data::normalize_data( ) {

	    Cols::iterator col  = columns.begin();
	    Cols::iterator end  = columns.end();

	    for( col = columns.begin() ; col < end ;  ++col )
		(*col)->normalize( );
	}

    }
}
