#pragma once

#include <stdlib.h>

class Random
{

public:

	int RandomInt(int _maxVal) { return RandomInt(0, _maxVal); }

	int RandomInt(int _minVal, int _maxVal) { return (rand() % (_maxVal - _minVal)) + _minVal; }

};