# Starfield Battle

This is the source code for a small game I made back in 2003.
I'm planning to use this project for a series of videos
where I'll clean up, modernize and refactor the code
to make it easier to read and bring it up to modern standards and best practices.

Links to the videos will be added here as I publish them.

## The original project

This is the game:

![](screenshots/gameplay.png?raw=true "Starfield Battle gameplay screenshot")

It's a pretty typical space shooter.
You fight randomly appearing enemy space ships
while the stars fly by. Each enemy has
a distinct pattern of movement.
The goal is to survive as long as possible and score points.
Score enough points and you make it into the high score list.

There's also a local multiplayer mode,
where two players on the same keyboard compete
to score the most points within a time limit.
Players cannot die in that mode,
taking damage subtracts points instead (down to a minimum of -20).

![](screenshots/multiplayer.png?raw=true "Starfield Battle multiplayer screenshot")

Back when I made this, I was 15 years old and still in highschool.
I did most of the work in 2003 during the summer break.
Based on the timestamps of the original files,
I also did some more follow-up work in early 2004,
but I don't remember what exactly those changes were
(I didn't use any version control system back then).
It might have been the multiplayer mode,
but I'm not sure.

At that time, I already had some experience programming in BASIC,
C (with some C++ features),
and Delphi (Pascal + OOP).
But this was the first bigger project I did,
considerably more complex than anything I had done before.

I was using Visual Studio 6.0 as IDE and compiler.
The game uses DirectX (for graphics, input and sound) and the Win32 API,
but no libraries otherwise,
just the standard library.

Most of the graphics were made in MS Paint,
with some silly lens flare and lighting effects added via Photoshop.
The explosion graphic however was "borrowed" from some commercial game,
and I don't remember where I got the sound effects from but I can only
imagine that they were also ripped from other games and maybe some
websites with free sound effects.

The original executable still somewhat runs on modern Windows (i.e. Windows 10),
but the exclusive fullscreen mode that it uses is pretty wonky.
It switches screen resolution to 800x600,
which looks terrible on a modern 4k or 1080p display.
Screen recording tools like OBS or even FRAPS don't work with the game.
And sometimes, it just crashes during startup.
On Windows 7, the game still runs pretty well.

## State of the code

The original code is, frankly, quite bad 😄
Please don't use this as an example of how to do things!

It's overly complex, inconsistent, redundant, poorly structured,
using many outdated paradigms, etc.

Overall, it's still mostly readable.
But a big portion of the logic is implemented as one single gigantic
function with very deeply nested if statements,
and a tangled mess of variables with
inconsistent scope and life time to keep track of state
(globals, locals, and static locals are mixed without any clear reason).
I'm pretty sure that there are also some memory leaks,
and instances of undefined behavior.

But all of these things can be fixed and improved!
So that's what I'm going to do with this code.
Once the video series is over,
the commit history of this repository should offer some
great examples for cleaning up and improving old legacy code
as well.

It's also worth noting that despite the quality of the code,
the game itself runs stable,
and is still fun to play.
Especially the multiplayer mode.

And, most importantly,
the experience of writing this game solidified my love
for programming - there's a good chance I might have chosen
a different career path if it weren't for this project.

## The plan

I'm going to target C++ 20.
Initially, I'll keep using MSVC on Windows,
but ultimately I'd like to make the code cross platform
and compatible with clang and gcc.

I'll figure out the exact details when recording the videos,
so this might still change somewhat.
But here's my rough plan for what I want to do with the code:

* ~Get it to compile in modern MSVC~
* Make it run better on Windows 10
* Run clang-format
* Eliminate all compiler warnings
* Remove dead code
* Run clang-tidy
* Maybe run some other static analyzers
* Replace use of Windows-specific types like `BOOL` with regular standard types
* Replace manual memory management with smart pointers
* Replace hand-rolled linked list class with standard containers
* Replace use of unsafe C library functions with modern equivalents
* Replace low-level string manipulation code with `std::string` and `std::format`
* Improve the structure and organization generally
* Better organize state, establish clear ownership and life time
* Break up the gigantic main function by extracting smaller sub functions
* Simplify overly complex & redundant logic and data structures
* Improve naming
* Simplify error handling and translate error messages to English
* Replace usage of DirectX and Win32 APIs with [SFML](https://www.sfml-dev.org/)
* Create a CMake project file to replace the Visual Studio solution
* Get the code to build and run on Linux and Mac OS
* Run with asan and ubsan sanitizers and eliminate all errors they find
* Set up CI with GitHub actions
* Rework the odd and unusual timing code
