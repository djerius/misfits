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
#include <iosfwd>

#include <own_or_observe_ptr.hpp>

#include <misfits/fits.hpp>

namespace misFITS {

    class HDU : public Shared<HDU> {

	friend class Row;
	friend class File;

    public:

	std::string extname;
	int extver;
	int hdu_num;

	own_or_observe::ptr<File> file;

	std::pair<int,int> get_hdrpos( );

	Keyword<std::string> read_keyword( const std::string& keyname,
					   const std::string& default_value = "") const;

	Keyword<std::string> read_keyn( int keynum, const std::string& default_value = "" ) const;

	template<typename T>
	Keyword<T> read_key( const std::string& keyname,
			     const T& default_value = StorageCode<T>::default_value() ) const;

	template<typename T>
	void write_key( const Keyword<T>& kw) const;

	template<typename T>
	void update_key( const Keyword<T>& kw) const;

	virtual ~HDU() {}

    protected:

	HDU( WeakFilePtr& file, int hdu_num = 0 );
	HDU( WeakFilePtr& file, const std::string& extname, int extver = 1 );

	HDU( SharedFilePtr& file, int hdu_num = 0 );
	HDU( SharedFilePtr& file, const std::string& extname, int extver = 1 );

	HDU( );
	void refresh();

    };

}


#endif // ! misFITS_HDU_H
