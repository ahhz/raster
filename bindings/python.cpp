#include <pybind11/pybind11.h>
#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/nodata_transform.h>
#include <pronto/raster/indicator/edge_density.h>
#include <pronto/raster/indicator/area_weighted_patch_size.h>


namespace py = pybind11;
namespace pr = pronto::raster;

enum class metric
{
    area_weighted_patch_size,
    edge_density
};

enum class window_shape
{
    square,
    circle
};

void moving_window_pattern(const std::string& input, const std::string& output, double radius, metric m, window_shape w)
{
    double nodata_value = -9999;
    auto in = pr::open_forward_only<int> (input, pr::access::read_only, 1);
    auto out = pr::create_from_model<double>(pr::filesystem::path(output), in);
    out.set_nodata_value(true, nodata_value);

    switch (w) {
    case window_shape::square: 
    {
        switch (m) {
        case metric::edge_density:
        {
            auto mw = pr::moving_window_indicator(in, pr::edge_square(static_cast<int>(radius)), pr::edge_density_generator<int>{});
            pr::assign(out, pr::optional_to_nodata(mw, nodata_value));
            break;
        }
        case metric::area_weighted_patch_size:
        {
            auto mw = pr::moving_window_indicator(in, pr::patch_square(static_cast<int>(radius)),
                pr::area_weighted_patch_size_generator{}, pr::queen_contiguity{});
            pr::assign(out, pr::optional_to_nodata(mw, nodata_value));
            break;
        }
        }

        break;
    }
    case window_shape::circle: {
        switch (m) {
        case metric::edge_density: 
        {
            auto mw = pr::moving_window_indicator(in, pr::edge_circle(radius), pr::edge_density_generator<int>{});
            pr::assign(out, pr::optional_to_nodata(mw, nodata_value));
            break;
        }
        case metric::area_weighted_patch_size: 
        {
            auto mw = pr::moving_window_indicator(in, pr::patch_circle(radius)
                , pr::area_weighted_patch_size_generator{}
            , pr::queen_contiguity{});
            pr::assign(out, pr::optional_to_nodata(mw, nodata_value));
            break;
        }
        }
        break;
    }
    }
}


PYBIND11_MODULE(example, m) {
    m.doc() = "Bindings for pronto raster moving window analysis"; 

    py::enum_<metric>(m, "Metric")
        .value("AreaWeightedPatchSize", metric::area_weighted_patch_size)
        .value("EdgeDensity", metric::edge_density);

    py::enum_<window_shape>(m, "WindowShape")
        .value("Circle", window_shape::circle)
        .value("Square", window_shape::square);

    m.def("moving_window_pattern", &moving_window_pattern, "Applies a moving window transform");
}