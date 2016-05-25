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

#ifndef misFITS_ROW_H
#define misFITS_ROW_H

#include <algorithm>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include <own_or_observe_ptr.hpp>
#include <misfits/types.hpp>
#include <misfits/table.hpp>

#include <misfits/row_entry.hpp>
#include <misfits/memblock.hpp>

namespace misFITS {


    /////////////
    // The Row //
    /////////////


    class Row {

    public:

	typedef std::vector< shared_ptr<RowEntry::ColumnBase> >Entries;

	bool read();
	bool read( LONGLONG row ) {
	    idx( row );
	    return read();
	}

	void write();
	void write( LONGLONG row ) {
	    idx( row );
	    write();
	}

	const ColumnInfo& colinfo( Table::Columns::size_type colnum ) { return table_->colinfo( colnum ); }
	const ColumnInfo& colinfo( const std::string& name ) { return table_->colinfo( name ); }

	template< class T >
	Row& add( const std::string& column_name, T* base ) {

	    const misFITS::ColumnInfo& ci = table_->colinfo( column_name );
	    entries.push_back( make_shared< RowEntry::Column<T> >( ci, base ) );
	    return *this;
	}

	Row& add( void* base, const Entry::MemBlock& block ) {

	    Entry::MemBlock::Entries::const_iterator entry = block.entries.begin();
	    Entry::MemBlock::Entries::const_iterator end = block.entries.end();

	    for ( ; entry < end ; ++entry ) {

		const misFITS::ColumnInfo& ci = table_->colinfo( (*entry)->name );
		entries.push_back( (*entry)->column( ci, base ) ); 
	    }

	    return *this;
	}

	/////////////////////////
        // Constructors	       //
        /////////////////////////

	Row( own_or_observe::rptr<Table>* base );
	Row( Table& table );
	Row( TablePtr& table );

	Row( FilePtr& file, int hdu_num = 0 );
	Row( FilePtr& file, const std::string& extname, int extver = 1 );

	LONGLONG idx() const { return idx_ ; }
	LONGLONG idx( LONGLONG idx ) {
	    idx_ = idx ;
	    return idx_;
	}
	LONGLONG advance( LONGLONG nrows = 1 ) {
	    idx_ += nrows;
	    return idx_;
	}

	bool auto_advance() const { return auto_advance_ ; }
	bool auto_advance( bool flag ) {
	    auto_advance_ = flag;
	    return auto_advance_;
	}

	Entries::size_type num_columns() const { return entries.size(); }

	LONGLONG num_rows() const { return table_->num_rows() ; }

	TablePtr table() const { return table_; }

    private :
	own_or_observe::ptr<Table> table_;

	void init ();
	void push_back( shared_ptr<RowEntry::ColumnBase> col ) {
	    entries.push_back( col );
	}

	LONGLONG idx_;
	bool auto_advance_;
	// if the row object is copied, don't want two objects
	// managing the same column entries
	Entries entries;
    };


}

#endif // ! misFITS_ROW_H
