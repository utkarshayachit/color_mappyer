import numpy
import color_mappyer

# generate an array with 1 million elements
# filled with random values between 0 and 1024
data = numpy.random.randint(0, 1024, 1000000, dtype=numpy.int32)

# generate a color map with 1024 entries
# color map with tuyples of (value, r, g, b) where value is float and r, g, b are uint8
colormap = numpy.array([],
                        dtype=[('value', data.dtype), ('r', numpy.uint8), ('g', numpy.uint8), ('b', numpy.uint8), ('a', numpy.uint8)])
for i in range(1024):
    colormap = numpy.append(colormap, numpy.array([(i, i % 256, (i // 256) % 256, (i // 65536) % 256, 255)], dtype=colormap.dtype))

print(colormap)
r = color_mappyer.map_categorical_colors(data, colormap)
print(r)
