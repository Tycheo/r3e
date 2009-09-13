#ifndef RANGE_H
#define RANGE_H

#include "Random.hpp"

template<class T> class Range {
public:
	Range(){}
	Range(T min, T max) : mMin(min), mMax(max) {}

	~Range(){}

	T GetRandom(){
		return Random(mMin, mMax);
	}
	
	T GetRange(){
		return mMax - mMin;
	}

	T mMin;
	T mMax;
};

#endif