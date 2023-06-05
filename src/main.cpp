#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <unordered_map>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

struct color_t
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
    color_t() : r(0), g(0), b(0), a(0) {}
    color_t(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : r(r), g(g), b(b), a(a) {}
};

template <typename T>
struct colormap_t {
    T value;
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

namespace py = pybind11;

template <typename T>
py::array_t<color_t> map_categorical_colors(const py::array_t<T>& input, const py::array_t<colormap_t<T>>& colormap, const color_t& nan_color = color_t(0, 0, 0, 0)) {

    // Create a map from the colormap to speed up the lookup
    std::unordered_map<T, color_t> colormap_map;
    auto colormap_buf = colormap.request();
    if (colormap_buf.ndim != 1) {
        throw std::runtime_error("Number of dimensions for `colormap` must be one");
    }

    auto colormap_ptr = static_cast<colormap_t<T>*>(colormap_buf.ptr);
    for (size_t idx = 0; idx < colormap_buf.shape[0]; ++idx) {
        const auto& item = colormap_ptr[idx];
        colormap_map[colormap_ptr[idx].value] = color_t(item.r, item.g, item.b, item.a);
    }

    // now to the mapping.
    auto buf = input.request();
    if (buf.ndim != 1) {
        throw std::runtime_error("Number of dimensions for `input` must be one");
    }

    auto ptr = static_cast<T*>(buf.ptr);
    auto result = py::array_t<color_t>(buf.size);
    auto result_buf = result.request();
    auto result_ptr = static_cast<color_t*>(result_buf.ptr);
    for (size_t idx = 0; idx < buf.shape[0]; idx++) {
        auto it = colormap_map.find(ptr[idx]);
        result_ptr[idx] = it != colormap_map.end() ? it->second : nan_color;
    }
    return result;
}

#define TS_OVERLOAD(_type) \
    [](const py::array_t<_type>& input, const py::array_t<colormap_t<_type>>& colormap) { \
        py::print("type: ", #_type); \
        return map_categorical_colors(input, colormap); \
    }

#define TS(_type) \
    m.def("map_categorical_colors", TS_OVERLOAD(_type), py::arg().noconvert(), py::arg().noconvert(false), R"pbdoc(     \
        Map numpy scalar array to colors using a colormap \
    )pbdoc");

PYBIND11_MODULE(color_mappyer, m) {
    PYBIND11_NUMPY_DTYPE(color_t, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::uint8_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::uint16_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::uint32_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::uint64_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::int8_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::int16_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::int32_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<std::int64_t>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<float>, value, r, g, b, a);
    PYBIND11_NUMPY_DTYPE(colormap_t<double>, value, r, g, b, a);

    m.doc() = R"pbdoc(
        Color Mapper
        -----------------------

        .. currentmodule:: color_mappyer

        .. autosummary::
           :toctree: _generate

           map_categorical_colors
    )pbdoc";

    TS(std::uint8_t);
    TS(std::uint16_t);
    TS(std::uint32_t);
    TS(std::uint64_t);
    TS(std::int8_t);
    TS(std::int16_t);
    TS(std::int32_t);
    TS(std::int64_t);
    TS(float);
    TS(double);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
