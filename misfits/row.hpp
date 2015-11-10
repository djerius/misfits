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

namespace misFITS {

    class Row;
    namespace Entry {

	////////////////////
        // Columns	  //
        ////////////////////

	class ColumnBase {

	    friend class misFITS::Row;

	private:
	    virtual void read( const File& file, LONGLONG firstrow ) = 0;
	    virtual void write( const File& file, LONGLONG firstrow ) = 0;

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

	    virtual void read( const File& file, LONGLONG firstrow ) {
		file.read_col( colnum_, firstrow, 1, nelem_, base_ );
	    }
	    virtual void write( const File& file, LONGLONG firstrow ) {
		file.write_col( colnum_, firstrow, 1, nelem_, base_ );
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

	    void read( const File& file, LONGLONG firstrow ) {
		file.read_col<T>( colnum_, firstrow, 1, nelem_, &(*base_)[0] );
	    }
	    void write( const File& file, LONGLONG firstrow ) {
		file.write_col<T>( colnum_, firstrow, 1, nelem_, &(*base_)[0] );
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

	    void read(  const File& file, LONGLONG firstrow );
	    void write( const File& file, LONGLONG firstrow );


	private:
	    BitSet* base_;
	    int colnum_;
	    LONGLONG nelem_;

	    BitSet::size_type nbits_;
	    BitSet::size_type nbytes_;

	    std::vector<BitSet::block_type> buffer;
	};


	template<> Column<byte_t>::Column( const ColumnInfo& info, byte_t* base );
	template<> Column< std::vector<byte_t> >::Column( const ColumnInfo& info, std::vector<byte_t>* base );


	///////////////////
        // MemBlocks	 //
        ///////////////////

	template<class ReturnClass>
	class MemBlockDSL {

	public:
	    MemBlockDSL( misFITS::Row* row, ReturnClass* caller, void* base ) : row_( row ), caller_( caller ), base_( base ) {}

	    template< class T >
	    MemBlockDSL& column( const std::string& column_name, size_t offset );
	    ReturnClass& end_memblock() { return *caller_ ; }

	    MemBlockDSL<MemBlockDSL> memblock( size_t offset ) {
		return MemBlockDSL<MemBlockDSL>( row_, this, static_cast<char*>(base_) + offset  );
	    }


	private:

	    misFITS::Row* row_;
	    ReturnClass* caller_;
	    void* base_;
	};


    }

    /////////////
    // The Row //
    /////////////


    class Row {

	template<class ReturnClass>
	friend class Entry::MemBlockDSL;

    public:

	bool read();
	void write();

	const ColumnInfo& colinfo( int colnum ) { return table->colinfo( colnum ); }
	const ColumnInfo& colinfo( const std::string& name ) { return table->colinfo( name ); }

	template< class T >
	Row& column( const std::string& column_name, T* base ) {

	    const misFITS::ColumnInfo& ci = table->colinfo( column_name );
	    entries.push_back( std::make_shared< Entry::Column<T> >( ci, base ) );
	    return *this;
	}

	Entry::MemBlockDSL<Row> memblock( void* base ) {
	    return Entry::MemBlockDSL<Row>( this, this, base );
	}

	/////////////////////////
        // Constructors	       //
        /////////////////////////

	Row( own_or_observe::rptr<Table>* base );
	Row( Table& table );
	Row( TablePtr& table );
	Row( FilePtr& file );
	Row( File& file );

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

    private :
	own_or_observe::ptr<Table> table;

	void init ();
	void push_back( std::shared_ptr<Entry::ColumnBase> col ) {
	    entries.push_back( col );
	}

	LONGLONG idx_;
	bool auto_advance_;
	// if the row object is copied, don't want two objects
	// managing the same column entries
	std::vector< std::shared_ptr<Entry::ColumnBase> > entries;
    };


    namespace Entry {

	template<class ReturnClass>
	template< class T >
	MemBlockDSL<ReturnClass>& MemBlockDSL<ReturnClass>::column( const std::string& column_name, size_t offset ) {
	    const misFITS::ColumnInfo& ci ( row_->colinfo( column_name ) );
	    row_->push_back( std::make_shared< Entry::Column<T> >( ci, reinterpret_cast<T*>(static_cast<char*>(base_) + offset )) ) ;
	    return *this;
	}
    }


}

#endif // ! misFITS_ROW_H
