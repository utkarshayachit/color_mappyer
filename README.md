ColorMapPyer: Map scalars to colors
==============

This Python package can be used to map a scalar array to categorical colors using a colormap. This is
intended as a quick workaround for performance issue with VTK's support for categorical colors.

Installation
------------

 - clone this repository
 - `pip install ./colormappyer`

License
-------

pybind11 is provided under a BSD-style license that can be found in the LICENSE
file. By using, distributing, or contributing to this project, you agree to the
terms and conditions of this license.

Test call
---------

```python
import numpy
import color_mappyer

# generate an array with 100 million elements
# filled with random values between 0 and 1024
data = numpy.random.randint(0, 1024, 1000000000, dtype=numpy.int32)

# generate a color map with 1024 entries
# color map with tuples of (value, r, g, b) where value is data.dtype and r, g, b are uint8
colormap = numpy.array([],
            dtype=[('value', data.dtype), ('r', numpy.uint8), ('g', numpy.uint8), ('b', numpy.uint8), ('a', numpy.uint8)])
for i in range(1024):
    colormap = numpy.append(colormap,
        numpy.array([(i, i % 256, (i // 256) % 256, (i // 65536) % 256, 255)],
        dtype=colormap.dtype))

print(colormap)
result = color_mappyer.map_categorical_colors(data, colormap)
```

[`cibuildwheel`]:          https://cibuildwheel.readthedocs.io
