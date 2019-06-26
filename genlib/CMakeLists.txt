set(genlib genlib)
set(genlib_SRCS
    bsptree.cpp  
    p2dpoly.cpp  
    pafmath.cpp  
    stringutils.cpp  
    xmlparse.cpp
	bsptree.h
	linreg.h
	readwritehelpers.h
	comm.h
	p2dpoly.h
	simplematrix.h
	containerutils.h 
	pafmath.h
	stringutils.h
	exceptions.h
	pflipper.h
	xmlparse.h)

add_compile_definitions(GENLIB_LIBRARY)

add_library(${genlib} STATIC ${genlib_SRCS})
