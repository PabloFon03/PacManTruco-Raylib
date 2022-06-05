#pragma once

// Abstract Class For Screen Contents
class SubScreen
{

public:

	SubScreen();
	virtual ~SubScreen();

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void OnDraw() = 0;

	int exitFlag{ 0 };

};