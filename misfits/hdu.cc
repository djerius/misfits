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

#include <string>

#include <misfits/hdu.hpp>
#include "fits_p.hpp"

namespace misFITS {

    HDU::HDU( WeakFilePtr& file_, int hdu_num_ ) : hdu_num( hdu_num_ ) {

	file.set<own_or_observe::observed>( file_ );
	if ( 0 == hdu_num )
	    hdu_num = file->hdu_num();

	refresh();
    }


    HDU::HDU( SharedFilePtr& file_, int hdu_num_ ) : hdu_num( hdu_num_ ) {

    	file.set<own_or_observe::observed>( file_ );
    	if ( 0 == hdu_num )
    	    hdu_num = file->hdu_num();
    	refresh();
    }

    HDU::HDU( WeakFilePtr& file_, const std::string& extname_, int extver_ ) : extname( extname_ ), extver( extver_ ) {

	file.set<own_or_observe::observed>( file_ );

	{
	    resetHDU chdu( file );
	    file->move_to( extname, extver );
	    hdu_num = file->hdu_num();
	}

	refresh();
    }

    HDU::HDU( SharedFilePtr& file_, const std::string& extname_, int extver_ ) : extname( extname_ ), extver( extver_ ) {

	file.set<own_or_observe::observed>( file_ );

	{
	    resetHDU chdu( file );
	    file->move_to( extname, extver );
	    hdu_num = file->hdu_num();
	}

	refresh();
    }

    HDU::HDU( ) {

	FilePtr fp( open<Entity::File,  Mode::Create>( "mem://" ) );
	file.set<own_or_observe::owned>( fp );

	hdu_num = 1;
    }

    void HDU::refresh() {

	resetHDU chdu( file );
	file->move_to( hdu_num );

	extname = file->read_key<std::string>( "EXTNAME", "" ).value;
	extver  = file->read_key<int>( "EXTVER", 1 ).value;
    }

}
