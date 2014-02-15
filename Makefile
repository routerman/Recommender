recom: recom.cpp
	g++ -O2 -o recom recom.cpp `pkg-config jubatus-client --libs --cflags`
