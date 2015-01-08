
MESSAGE("Adding all the plugins directories")
# add the sub-directories

ADD_SUBDIRECTORY(libiotkitpubsub)
ADD_SUBDIRECTORY(libmqttpubsub)
ADD_SUBDIRECTORY(libzmqpubsub)
ADD_SUBDIRECTORY(libzmqreqrep)

# set the compiler flags
SET(CMAKE_C_FLAGS "-c -fPIC")
