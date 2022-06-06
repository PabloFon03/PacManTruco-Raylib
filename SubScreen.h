#pragma once

#include "Resources.h"

// Abstract Class For Screen Contents
class SubScreen
{

public:	

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void OnDraw() = 0;

	int exitFlag{ 0 };

	Resources* resources;

};