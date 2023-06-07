import numpy
import color_mappyer

# generate an array with 1 million elements
# filled with random values between 0 and 1024
data = numpy.random.randint(0, 1024, 1000000, dtype=numpy.int32)

# generate a color map with 1024 entries
# color map with tuyples of (value, r, g, b) where value is float and r, g, b are uint8
scalars = numpy.linspace(0, 1023, 1024, dtype=data.dtype)
colors = numpy.random.randint(0, 256, (1024, 4), dtype=numpy.uint8)

r = color_mappyer.discrete(data, scalars, colors)
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
