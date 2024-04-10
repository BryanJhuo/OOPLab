#pragma once

#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H
#include "../Game/myObject.h"

class MapController: public Object {
public:
	game_framework::CMovingBitmap mapController[6];
	bool controllerState[6];

	void generateObject();
	void showObject(int map_stage);

};

#endif // !MAPCONTROLLER_H