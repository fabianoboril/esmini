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

#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "Catalogs.hpp"
#include "Entities.hpp"
#include "Init.hpp"
#include "Story.hpp"
#include "ScenarioGateway.hpp"
#include "ScenarioReader.hpp"
#include "RoadNetwork.hpp"

namespace scenarioengine
{

	class ScenarioEngine
	{
	public:

		Entities entities;

		//	Cars cars;

		ScenarioEngine(std::string oscFilename, double startTime, ExternalControlMode ext_control = ExternalControlMode::EXT_CONTROL_BY_OSC);
		ScenarioEngine(const pugi::xml_document &xml_doc, std::string oscFilename, double startTime, ExternalControlMode ext_control = ExternalControlMode::EXT_CONTROL_BY_OSC);
		ScenarioEngine() {};
		~ScenarioEngine();

		void InitScenario(std::string oscFilename, double startTime, ExternalControlMode ext_control);
		void InitScenario(const pugi::xml_document &xml_doc, std::string oscFilename, double startTime, ExternalControlMode ext_control);

		void step(double deltaSimTime, bool initial = false);
		void setSimulationTime(double simulationTime);
		void setTimeStep(double timeStep);
		void printSimulationTime();
		void stepObjects(double dt);

		std::string getSceneGraphFilename() { return roadNetwork.SceneGraph.filepath; }
		std::string getOdrFilename() { return roadNetwork.Logics.filepath; }
		roadmanager::OpenDrive *getRoadManager() { return odrManager; }

		ScenarioGateway *getScenarioGateway();
		bool GetExtControl();

	private:
		// OpenSCENARIO parameters
		Catalogs catalogs;
		Init init;
		std::vector<Story*> story;
		ScenarioReader scenarioReader;
		RoadNetwork roadNetwork;
		roadmanager::OpenDrive *odrManager;
		ExternalControlMode req_ext_control_;  // Requested Ego (id 0) control mode

		// Simulation parameters
		double startTime;
		double simulationTime;
		double timeStep;

		// 

		//Conditions conditions;
		//Actions actions;
		ScenarioGateway scenarioGateway;

		void parseScenario(double startTime, ExternalControlMode ext_control);
	};

}
