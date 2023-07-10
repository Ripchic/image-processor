# image-processor
C++ project oriented on interaction with images
# Graphic filters (image_processor)
A console application that allows to apply various filters to images, similar to filters in popular graphic editors.


## Supported image format

The input and output image files must be in [BMP](http://en.wikipedia.org/wiki/BMP_file_format) format.

The BMP format supports quite a lot of variations, but in this task will be used
24-bit BMP without compression and without a color table. Type of used `DIB header` - `BITMAPINFOHEADER`.

An example of a file in the desired format is available in the Wikipedia article [in the "Example 1" section](https://en.wikipedia.org/wiki/BMP_file_format#Example_1).

When testing, be sure to pay attention to the fact that the test image was saved in a 24-bit BMP.

## Format of command line arguments

Description of the format of command line arguments:

`{name} {path to a input file} {path to a output file}
[-{filter name 1} [filter parameter 1] [filter parameters 2] ...]
[-{filter name 2} [filter parameter 1] [filter parameter 2] ...] ...`

## Compilation

`cmake -B build -S . -D CMAKE_BUILD_TYPE=Release`

`cmake --build build `


### Example
`./image_processor input.bmp /tmp/output.bmp -crop 800 600 -gs -blur 0.5`

In this example
1. An image is loaded from a file `input.bmp`
2. It is cropped to an image with the beginning in the upper left corner and the size of 800x600 pixels
3. Transfers to shades of gray
4. A blur with sigma 0.5 is applied
5. The resulting image is saved to a file `/tmp/output.bmp`

The filter list may be empty, then the image must be saved unchanged.
Filters are applied in the order in which they are listed in the command line arguments.

## Filters

Crop

`-crop width height`

Grayscale

`-gs`

Negative

`-neg`

Sharpening

`-sharp`

EdgeDetection

`-edge threshold`

GaussianBlur

`-blur sigma [kernel_radius]`

Pixelation

`-pixelate pixel_size`

Embossing

`-emboss`

Noise

`-noise strength`

Walhorisation

`-walhorisation`

Twirl

`-twirl X Y`
