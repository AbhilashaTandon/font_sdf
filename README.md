# Fontify

A little font renderer based on Sebastian Lague's [video](https://www.youtube.com/watch?v=SO83KQuuZvg) and [this paper](https://jcgt.org/published/0006/02/02/).

## Details

In rendering text, there are three main steps involved: parsing the font file, extracting the glyph data from the file and converting it to the proper format, and actually rendering the characters.

The code for the first part of this is contained in `font.h/cpp`, `font_file.h/cpp`, and `read_cmap.cpp`. The `FontFile` class stores the raw bytes of the file and has methods for seeking and extracting its variables. The `Font` class stores the data of the file parsed and separated into the variables we need. These files together parse a given font file in the TTF format and extract the raw bezier curves for each character. The more complex aspects of font rendering involving the TrueType instruction set are not dealt with, so the fonts may look unclear at small scales. The specification for the TTF file format can be found [at this page](https://developer.apple.com/fonts/TrueType-Reference-Manual/).

The second part is accomplished by `bezier.h/cpp`, `glyph.h/cpp`, and `glyph_rendering.cpp`. These files convert the data given in the TTF file to a more usable format, adding in implicit points in the bezier curves and converting offsets to actual coordinates in em space. This data is then packed into a texture containing the coordinates for each point in the glyph in groups of three for each quadratic bezier. The high and low bytes of the x coordinate are stored in the red and green channels of the texel respectively, and the high and low bytes of the y coordinate are stored in the blue and alpha channels.

The third part is accomplished by the shader, contained in `shader.frag`. This utilizes the "winding number" method used in the paper above to directly render bezier curves from their data.

## Todo

- Add antialiasing
- Add in virtual machine to run TTF instructions for small font rendering
- Add command line interface
- Use other methods from the paper (horizontal and vertical bands) to improve performance
