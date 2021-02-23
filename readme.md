# OpenGL2dEx Developer's Guide
*Note: this will be difficult to get running at the moment - LearnOpenGL.com is the best resource while I figure out the best/safest way to add the libraries to the project.  See the "Libraries" section for some more info.*

This project is based on the 2D game example over at [Learn OpenGL](https://learnopengl.com).  I have kept the overall classes the same, but put my own touches on the way these classes are implemented and how they interact (e.g. instead of using string IDs that are hard-coded, I allow the ResourceManager to specify the ID of a loaded texture or shader, which is then passed through the other classes).  However, my implementation isn't crazy different from the implementation there.  I've only really tested this on a Windows 10 laptop and a Windows 10 desktop, so I'm not certain how well it behaves on other machines, although it worked nicely when I switched between the two so it should hopefully work on other machines without issue.

## Libraries
Here is a list of the libraries I currently use (not sure all of these are used in this project - I need to check): 
 * assimp
 * freetype-2.10.2
 * glad
 * GLFW
 * glm
 * irrklang-32bit-1.6.0
 * KHR
 * stb

I'll try to figure out the best way to deal with these, but for now, the best resource is learnopengl.com until I get this all set up better.

## Credits
 * Portions of this software are copyright © 2020 The FreeType Project (www.freetype.org).  All rights reserved.  The FreeType project is licensed under the FTL license.
 * This software makes use of the assimp library for importing assets.
 * This software makes use of the GLFW library for windowing and input processing.
 * This software makes use of the GLM library under the MIT license.
 * This software makes use of the irrKlang library under their free (non-commercial) licensing.  Please see [irrKlang's website](https://www.ambiera.com/irrklang/index.html) for more information about irrKlang.
 * This software makes use of the stb image loading software under the public domain.

**Disclaimer: I haven't tested this out a ton and I know there are certainly some bugs with collisions.

TODO(sasiala): update libraries & fill out rest of ReadMe
