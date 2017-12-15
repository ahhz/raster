# optional<T>
Depending on the [configuration](./../installation_configuration.md) of the project 'blink::raster::optional<T>' is a typedef for either 'boost::optional<T>' or 'std::optional<T>' (default). In the Blink Raster library the `optional<T>` is used as the value_type for Rasters that normally have a value of type 'T' but are optionally masked out as 'nodata' values.
