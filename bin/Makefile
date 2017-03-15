exec		=	GLL_main

lib		= 	gll

CPPFLAGS 	+=	-g -I../inc -Wall 
LDFLAGS		+=	-L../lib

.PHONY		:	clean

all		:	$(exec)

clean           :	
			rm -rf $(exec) 

$(exec)	: % : %.cc
		$(CXX) $(CPPFLAGS) $(LDFLAGS) $< -o $@ -lpthread -lgll -lTrg

$(exec)	: ../lib/libgll.a


