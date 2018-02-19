# Raster
- [Rationale](./rationale.md)
- [Installation and Configuration](./installation_configuration.md)

## Examples
1. [Create a raster and fill with values](./examples/example_1.md)
2. [Open a raster and fill with values](./examples/example_2.md)
3. [Sum two rasters cell-by-cell](./examples/example_3.md)
4. [Multiply raster by constant](./examples/example_4.md)
5. [Apply a custom function on a four-cell moving window ](./examples/example_5.md)
6. [Calculate statistics for a circular moving window](./examples/example_6.md)
7. [Calculate statistics for a square moving window](./examples/example_7.md)
8. [Use the optional value type for nodata values](./examples/example_8.md)
9. [Iterate over square windows centered on each cell](./examples/example_9.md)
10. [Create a raster of random values based on a probability distribution](./examples/example_10.md)
11. [Apply a function to a subsection of a raster](./examples/example_11.md)

## Concepts
- [Raster](./concepts/raster.md)
- [Raster View](./concepts/raster_view.md)
- [Raster Iterator](./concepts/raster_iterator.md)
- [Random Accessible Raster Iterator](./concepts/random_accessible_raster_iterator.md)
- [Proxy Reference](./concepts/proxy_reference.md)
- [Indicator](./concepts/indicator.md)
- [Indicator Generator](./concepts/indicator_generator.md)

## Functions

### Opening and creating raster files
- [create](./functions/create.md)
- [create_from_model](./functions/create_from_model.md)
- [create_temp](./functions/create_temp.md)
- [create_temp_from_model](./functions/create_temp_from_model.md)
- [open](./functions/open.md)
- [make_gdalrasterband_view](./functions/make_gdalrasterband_view.md)

### Copying values between rasters
- [assign](./functions/assign.md)

### Creating raster views for existing data
- [h_edge](./functions/h_edge.md)
- [v_edge](./functions/v_edge.md)
- [offset](./functions/offset.md)
- [pad](./functions/pad.md)
- [transform](./functions/transform.md)

### Creating views to work with nodata values
- [optionalize](./functions/optionalize.md)
- [nodata_to_optional](./functions/nodata_to_optional.md)
- [optional_to_nodata](./functions/optional_to_nodata.md)
- [optionalize_function](./functions/optionalize_function.md)

### Creating raster views for moving window analysis
- [moving_window_indicator](./functions/moving_window_indicator.md)
- [make_subraster_window_view]

### Creating data-less raster views
- [uniform](./functions/uniform.md)
- [random_distribution_raster](./functions/random_distribution_raster.md)

### Type erasure
- [make_any_blind_raster](./functions/make_any_blind_raster.md)
- [make_any_raster](./functions/make_any_raster.md)
- [open_any](./functions/open_any.md)
- [blind_function](./functions/blind_function.md)

### Raster algebra support
- [raster_algebra_wrap](./functions/raster_algebra_wrap.md)
- [raster_algebra_transform](./functions/raster_algebra_transform.md)

### Displaying raster values
- [plot_raster](./functions/plot_raster.md)


## Types
- [access(enum)](./types/access.md)
- [GDALDataType(enum)](./types/gdal_data_type.md)
- [any_raster](./types/any_raster.md)
- [any_blind_raster](./types/any_blind_raster.md)
- [raster_algebra_wrapper](./types/raster_algebra_wrapper.md)
- [optional](./types/optional.md)
- [filesystem::path](./types/path.md)
- [gdal_raster_view](./types/gdal_raster_view.md)
- [padded_raster_view](./types/padded_raster_view.md)
- [pair_raster_view](./types/pair_raster_view.md)
- [tuple_raster_view](./types/tuple_raster_view.md)
- [transform_raster_view](./types/transform_raster_view.md)
- [uniform_raster_view](./types/uniform_raster_view.md)
- [random_raster_view]
- [circular_window_view]
- [circular_edge_window_view]
- [square_window_view]
- [square_edge_window_view]











