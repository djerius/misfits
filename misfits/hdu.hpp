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

	friend class File;
	friend class resetHDU;
	friend const Keyword<std::string> KeywordIterator::current();

    public:

	std::pair<int,int> get_hdrpos( );

	template<typename T>
	Keyword<T> get_keyword( const std::string& keyname,
				 const T& default_value = StorageCode<T>::default_value() ) const;

	template<typename T>
	void set_keyword( const Keyword<T>& kw) const;

	void delete_keyword( const std::string& keyname ) const;
	bool has_keyword( const std::string& keyname ) const;

	void add_history(  const std::string& history ) const;
	void add_comment(  const std::string& comment ) const;


	void dump_keywords( );

	void dump_keywords( std::ostream& os, int keynum = 1  );

	virtual ~HDU() {}

	HDU( WeakFilePtr& file, int hdu_num = 0 );
	HDU( WeakFilePtr& file, const std::string& extname, int extver = 1 );

	HDU( SharedFilePtr& file, int hdu_num = 0 );
	HDU( SharedFilePtr& file, const std::string& extname, int extver = 1 );

	HDU( );
	void refresh();

	SharedFilePtr file( ) const { return file_.get(); }
	const std::string& extname() const { return extname_; }
	int hdu_num() const { return hdu_num_; }
	int extver() const { return extver_; }

	void set_as_chdu() const;

    private:
	Keyword<std::string> read_keyn( int keynum, const std::string& default_value = "" ) const;


    protected:
	own_or_observe::ptr<File> file_;
	std::string extname_;
	int extver_;
	int hdu_num_;


    };


}


#endif // ! misFITS_HDU_H
