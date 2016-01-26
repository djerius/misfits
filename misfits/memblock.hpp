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

#ifndef misFITS_MEMBLOCK_H
#define misFITS_MEMBLOCK_H

#include <cstddef>
#include <string>
#include <vector>

namespace misFITS {

    namespace Entry {

	class ColumnBase;
	class MemBlock {

	public:
	    friend class misFITS::Row;


	    struct EntryBase {
		std::string name;
		ptrdiff_t offset;

		EntryBase ( const std::string& name, ptrdiff_t offset ) : name ( name ), offset( offset ) {}
		virtual ~EntryBase() {}
		virtual shared_ptr<misFITS::Entry::ColumnBase> column( const ColumnInfo& info, void* base ) const = 0;
		virtual shared_ptr<EntryBase> clone() const = 0;

	    };

	    typedef shared_ptr<EntryBase> EntryBasePtr;
	    typedef std::vector< EntryBasePtr > Entries;


	    template<typename EntryType>
	    struct Entry : EntryBase {

		Entry ( const std::string& name, ptrdiff_t offset ) : EntryBase( name, offset ) {}

		EntryBasePtr clone() const {

		    return make_shared<Entry>( name, offset );
		}

		shared_ptr<misFITS::Entry::ColumnBase> column( const ColumnInfo& info, void* base ) const {
		    return make_shared< misFITS::Entry::Column<EntryType> >( info, reinterpret_cast<EntryType*>( reinterpret_cast<unsigned char*>(base) + EntryBase::offset ) );
		}

		~Entry() {};

	    };

	    void add ( ptrdiff_t offset, const MemBlock& block );

	protected:

	    Entries entries;
	    ptrdiff_t increment;

	public:
	    virtual ~MemBlock() {}
	    MemBlock( ptrdiff_t increment ) :increment( increment ) {};

	};


	template< class T>
	class MemBlockAddr : public MemBlock {

	    const void* base_;

	public:

	    MemBlockAddr( const T* base, ptrdiff_t increment = sizeof(T) ) : MemBlock( increment ), base_( base ) {}

	    template< typename EntryType >
	    MemBlockAddr& add ( const std::string& name, EntryType *addr ) {
		entries.push_back( make_shared< Entry<EntryType> > ( name,
								     reinterpret_cast<unsigned char*>(addr)
								     - reinterpret_cast<unsigned char*>( const_cast<void*>(base_) )
								     )
				   );
		return *this;
	    }

	    MemBlockAddr& add ( ptrdiff_t offset, const MemBlock& block ) {
		MemBlock::add( offset, block );
		return *this;
	    }


	};

	template< class T>
	class MemBlockOffset : public MemBlock {

	public:

	    MemBlockOffset( ptrdiff_t increment = sizeof(T) ) : MemBlock( increment ) {};

	    template< typename EntryType >
	    MemBlockOffset& add ( const std::string& name, ptrdiff_t offset  ) {
		entries.push_back( make_shared< Entry<EntryType> > ( name, offset) );
		return *this;
	    }

	    MemBlockOffset& add ( ptrdiff_t offset, const MemBlock& block ) {
		MemBlock::add( offset, block );
		return *this;
	    }


	};

	template<typename T>
	MemBlockOffset<T> memblock() {
	    return MemBlockOffset<T>();
	}

	template<typename T>
	MemBlockAddr<T> memblock( const T* base) {
	    return MemBlockAddr<T>( base);
	}

    }

}



#endif  // ! misFITS_MEMBLOCK_H
