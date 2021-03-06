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
#include "RoadNetwork.hpp"
#include "Catalogs.hpp"
#include "Entities.hpp"
#include "Init.hpp"
#include "Story.hpp"
#include "OSCPosition.hpp"
#include "OSCProperties.hpp"
#include "pugixml.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace scenarioengine
{

	typedef enum
	{
		EXT_CONTROL_BY_OSC,
		EXT_CONTROL_OFF,
		EXT_CONTROL_ON,
	} ExternalControlMode;

	class ScenarioReader
	{
	public:

		ScenarioReader();
		int loadOSCFile(const char * path, ExternalControlMode ext_control);
		void loadOSCMem(const pugi::xml_document &xml_doc, const char *path, ExternalControlMode ext_control);

		void LoadCatalog(pugi::xml_node catalogChild, Entities *entities, Catalogs *catalogs);

		// RoadNetwork
		void parseRoadNetwork(RoadNetwork &roadNetwork);
		void parseOSCFile(OSCFile &file, pugi::xml_node fileNode);

		// ParameterDeclaration
		void parseParameterDeclaration();

		// Catalogs
		void parseCatalogs(Catalogs &catalogs, Entities *entities);
		roadmanager::Route* parseOSCRoute(pugi::xml_node routeNode, Entities *entities, Catalogs *catalogs);
		void ParseOSCProperties(OSCProperties &properties, pugi::xml_node &xml_node);
		Vehicle* parseOSCVehicle(pugi::xml_node vehicleNode, Catalogs *catalogs);

		// Enitites
		void parseEntities(Entities &entities, Catalogs *catalogs);
		Object* FindObjectByName(std::string name, Entities *entities);

		// Storyboard - Init
		void parseInit(Init &init, Entities *entities, Catalogs *catalogs);
		OSCPrivateAction *parseOSCPrivateAction(pugi::xml_node actionNode, Entities *entities, Object *object, Catalogs *catalogs);
		void parseOSCOrientation(OSCOrientation &orientation, pugi::xml_node orientationNode);
		OSCPosition *parseOSCPosition(pugi::xml_node positionNode, Entities *entities, Catalogs *catalogs);

		// Storyboard - Story
		OSCCondition *parseOSCCondition(pugi::xml_node conditionNode, Entities *entities, Catalogs *catalogs);
		//	void parseOSCConditionGroup(OSCConditionGroup *conditionGroup, pugi::xml_node conditionGroupNode);
		void parseStory(std::vector<Story*> &storyVector, Entities *entities, Catalogs *catalogs);
		void parseOSCManeuver(OSCManeuver *maneuver, pugi::xml_node maneuverNode, Entities *entities, ActSequence *act_sequence, Catalogs *catalogs);

		// Help functions
		std::string getParameter(std::string name);
		void addParameter(std::string name, std::string value);

		std::string ExtControlMode2Str(ExternalControlMode mode)
		{
			if (mode == ExternalControlMode::EXT_CONTROL_BY_OSC) return "by OSC";
			else if (mode == ExternalControlMode::EXT_CONTROL_OFF) return "Off";
			else if (mode == ExternalControlMode::EXT_CONTROL_ON) return "On";
			else return "Unknown";
		}

	private:
		pugi::xml_document doc;
		OSCParameterDeclaration parameterDeclaration;
		int objectCnt;
		std::string oscFilename;
		ExternalControlMode req_ext_control_;  // Requested Ego (id 0) control mode

		// Use always this method when reading attributes, it will resolve any variables
		std::string ReadAttribute(pugi::xml_attribute attribute, bool required = false);
	};

}
