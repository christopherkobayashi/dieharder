#! /bin/sh

# Run this to generate all the auto-generated files needed by the GNU
# configure program.  This must be done FIRST; note well that there is
# no Makefile to run until it is done.

echo "Running autoheader..."
autoheader
echo "Running aclocal..."
aclocal
echo "Running libtoolize..."
libtoolize
# rm -f Makefile Makefile.in dieharder/Makefile dieharder/Makefile.in \
#    libdieharder/Makefile libdieharder/Makefile.in
echo "Running automake..."
automake --add-missing --copy --gnu

echo "Running autoreconf..."
autoreconf

echo "Now run ./configure with desired arguments."
