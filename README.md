# VoronoiMapGen
Generation of random Island maps with Voronoi using C++

About the Procedural Island Generator
-------
This project is a procedural island generator following the tutorial at [http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/](http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation) 

It has the following steps:

   - Place a number of random but uniformly distributed points.
   - Calculate the Voronoi diagran of the points.
   - Determine which cells are land and which are water.
   - Calculate the elevation of each point as its distance to the sea. Normalize all the heights.
   - Place river sources in random spots and let them flow downwards.
   - Calculate the moisture value each point given its distance to the sea (less moisture) and rivers (more moisture)
   - Calculate the Biome according to moisture

How to Use
-------
Run the exe Executable file under Bin\Debug\ or Bin\Release floder, some parameters as below:
 - the 2D image size can be changed from 20 to 65535, I think too small image size is meaningless.
 - In a shape number like 20928-2, 20928 chooses the overall island shape and 2 is the random number seed for the details (random points, noisy edges, rivers). 
 You can type in a shape number and press "Generate" button to generate that map. The details will vary based on the other options you’ve selected.
 - Radial, Perlin, Square, Blob are about the island shape.
 - Relaxed, PoissonDisk, Squared, Hexagon are about how the map is divided up into polygons.
 - site number also can be changed, but using 4000 or 8000 points can be slow.
 - The PNG output is now 1024x1024, and draws the selected mode (biomes, polygons, smooth, 2d slopes, 3d slopes etc.). 
 - The PNG output and XML output will be generated under the Data\Output\ directory.

Requirements
-------
No other requirment, the project is based [my SGP Engine](https://github.com/phoenixzz/SGPEngine).
You can open Builds\VisualStudio2010\VoronoiMapGen.sln with VS 2010, Compile and Enjoy it!


Developed By
-------
I would like to give thanks to the people whose libraries helped make this project a reality:

- [ImGui](https://github.com/ocornut/imgui)  Immediate Mode Graphical User interface for C++ with minimal dependencies
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)Single file Library: image loading/decoding from file/memory: JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC
- [stb_image_resize.h](https://github.com/nothings/stb/blob/master/stb_image_resize.h)Single file Library: resize images larger/smaller with good quality
- [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)Single file Library: image writing to disk: PNG, TGA, BMP
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) Single file Library: modal dialogs inc. file open/save
- [pugixml](http://pugixml.org/) Light-weight, simple and fast XML parser for C++ with XPath support

by phoenixzz (Weng xiao yi) - <phoenixzz@sina.com>