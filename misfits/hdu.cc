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

namespace misFITS {

    HDU::HDU( WeakFilePtr& file) {

	RefManager::Base<File>* rm = new RefManager::Type<File,RefManager::Observer>( file );
	rm_.reset( rm );
	hdu_num = (*rm_)()->hdu_num();

	refresh();
    }

    HDU::HDU( File& file ) {

	RefManager::Base<File>* rm = new RefManager::Type<File,RefManager::Bare>( &file );
	rm_.reset( rm );
	hdu_num = (*rm_)()->hdu_num();

	refresh();
    }

    HDU::HDU( const std::string& extname) {

	RefManager::Base<File>* rm = new RefManager::Type<File,RefManager::Owner>
	    ( open<Entity::File,
			     Mode::Create>( "mem://" )
	      );

	rm_.reset( rm );
	misFITS_CHECK_CFITSIO_EXPR( fits_create_tbl( file()->fptr(),
						     BINARY_TBL,
						     0, 0, NULL, NULL, NULL,
						     extname.c_str(), &status  ) );
	hdu_num = (*rm)()->hdu_num();

	refresh();
    }

    void HDU::refresh() {

	extname = file()->read_key<std::string>( "EXTNAME").value;

    }


}
