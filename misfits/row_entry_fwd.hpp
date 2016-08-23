#ifndef misFITS_ROW_ENTRY_FWD_HPP
#define misFITS_ROW_ENTRY_FWD_HPP

namespace misFITS {

    namespace RowEntry {
	template<typename T> class ColumnPointer;

	template< typename CCType,
		  ColumnType::ID::type FitsType,
		  StorageType BufferStorageType,
		  typename Buffer = std::vector< typename NativeType<BufferStorageType>::storage_type >
		  >
	class BufferedColumn;

	template<typename T> class Column;
	template<typename T, typename TV> class ColumnVector;
	template<typename T, typename TV> class BoolColumnVector;
	template<typename T> class StringColumnVector;

    }

}

#endif // ! misFITS_ROW_ENTRY_FWD_HPP
