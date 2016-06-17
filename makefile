
cms: cms.cpp database.cpp cms.h database.h
	g++ -std=c++11 -stdlib=libc++ cms.cpp database.cpp -o cms

test: cms
	./cms < input1.testin > input1.testout.test
	diff input1.testout input1.testout.test
