![X project logo. Two letters D and C, extruded slightly and placed against a quad within the 3D application Blender.](https://github.com/DavidCradock/DC/blob/51c30954811147e2ef6a3bb882f0f1131df434be/github_images/github_social_image.png)
# My current personal project.
## Written in C++ for Windows 11, using OpenGL/Vulkan for rendering.
### This project is intended to become a bunch of code which I can use to develop a game with.
#### Not only a game, but also applications too. I wish to implement an image editor to replace my aging Adobe Photoshop as it's user interface does not scale on high DPI displays making the interface incredibly difficult to use and I find the way which open source alternatives deal with the alpha channel awkward. I also refuse to pay a monthly fee for software to obtain access to the latest version of Photoshop.
** Build help **
- Download the source as a zip and extract somewhere.
- Download and install Visual studio
- Load the X.sln file into Visual studio, hit compile, done.

TODO
Go through all code and refactor params so that they are all lowercase for their first character and have PARAM at the end in the definition only and only if needed.
Go through all header file comments and check to make sure the mentioned variables are correct
Add audio from old codebase and refactor

Notes...

No logging because it's un-needed overhead and I usually implement logging, then never use it. The debugger is well suited for, surprise, debugging. The logs never help me.

Changes to coding style

Do not use _m for members as it's bloody obvious that the member is a member.

Do not use pre-pended type (for example fSomeFloat) as the IDE shows the type of a member/variable)

Do not use _ underscore for private members as the IDE shows that a member is private.

Use a descriptive, non abbreviated name for a variable/member. Abbreviation makes things less clear and autocomplete makes adding names easy with less typing.

Do not add "C" to beginning of class declarations, it's a class, it's obvious.


Error handling

Do not use exceptions, there's some overhead and using macros for critical errors is fine.

Singleton pattern.

Do not use unless appropriate.

Singletons are great for allowing quick access to a "global" object, but as their name implies, prevents creation of multiple instances.

Their initialisation order can be really messy.

Use globals instead, or pass pointers/references to objects.

Strings.

Use wide strings as it's the default setting, they are recommended for many positive reasons (IE some user with a weird character is now supported) and it's easier to work with than multibyte.

Sure, the L"bloody L!" is kinda annoying having to add Ls everywhere, but calm down, it's only a single character to add. Relax! :D

Source code filenaming.

Prepend "DC" for the library's main include header (and possible .cpp), but for the rest of the files, do not add the "DC"

Camel style casing, someFileWhichDoesSomething. It's easy to read and quite sparse.

For parameter to methods/functions follow the above naming (IE no f/i/str etc) make first letter uppercase if needed (For example when needing to differenciate between object members)
