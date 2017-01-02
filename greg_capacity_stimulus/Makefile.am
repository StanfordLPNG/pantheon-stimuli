AM_CPPFLAGS = $(CXX11_FLAGS) -I$(srcdir)/../src
AM_CXXFLAGS = $(PICKY_CXXFLAGS)
LDADD = ../src/libsourdough.a -lpthread

controller = controller.hh controller.cc

bin_PROGRAMS = sender receiver

sender_SOURCES = ../src/sender.cc $(controller)

receiver_SOURCES = ../src/receiver.cc $(controller)
