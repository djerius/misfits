#include "fits.hpp"
#include "hdu.hpp"

namespace misFITS {

    const Keyword<std::string>
    KeywordIterator::current() {

	const Keyword<std::string>& kw = hdup_->read_keyn( keynum_, "" );
	return kw;

    }

    KeywordIterator::KeywordIterator( const HDUPtr& hdup, int keynum ) : hdup_(hdup), keynum_(keynum) {

	nkeys =  hdup_->get_hdrpos().first;

    }

}
