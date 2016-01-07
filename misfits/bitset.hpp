#ifndef misFITS_BITSET_HPP
#define misFITS_BITSET_HPP

#include <boost/dynamic_bitset.hpp>

namespace misFITS {

    typedef uint8_t byte_t;

    typedef boost::dynamic_bitset<byte_t> BitSet;

    byte_t reverse_bits(byte_t);

    template<typename Iterator>
    class BitSet_output_iterator : public std::iterator <std::output_iterator_tag, byte_t> {

    protected:

	Iterator iter_;

    public:

	explicit BitSet_output_iterator( Iterator& iter ) : iter_( iter ) {};

	BitSet_output_iterator& operator=( const byte_t& value ) {
	    *iter_ = reverse_bits( value );
	    return *this;
	}

	BitSet_output_iterator& operator* ()         {               return *this; }
	BitSet_output_iterator& operator++ ()        { iter_++;      return *this; }
	BitSet_output_iterator& operator++ (int inc) { iter_+= inc ; return *this; }


    };

    template<typename Iterator>
    BitSet_output_iterator<Iterator>
    bitset_output_iterator( Iterator iter ) {
	return BitSet_output_iterator<Iterator>( iter );
    }

    template <typename Iterator>
    class BitSet_input_iterator : public std::iterator <std::input_iterator_tag, byte_t> {

    protected:

	Iterator iter_;
	byte_t cache_;

    public:

	explicit BitSet_input_iterator( Iterator& iter ) : iter_( iter ) {};

	byte_t operator* ()                         { return reverse_bits( *iter_ ); }
	BitSet_input_iterator& operator++ ()        { iter_++;      return *this; }
	BitSet_input_iterator& operator++ (int inc) { iter_+= inc ; return *this; }

	bool operator == ( const BitSet_input_iterator& other ) const {
	    return iter_ == other.iter_;
	}

	bool operator != ( const BitSet_input_iterator& other ) const {
	    return iter_ != other.iter_;
	}

    };

    template<typename Iterator>
    BitSet_input_iterator<Iterator>
    bitset_input_iterator( Iterator iter ) {
	return BitSet_input_iterator<Iterator>( iter );
    }

}

#endif // !misFITS_BITSET_HPP
