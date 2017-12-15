# enum GDALDataType
The GDAL libary uses GDALDataType to indicate the value type of a dataset. The following types are supported by GDAL:

```cpp
enum  	GDALDataType {
  GDT_Unknown = 0, GDT_Byte = 1, GDT_UInt16 = 2, GDT_Int16 = 3,
  GDT_UInt32 = 4, GDT_Int32 = 5, GDT_Float32 = 6, GDT_Float64 = 7,
  GDT_CInt16 = 8, GDT_CInt32 = 9, GDT_CFloat32 = 10, GDT_CFloat64 = 11,
  GDT_TypeCount = 12
}
```
Of these, only the following are supported by the Blink Raster library:
- `GDT_Byte` (associated with `std::uint8_t`)
- `GDT_UInt16` (associated with `std::uint16_t`)
- `GDT_Int16` (associated with `std::int16_t`)
- `GDT_UInt32` (associated with `std::uint32_t`)
- `GDT_Int32` (associated with `std::int32_t`)
- `GDT_Float32` (associated with `float`)
- `GDT_Float64` (associated with `double`)

The data types that are not supported (yet) are associated with complex numbers.