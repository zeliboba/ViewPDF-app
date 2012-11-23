Notes
-----

The repository is created from tarballs of few latest releases taken from the
homepage, I did not check if the full development history is available
publicly (there are remains of mercurial VCS).

The following package is required to compile Ubuntu/Debian probably in
addition to those mentioned in [DiffPDF-app](DiffPDF-app)

    $ sudo apt-get install libenchant-dev

-----

ViewPDF
=======

ViewPDF is used to view PDF files.

ViewPDF shows PDF files, clearly indicating annotations and possible
mistakes (such as spelling errors).

Home page: http://www.qtrac.eu/viewpdf.html


Compiling and Installing ViewPDF
================================

Prerequisites: A C++ compiler, the Qt 4 libraries (at least Qt 4.6), 
the Poppler libraries (including Poppler's Qt 4 headers; at least
0.18; but 0.20.4 or later is recommended), the enchant library, and at
least one spelling library for enchant to use and at least one spelling
dictionary.

1. Unpack the archive file, viewpdf-XXX.tar.gz

    $ tar xvfz viewpdf-XXX.tar.gz

2. Change directory to viewpdf-XXX

    $ cd viewpdf-XXX

3. Run qmake; on some systems, run qmake-qt4

    $ qmake

4. Run make

    $ make

5. Copy or soft-link the viewpdf executable to somewhere on your PATH
6. Only the executable is needed; all the files that were unpacked or
   generated can be safely deleted.

That's it!


Running ViewPDF
===============

If you hit a bug, please report it to mark@qtrac.eu. Be sure to include
"ViewPDF" in the subject line and specify the version you are using
and details of your system, e.g., operating system name and version,
compiler name and version, Qt library version, Poppler library version.


License
=======

This program was written by Mark Summerfield.
Copyright (c) 2012 Qtrac Ltd. All rights reserved.

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 2 of the License, or (at your
option), any later version. This program is distributed in the hope that
it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License (in file gpl-2.0.txt) for more details.
