#include "row_test.hpp"


TEST_P( ReadRowTest, ReadRow ) {
    misFITS::Row row = tobj->row();

    SCOPED_TRACE( tobj->name_ );
    test_fiducial( row  );
}
