# OpenGL2dEx Developer's Guide
This project is based on the 2D game example over at [Learn OpenGL](https://learnopengl.com).  I have kept the overall classes the same, but put my own touches on the way these classes are implemented and how they interact (e.g. instead of using string IDs that are hard-coded, I allow the ResourceManager to specify the ID of a loaded texture or shader, which is then passed through the other classes).  However, my implementation isn't crazy different from the implementation there.  I've only really tested this on a Windows 10 laptop and a Windows 10 desktop, so I'm not certain how well it behaves on other machines, although it worked nicely when I switched between the two so it should hopefully work on other machines without issue.

## Libraries
Here is a list of the libraries I currently use (not sure all of these are used in this project - I need to check): 
 * assimp
 * freetype-2.10.2
 * glad
 * GLFW
 * glmirrklang-32bit-1.6.0
 * KHR
 * stb

I'll try to figure out the best way to deal with these, but for now, the best resource is learnopengl.com until I get this all set up better.

**Disclaimer: I haven't tested this out a ton and I know there are certainly some bugs with collisions.

TODO(sasiala): update libraries & fill out rest of ReadMe