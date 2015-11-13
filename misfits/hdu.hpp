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

// -*-c++-*-

#ifndef misFITS_HDU_H
#define misFITS_HDU_H

#include <string>
#include <own_or_observe_ptr.hpp>

#include <misfits/fits.hpp>

namespace misFITS {

    class HDU {

	friend class Row;
	friend class File;

    public:

	std::string extname;
	int hdu_num;

	own_or_observe::ptr<File> file;

	virtual ~HDU() {}

    protected:

	HDU( File& file);
	HDU( WeakFilePtr& file);
	HDU( SharedFilePtr& file);
	HDU( );
	void refresh();

    };


}


#endif // ! misFITS_HDU_H
