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

#include <misfits/memblock.hpp>

namespace misFITS {

    class Row;

    namespace Entry {

	////////////////////
        // Columns	  //
        ////////////////////

	class ColumnBase {

	    friend class misFITS::Row;

	private:
	    virtual void read( const Table& table, LONGLONG firstrow ) = 0;
	    virtual void write( const Table& table, LONGLONG firstrow ) = 0;

	protected:
	    virtual ~ColumnBase() {}
	};

	template< typename T >
	class Column : public ColumnBase {

	public:
	    Column( const ColumnInfo& info, T* base ) : base_( base ),
						  colnum_( info.colnum ),
						  nelem_( info.nelem() ) {}
	    virtual ~Column() { };

	    virtual void read( const Table& table, LONGLONG firstrow ) {
		table.read_col( colnum_, firstrow, 1, nelem_, base_ );
	    }
	    virtual void write( const Table& table, LONGLONG firstrow ) {
		table.write_col( colnum_, firstrow, 1, nelem_, base_ );
	    }

	protected:
	    T* base_;
	    int colnum_;
	    LONGLONG nelem_;
	};

	template< typename T>
	class Column< std::vector<T> > : public ColumnBase {

	public:
	    Column( const ColumnInfo& info, std::vector<T>* base ) : base_( base ),
							       colnum_( info.colnum ),
							       nelem_( info.nelem() ) {
		base_->resize( nelem_ );
	    }

	    void read( const Table& table, LONGLONG firstrow ) {
		table.read_col<T>( colnum_, firstrow, 1, nelem_, &(*base_)[0] );
	    }
	    void write( const Table& table, LONGLONG firstrow ) {
		table.write_col<T>( colnum_, firstrow, 1, nelem_, &(*base_)[0] );
	    }

	private:
	    std::vector<T>* base_;
	    int colnum_;
	    LONGLONG nelem_;
	};

	template<>
	class Column< BitSet > : public ColumnBase {

	public:

	    Column( const ColumnInfo& info, BitSet* base );
	    virtual ~Column() { };

	    void read(  const Table& table, LONGLONG firstrow );
	    void write( const Table& table, LONGLONG firstrow );


	private:
	    BitSet* base_;
	    int colnum_;
	    LONGLONG nbits_;
	    BitSet::size_type max_bits_;

	    // number of bytes required to store the bits. cached for
	    // speed
	    BitSet::size_type nbytes_;

	    std::vector<BitSet::block_type> buffer;
	};

	template<>
	class Column< bool > : public ColumnBase {

	public:
	    Column( const ColumnInfo& info, bool* base ) :
		base_( base ),
		colnum_( info.colnum ),
		nelem_( info.nelem() ) {

		if ( sizeof(bool) != sizeof( NativeType<SC_BYTE>::storage_type ) )
		    buffer.resize( nelem_ );
	    }
	    virtual ~Column() { };

	    virtual void read( const Table& table, LONGLONG firstrow );
	    virtual void write( const Table& table, LONGLONG firstrow );

	protected:
	    bool* base_;
	    int colnum_;
	    LONGLONG nelem_;
	    std::vector<NativeType<SC_BYTE>::storage_type> buffer;
	};

	template<>
	class Column< std::vector<bool> > : public ColumnBase {

	public:
	    Column( const ColumnInfo& info, std::vector<bool>* base ) : base_( base ),
									colnum_( info.colnum ),
									nelem_( info.nelem() ) {
		base->resize( nelem_ );
		buffer.resize( nelem_ );
	    }

	    void read( const Table& table, LONGLONG firstrow );
	    void write( const Table& table, LONGLONG firstrow );

	private:
	    std::vector<bool>* base_;
	    int colnum_;
	    LONGLONG nelem_;
	    std::vector<NativeType<SC_BYTE>::storage_type> buffer;
	};


	template<>
	class Column<std::string>: public ColumnBase {

	public:

	    Column( const ColumnInfo& info, std::string* base );
	    virtual ~Column() { };

	    void read(  const Table& table, LONGLONG firstrow );
	    void write( const Table& table, LONGLONG firstrow );

	private:

	    // for compatibility with C++ < 11, use intermediate
	    // buffer.  in C++11, consecutive characters in
	    // std::string are defined to be contiguous, and we can
	    // read directly into the string. see stackoverflow.com/questions/25169915
	    std::vector<char> buffer;

	    std::string* base_;
	    int colnum_;
	    LONGLONG nelem_;
	    LONGLONG offset;
	    LONGLONG nbytes;

	};

	template<>
	class Column< std::vector<std::string> >: public ColumnBase {

	public:

	    Column( const ColumnInfo& info, std::vector<std::string>* base );
	    virtual ~Column() { };

	    void read(  const Table& table, LONGLONG firstrow );
	    void write( const Table& table, LONGLONG firstrow );


	private:
	    std::vector<std::string>* base_;

	    // for compatibility with C++ < 11, use intermediate
	    // buffer.  in C++11, consecutive characters in
	    // std::string are defined to be contiguous, and we can
	    // read directly into the string. see stackoverflow.com/questions/25169915

	    std::vector<char> buffer;
	    int colnum_;
	    LONGLONG nelem_;
	    LONGLONG offset;
	    LONGLONG nbytes;
	    LONGLONG width;

	};


	template<> Column<byte_t>::Column( const ColumnInfo& info, byte_t* base );
	template<> Column< std::vector<byte_t> >::Column( const ColumnInfo& info, std::vector<byte_t>* base );

	///////////////////
        // MemBlocks	 //
        ///////////////////


    }

    /////////////
    // The Row //
    /////////////


    class Row {

    public:

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

	const ColumnInfo& colinfo( int colnum ) { return table_->colinfo( colnum ); }
	const ColumnInfo& colinfo( const std::string& name ) { return table_->colinfo( name ); }

	template< class T >
	Row& add( const std::string& column_name, T* base ) {

	    const misFITS::ColumnInfo& ci = table_->colinfo( column_name );
	    entries.push_back( make_shared< Entry::Column<T> >( ci, base ) );
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

	int num_columns() const { return entries.size(); }

	LONGLONG num_rows() const { return table_->num_rows() ; }

	TablePtr table() const { return table_; }

    private :
	own_or_observe::ptr<Table> table_;

	void init ();
	void push_back( shared_ptr<Entry::ColumnBase> col ) {
	    entries.push_back( col );
	}

	LONGLONG idx_;
	bool auto_advance_;
	// if the row object is copied, don't want two objects
	// managing the same column entries
	std::vector< shared_ptr<Entry::ColumnBase> > entries;
    };


}

#endif // ! misFITS_ROW_H
