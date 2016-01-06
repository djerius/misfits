#include "misfits/fits.hpp"

namespace misFITS {

    const Keyword<std::string>
    KeywordIterator::current() {

	int chdu = fp_->hdu_num();

	try {
	    fp_->movabs_hdu( hdu_num_ );
	    const Keyword<std::string>& kw = fp_->read_keyn( keynum_, "" );
	    fp_->movabs_hdu( chdu );
	    return kw;
	}
	catch (...) {
	    fp_->movabs_hdu( chdu );
	    throw;
	}
    }

    KeywordIterator::KeywordIterator( FilePtr fp, int hdu_num, int keynum ) :
	keynum_(keynum), fp_(fp), hdu_num_( hdu_num == 0 ? fp_->hdu_num() : hdu_num ) {

	int chdu = fp_->hdu_num();

	try {
	    fp_->movabs_hdu( hdu_num_ );
	    nkeys =  fp_->get_hdrpos().first;
	    fp_->movabs_hdu( chdu );
	}
	catch (...) {
	    fp_->movabs_hdu( chdu );
	    throw;
	}

    }


}
