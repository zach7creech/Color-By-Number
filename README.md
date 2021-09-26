# Color-By-Number
Color-By-Number is a program that utilizes Dr. James Plank's graphing utility jgraph to convert images into color-by-number games.

## Using Color-By-Number
Installing ImageMagick is recommended. I haven't tested how `convert` and its options like `-resize` work without it.
ImageMagick can be installed on Linux operating systems with `sudo apt install imagemagick`.

After cloning this repository, simply type "make" to compile the program. The executable in /bin/ should already be current, but do it just to make sure.
Running "make clean" will remove the executable and all object files.
Make will also run the program on all of the images in the /examples/ directory, outputting each image's color-by-number in /examples/pdfs.

### Shell script color-by-number.sh
The easiest way to use the program is by using this shell script.
```
sh color-by-number.sh <input-image> <output-name> <color> <size> <landscape> <options>
```

### Required Arguments

* <input-image> The filename of the input image. Any image type that can be converted by ImageMagick is supported.
* <output-name> The filename for the outputted color-by-number. Extensions .pdf and .jpg are supported. I *highly* recommend using .pdf for denser images, as .jpg will likely compress the image too much to be readable.
* <color> The number of colors to use from /color/robust. Currently there are 6 options: robust, simpler, dull, flat, ROYGBIV, and extreme. Robust provides the most detail without being ridiculous, and each option beyond reduces the number of colors used. ROYGBIV and extreme are mostly novelties because they create images with too few and too many colors, respectively. When using the extreme command, colors in the legend will only be displayed with their hex codes. The extreme command outputs every color in the original image with no approximation, so it is incredibly unlikely that all of the colors in the legend will be displayed without running off the page.
