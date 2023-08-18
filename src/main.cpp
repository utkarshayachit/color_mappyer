#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <unordered_map>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

/// Map categorical values to colors. For any value not in the colormap, the `nan_color` is used.
/// @param data The data to map (ndim == 1, shape == (data_size,))
/// @param values The values in the colormap (ndim == 1, shape == (N,))
/// @param colors The colors in the colormap (ndim == 2, shape == (N, 4))
/// @param nan_color The color to use for values not in the colormap
/// @return The mapped colors (ndim == 2, shape == (data_size, 4))
template <typename T>
py::array_t<std::uint8_t> discrete(const py::array_t<T>& data,
    const py::array_t<T>& values, const py::array_t<std::uint8_t>& colors,
    const std::array<std::uint8_t, 4>& nan_color = {0, 0, 0, 0}) {

    // validate the inputs
    auto data_buf = data.request();
    if (data_buf.ndim != 1) {
        throw std::runtime_error("Number of dimensions for `data` must be one");
    }

    auto values_buf = values.request();
    if (values_buf.ndim != 1) {
        throw std::runtime_error("Number of dimensions for `values` must be one");
    }

    auto colors_buf = colors.request();
    if (colors_buf.ndim != 2) {
        throw std::runtime_error("Number of dimensions for `colors` must be two");
    }

    if (colors_buf.shape[1] != 4) {
        throw std::runtime_error("Number of columns for `colors` must be four");
    }

    if (colors_buf.shape[0] != values_buf.shape[0]) {
        throw std::runtime_error("Number of rows for `colors` must match number of rows for `values`");
    }

    // Create a map from the colormap to speed up the lookup
    std::unordered_map<T, std::array<std::uint8_t, 4>> colormap;

    auto values_ptr = static_cast<T*>(values_buf.ptr);
    auto colors_ptr = static_cast<std::uint8_t*>(colors_buf.ptr);
    for (size_t idx = 0; idx < static_cast<size_t>(values_buf.shape[0]); idx++) {
        colormap[values_ptr[idx]] = {
            colors_ptr[idx * 4 + 0],
            colors_ptr[idx * 4 + 1],
            colors_ptr[idx * 4 + 2],
            colors_ptr[idx * 4 + 3],
        };
    }

    // Create the output array (shape == (data_size, 4))
    auto result = py::array_t<std::uint8_t>(data_buf.size * 4).reshape({static_cast<int>(data_buf.size), 4});

    // map the data
    auto data_ptr = static_cast<T*>(data_buf.ptr);
    auto result_ptr = static_cast<std::uint8_t*>(result.request().ptr);
    for (size_t idx = 0; idx < static_cast<size_t>(data_buf.size); idx++) {
        auto it = colormap.find(data_ptr[idx]);
        if (it != colormap.end()) {
            std::copy(it->second.begin(), it->second.end(), result_ptr + idx * 4);
        } else {
            std::copy(nan_color.begin(), nan_color.end(), result_ptr + idx * 4);
        }
    }

    return result;
}

#define TS_OVERLOAD(T) \
    [](const py::array_t<T>& data,  \
        const py::array_t<T>& values, const py::array_t<std::uint8_t>& colors) { \
        return discrete(data, values, colors); \
    }

#define TS(_type) \
    m.def("discrete", TS_OVERLOAD(_type), \
        py::arg().noconvert(), \
        py::arg().noconvert(), \
        py::arg().noconvert(), \
        R"pbdoc(     \
        Map numpy scalar array to colors using a colormap \
    )pbdoc");

PYBIND11_MODULE(color_mappyer, m) {
    m.doc() = R"pbdoc(
        Color Mapper
        -----------------------

        .. currentmodule:: color_mappyer

        .. autosummary::
           :toctree: _generate

           discrete
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
