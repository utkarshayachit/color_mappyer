color_mappyer: Map scalars to colors
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

# generate an array with 1 million elements
# filled with random values between 0 and 1024
data = numpy.random.randint(0, 1024, 1000000, dtype=numpy.int32)

# generate a color map with 1024 entries
# color map with tuyples of (value, r, g, b) where value is float and r, g, b are uint8
scalars = numpy.linspace(0, 1023, 1024, dtype=data.dtype)
colors = numpy.random.randint(0, 256, (1024, 4), dtype=numpy.uint8)

r = color_mappyer.map_categorical_colors(data, scalars, colors)
print(data)
print(scalars)
print(colors)
print(r)

assert r.shape[0] == data.shape[0]
assert r.shape[1] == 4
assert r.dtype == numpy.uint8

# validate for random indices
for idx in numpy.random.randint(0, data.shape[0], 1000):
    assert numpy.all(r[idx] == colors[scalars[data[idx]]])
```
