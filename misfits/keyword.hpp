#ifndef misFITS_KEYWORD_H
#define misFITS_KEYWORD_H

#include <iterator>

#include <misfits/fits.hpp>

namespace misFITS {

    // simple thing to return a keyword
    template<typename T>
    struct Keyword {
	std::string keyname;
	T value;
	std::string comment;
	bool defined;

	Keyword( const std::string& keyname, const T& value, const std::string& comment = "", bool defined = true) :
	    keyname(keyname), value(value), comment(comment ), defined( defined ) { }
    };


    class KeywordIterator : public std::iterator <std::forward_iterator_tag, Keyword<std::string>, void >{

    private:
	HDUPtr hdup_;
	int keynum_;
	int nkeys;
	const Keyword<std::string> current();

    public:
	explicit KeywordIterator( const HDUPtr& hdup_, int keynum = 1 );

	const Keyword<std::string> operator* () { return current(); }

	const Keyword<std::string> operator++ () {
	    ++keynum_;
	    return current();
	}

	const Keyword<std::string> operator++ ( int inc ) {
	    keynum_ += inc;
	    return current();
	}

	KeywordIterator end() const {
	    return KeywordIterator( hdup_, nkeys + 1 ) ;
	}

	bool operator==( const KeywordIterator& other ) const {
	    return other.hdup_ 	  == hdup_
		&& other.keynum_  == keynum_;
	}

	bool operator!=( const KeywordIterator& other ) const {
	    return ! ( (*this) == other );
	}

    };

}

#endif // ! misFITS_KEYWORD_H
