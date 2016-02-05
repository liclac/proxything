proxything
==========

Proxy thingy for a test assignment.

Dependencies:
-------------

* A recent version of [Boost](http://www.boost.org/) available in your INCLUDEPATH.
* [CMake](https://cmake.org/) 3.2+.

Both are readily available from [Homebrew](http://brew.sh/) if you're on a mac.

Design choices:
---------------

**C++** is the obvious choice for this application. An equivalent application implemented in C would be harder to read and maintain, and the memory management is full of unnecessary pitfalls. There are cases where C is a better choice (most notably resource-constrained, embedded systems), but generally speaking, it's better to use pieces of plain C inside a C++ program if needed - for the more complete standard library (featuring things such as maps, that need third-party implementations in C) if nothing else.

CMake is my go-to build system for C and C++. It can cover for platform differences and spit out project files for a large number of different toolchains and compilers - including Makefiles, Xcode projects and Visual Studio solutions. It's quite feature-rich, and has a clean syntax.

Of course, when I think "C++" and "networking", the first thing that springs to mind is the excellent **[ASIO](http://think-async.com/) library**. As C++11 pulled in all the stuff it needs into the standard library, it can run either with or without Boost, which leads to the next question: use Boost or not?

Here, I decided that yes, I will use **Boost**. I could just as well have used [Poco](http://pocoproject.org/), as both Boost and Poco provide the functionality I need (logging and argument parsing), just with different approaches. Boost has more utilities for various things and is overall very low-level, Poco has a more cohesive, opinionated foundation for building applications. It was basically a coin flip.

Next, I decided that, as a challenge, the server should be **single-threaded by default**. I eventually added support for running multiple threads to distribute the workload, but I wanted to make sure to make it **fully non-blocking**, rather than be lazy and spawn threads for every connection or something like that. It worked for nginx!

This decision caused a bit of a problem when it came to caching: there is no cross-platform way of doing asynchronous disk IO. I could just rely on disk IO being "instant" and do it synchronously... but it's only instant for me because my laptop has an SSD, and isn't under heavy enough load to slow it down. I've experienced the horrors of disk IO starvation first-hand, I don't ever want to be the cause of that.

So, I started digging through the ASIO docs. Turns out, you can provide your own services through the ASIO framework, and provide custom IO objects through it. So I built a service for asynchronous disk IO, with pluggable implementations for good measure (emulating the built-in services).

The default (and currently only) implementation uses a backgrund thread and an ASIO service as a task queue, to queue up synchronous disk operations on that thread. This keeps the server thread(s) responsive, and shouldn't become a bottleneck before your disk IO does (a claim that needs benchmarking to back up; increasing the number of IO threads is easy though).

A couple of quick-fire choices:

* In a "real" application, there would likely be a clear separation of subsystems (proxy, cache, etc), all with access to the application configuration (merged between commandline arguments and a configuration file). Not necessary here, as it's a very simple application, and all the configuration options we have can be set by `app` on startup.
* Boost's "trivial" logging setup is sufficient for this application.
* As much as possible should be in source files, as it speeds up compilation. Exceptions:
    * Trampoline functions (eg. everything in `fs_entry`) that are just inline delegates to something else.
    * Trivially inline-able getters and setters.
* No direct external access to member variables (named `m_`), except in structs.
* Empty- or single-statement functions can be written on one line.
* By calling Doxygen as part of the built process, undocumented functions become compiler warnings.
