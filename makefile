
cms: cms.cpp database.cpp cms.h database.h
	g++ -std=c++11 -stdlib=libc++ cms.cpp database.cpp CMSOutput.cpp network.cpp CMSInput.cpp -o cms

test: cms
	./cms < input1.testin > input1.testout.test
	diff input1.testout input1.testout.test
	./cms < input2.testin > input2.testout.test
	diff input2.testout input2.testout.test
