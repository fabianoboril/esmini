/* 
 * esmini - Environment Simulator Minimalistic 
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

#pragma once
#include "RoadManager.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

namespace scenarioengine
{

#define NAME_LEN 32

	struct ObjectStateStruct
	{
		int id;
		int model_id;
		int ext_control; // 1=on 0=off
		float timeStamp;
		char name[NAME_LEN];
		roadmanager::Position pos;
		float speed;
	};

	class ObjectState
	{
	public:
		ObjectState();
		ObjectState(int id, std::string name, int model_id, int ext_control, double timestamp, roadmanager::Position *pos, double speed);
		ObjectState(int id, std::string name, int model_id, int ext_control, double timestamp, double x, double y, double z, double h, double p, double r, double speed);
		ObjectState(int id, std::string name, int model_id, int ext_control, double timestamp, int roadId, int laneId, double laneOffset, double s, double speed);

		ObjectStateStruct getStruct() { return state_; }

		void Print();

		ObjectStateStruct state_;

	private:

		friend class ScemarioGateway;
	};


	class ScenarioGateway
	{
	public:

		ScenarioGateway();
		~ScenarioGateway();

		void reportObject(ObjectState objectState);
		int getNumberOfObjects() { return (int)objectState_.size(); }
		ObjectState getObjectStateByIdx(int idx) { return *objectState_[idx]; }
		ObjectState *getObjectStatePtrByIdx(int idx) { return objectState_[idx]; }
		int getObjectStateById(int idx, ObjectState &objState);
		int RecordToFile(std::string filename, std::string odr_filename, std::string model_filename);

	private:
		std::vector<ObjectState*> objectState_;
		std::ofstream data_file_;
	};

}