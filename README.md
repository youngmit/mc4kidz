# mc4kidz
MC 4 Kidz! is a simple monte carlo neutron transport simulator.
While wildly inaccurate and absolutely useless for serious work of any kind, it is fun to watch and play with.
It was created as demonstration of basic nuclear engineering computational methods for outreach events and the like.

Screenshot:
![Sorry for the resolution](https://raw.githubusercontent.com/youngmit/mc4kidz/master/screenshot.PNG)

# Features
 - Supports simple geometries made of circles and boxes, but by default starts with a 10x10 square lattice of fuel
 - Uses multi-group macroscopic cross sections, and comes with several materials from the C5G7 library
 - Play/pause with frame-by-frame advancement
 - Restart functionality
 - Arbitrary particle insertion with mouse
 - Cycling through region materials with mouse
 - Pie chart showing interaction probabilities. Not as interesting as I had hoped, since most cases are so scattering-dominated
 - Histogram showing energy spectrum
 - Line plot showing total neutron population with time
 - Togglable vacuum/reflective boundary condition
 
# How to build
MC 4 Kidz! should work under Windows and Linux, and is built using CMake.
It is based on OpenGL and GLUT for now, so you will need these in your system.
In linux this usually means having your video driver set up and some sort of `dev` package with the OpenGL headers, and `freeglut`.
Under windows, I found it pretty easy to obtain these using `vcpkg`. Check that out over at https://github.com/Microsoft/vcpkg.
I use `std::optional` in some places, so a C++17 compiler is necessary.

If you're in windows, just get a hold of the latest version of VS and open the directory that you cloned this repository into and it should sort of do the rest.
What a fascinating modern world we live in.

In linux just make a build directory somewhere, `cd` into it and run something like `cmake [path to repo]`, then `make`.
An `mc4kidz` executable should be sitting in the `mc4kidz` directory.

# User Manual
When starting the program, the simulation should be paused; see below for how to get it started.
Right now, the simulation will always start with some number of particles, emitted isotropically from the center of the domain (the number changes with my whims, and at some point might become configurable).
They start in the slowest energy group, and are red, which indicates that they are in the zero-th generation.
As fissions occur, the daughter neutrons will be orange, then yellow, through GOYGBIV, back to red again.

Since the range of particle velocities in the real world is so large that it would be impossible to discern motion in both fission neutrons and thermal neutrons, the particle velocity is super approximate.
As a result, time-dependent phenomena are straight-up wrong.
I suppose at some point I could go into the material properties and adjust them accordingly to at least get the correct reaction rates, but ¯\\\_(ツ)_/¯.

The particle tracking in MC 4 Kidz! is also pretty unphysical, in that particle motion is constrained to the 2-D plane.
As a result, particles tend to move "farther" than they ought to, since in the real 3-D world, particles should have velocity components into/out of the page/screen.
This is done for simplicity, and because accuracy isn't even close to a priority.

## Controls

The controls for MC 4 Kidz! are admittedly esoteric, but pretty simple:
 - `r`: Restart the simulation
 - `p`: Play/pause the simulation
 - `space`: Advance the simulation by one frame when paused
 - `b`: Toggle the reflective boundary condition
 - `l`: Toggle particle labels (useful for debugging)
 - `w`: Toggle particle waypoints (useful for debugging, but also fun to look at when there aren't many particles)
 - `c`: Cycle all pin materials in the lattice
 - Left mouse button: Emit thermal neutrons from the m

It is possible to start the executable with a single command-line argument, specifying the path to a "Playbook" file.
Such a file is used to automate the interaction of a user, allowing the program to run through an unattended demonstration.
The format of this file is very simple, but for the time being not fully specified here.
Each line of the file looks something like this:

`N_tics Instruction [arguments]`

`N_tics` is the number of frames to render before carrying out the specified `Instruction`, which depending on the instruction may take one or more arguments. The current set of instructions is:
 - `reset`
 - `toggle_boundary`
 - `toggle_material`
 - `set_material`
 - `add_particles`
 - `noop`
 - `halt`
 
 For an example playbook file, look at `tests/test_playbook.txt`.