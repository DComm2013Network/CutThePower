# This script creates a Makefile for a project.
# All of the source files must be in a /src folder.
# This script will create a /obj folder for the object 
# files and a /bin folder for the executable.
#
# By Jordan Marling
#
# Usage:
#	./make.sh <executable name>

if [ ! $1 ]
then
	echo "Usage: ./make.sh <executable name>"
	exit
fi


if [ ! -d "src" ]
then
	echo "src directory does not exist."
	exit
fi

FILE="Makefile"

FLAGS="-Wall"
LIBS="-lSDL2 -lSDL2_net"
CC="g++"

SRC_FILES=""
COMPILE=""

strindex() { 
  x="${1%%$2*}"
  [[ $x = $1 ]] && echo -1 || echo ${#x}
}


if [ -f $FILE ]
then
	
	contents=$(<$FILE)
	
	a=$(strindex "$contents" "CC=")
	a=$(($a + 3))
	b=$(strindex "${contents:$a}" $'\n')
	
	CC=${contents:$a:$b}
	
	a=$(strindex "$contents" "FLAGS=")
	a=$(($a + 6))
	b=$(strindex "${contents:$a}" $'\n')
	
	FLAGS=${contents:$a:$b}
	
	a=$(strindex "$contents" "LIBS=")
	a=$(($a + 5))
	b=$(strindex "${contents:$a}" $'\n')
	
	LIBS=${contents:$a:$b}

fi

src="src/*"
for f in $(find $src -name '*.c' -or -name '*.cpp');
do
	
	f=${f:4}
	obj=${f/.cpp/.o}
	obj=${obj/.c/.o}
	cpp=$f
	SRC_FILES="$SRC_FILES \$(OBJDIR)/${obj}"
	
	path=$(dirname "$obj")
	
	if [ "$path" == "." ]
	then
		path=""
	fi
	
	COMPILE="$COMPILE
\$(OBJDIR)/${obj}: \$(SRCDIR)/${cpp}
	test -d \$(OBJDIR)/$path || mkdir -p \$(OBJDIR)/$path
	\$(CC) \$(FLAGS) -c -o \$(OBJDIR)/${obj} \$(SRCDIR)/${cpp}
"
	
done





echo "CC=${CC}
FLAGS=${FLAGS}
LIBS=${LIBS}

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=\$(BINDIR)/${1}
OBJ_DEFAULT=${SRC_FILES:1}

${1}: \$(OBJ_DEFAULT)
	test -d \$(BINDIR) || mkdir -p \$(BINDIR)
	\$(CC) \$(FLAGS) -o \$(BINDIR)/${1} \$(OBJ_DEFAULT) \$(LIBS)

run: ${1}
	\$(BINDIR)/${1}

clean:
	rm -f \$(OBJ_DEFAULT) \$(BIN_DEFAULT)

debug: \$(OBJ_DEFAULT)
	test -d \$(BINDIR) || mkdir -p \$(BINDIR)
	\$(CC) \$(FLAGS) -g -o \$(BINDIR)/${1} \$(OBJ_DEFAULT) \$(LIBS)
	gdb -tui \$(BINDIR)/${1}

${COMPILE}" > $FILE
