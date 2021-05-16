cc=gcc209

%.o: %.c
	$(cc) -c $<

all: ish
clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f ish.o lexical.o syntactic.o execution.o dynarray.o

ish: dynarray.o lexical.o syntactic.o execution.o
	$(cc) $? -o $@

dynarray.o: dynarray.h
lexical.o: lexical.h
syntactic.o: syntactic.h
execution.o: dynarray.h lexical.h syntactic.h
