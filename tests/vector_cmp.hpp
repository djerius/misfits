#ifndef VECTOR_CMP_HPP
#define VECTOR_CMP_HPP

#include <vector>
#include <boost/container/vector.hpp>

// put these in Google Test's namespace so it can find them


namespace testing {
    namespace internal {

	template< typename T>
	bool operator== ( const boost::container::vector<T>& v1, const std::vector<T>& v2  ) {

	    if ( v1.size() != v2.size() )
		return false;

	    typename boost::container::vector<T>::const_iterator v1p = v1.begin();
	    typename boost::container::vector<T>::const_iterator end = v1.end();
	    typename std::vector<T>::const_iterator v2p = v2.begin();

	    for ( ; v1p != end ; ++v1p, ++v2p )
		if ( *v1p != *v2p ) return false;

	    return true;
	}

	template< typename T>
	bool operator== ( const std::vector<T>& v2, const boost::container::vector<T>& v1  ) {

	    return operator==( v1, v2 );

	}

	template< typename T>
	bool operator!= ( const boost::container::vector<T>& v1, const std::vector<T>& v2  ) {

	    return ! operator==( v1, v2 );

	}

	template< typename T>
	bool operator!= ( const std::vector<T>& v2, const boost::container::vector<T>& v1  ) {

	    return ! operator==( v1, v2 );

	}
    }
}

template< typename T>
bool operator== ( const boost::container::vector<T>& v1, const std::vector<T>& v2  ) {

    if ( v1.size() != v2.size() )
	return false;

    typename boost::container::vector<T>::const_iterator v1p = v1.begin();
    typename boost::container::vector<T>::const_iterator end = v1.end();
    typename std::vector<T>::const_iterator v2p = v2.begin();

    for ( ; v1p != end ; ++v1p, ++v2p )
	if ( *v1p != *v2p ) return false;

    return true;
}

template< typename T>
bool operator== ( const std::vector<T>& v2, const boost::container::vector<T>& v1  ) {

    return operator==( v1, v2 );

}

template< typename T>
bool operator!= ( const boost::container::vector<T>& v1, const std::vector<T>& v2  ) {

    return ! operator==( v1, v2 );

}

template< typename T>
bool operator!= ( const std::vector<T>& v2, const boost::container::vector<T>& v1  ) {

    return ! operator==( v1, v2 );

}




#endif // ! VECTOR_CMP_HPP
