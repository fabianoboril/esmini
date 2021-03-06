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

#ifndef VIEWER_HPP_
#define VIEWER_HPP_

#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgGA/NodeTrackerManipulator>
#include <osg/MatrixTransform>
#include <string>

#include "RubberbandManipulator.hpp"
#include "RoadManager.hpp"

static const char* carModelsFiles_[] =
{
//	"s90.fbx",
	"car_white.osgb",
	"car_blue.osgb",
	"car_red.osgb",
	"car_yellow.osgb",
//	"truck_blue.osgb",
//	"truck_red.osgb",
	"truck_yellow.osgb",
//	"van_blue.osgb",
	"van_red.osgb",
//	"van_yellow.osgb",
	"bus_blue.osgb",
//	"bus_red.osgb",
//	"bus_yellow.osgb",
};

namespace viewer
{
	class CarModel
	{
	public:
		osg::ref_ptr<osg::LOD> node_;
		osg::ref_ptr<osg::PositionAttitudeTransform> txNode_;
		std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> wheel_;
		osg::ref_ptr<osg::LOD> model;
		osg::Quat quat_;
		double size_x;
		double size_y;
		double center_x;
		double center_y;
		std::string filename_;

		CarModel(osg::ref_ptr<osg::LOD> lod);
		~CarModel();
		void SetPosition(double x, double y, double z);
		void SetRotation(double h, double p, double r);
		void UpdateWheels(double wheel_angle, double wheel_rotation);

		osg::ref_ptr<osg::PositionAttitudeTransform>  AddWheel(osg::ref_ptr<osg::Node> carNode, const char *wheelName);
	};

	class Viewer
	{
	public:
		int currentCarInFocus_;
		int camMode_;
		osg::ref_ptr<osg::Group> line_node_;
		
		// Driver model steering debug visualization
		osg::ref_ptr<osg::Geometry> dm_steering_target_line_;
		osg::ref_ptr<osg::Vec3Array> dm_steering_target_line_vertexData_;
		osg::ref_ptr<osg::Geometry> dm_steering_target_point_;
		osg::ref_ptr<osg::Vec3Array> dm_steering_target_point_data_;

		// Vehicle position debug visualization
		osg::ref_ptr<osg::Node> shadow_node_;
		osg::ref_ptr<osg::Vec3Array> vertexData;
		osg::ref_ptr<osg::Geometry> vehicleLine_;
		osg::ref_ptr<osg::Vec3Array> pointData;
		osg::ref_ptr<osg::Geometry> vehiclePoint_;

		// Road debug visualization
		osg::ref_ptr<osg::Group> odrLines_;
		osg::ref_ptr<osg::PositionAttitudeTransform> envTx_;
		osg::ref_ptr<osg::Node> environment_;
		osg::ref_ptr<osgGA::RubberbandManipulator> rubberbandManipulator_;
		osg::ref_ptr<osgGA::NodeTrackerManipulator> nodeTrackerManipulator_;
		std::vector<CarModel*> cars_;
		float lodScale_;
		osgViewer::Viewer *osgViewer_;
		osg::MatrixTransform* rootnode_;
		roadmanager::OpenDrive *odrManager_;
		std::vector<osg::ref_ptr<osg::LOD>> carModels_;

		Viewer(roadmanager::OpenDrive *odrManager, const char *modelFilename, osg::ArgumentParser arguments, bool create_ego_debug_lines = false);
		~Viewer();
		CarModel* AddCar(std::string modelFilepath);
		int AddEnvironment(const char* filename);
		osg::ref_ptr<osg::LOD> LoadCarModel(const char *filename);
		void UpdateDriverModelPoint(roadmanager::Position *pos, double distance);
		void UpdateVehicleLineAndPoints(roadmanager::Position *pos);
		void setKeyUp(bool pressed) { keyUp_ = pressed; }
		void setKeyDown(bool pressed) { keyDown_ = pressed; }
		void setKeyLeft(bool pressed) { keyLeft_ = pressed; }
		void setKeyRight(bool pressed) { keyRight_ = pressed; }
		bool getKeyUp() { return keyUp_; }
		bool getKeyDown() { return keyDown_; }
		bool getKeyLeft() { return keyLeft_; }
		bool getKeyRight() { return keyRight_; }
		void SetQuitRequest(bool value) { quit_request_ = value; }
		bool GetQuitRequest() { return quit_request_;  }

	private:

		std::string modelFilename_;

		bool ReadCarModels();
		bool CreateRoadLines(roadmanager::OpenDrive* od, osg::Group* parent);
		bool CreateVehicleLineAndPoint(osg::Group* parent);
		bool CreateDriverModelLineAndPoint(osg::Group* parent);
		bool keyUp_;
		bool keyDown_;
		bool keyLeft_;
		bool keyRight_;
		bool quit_request_;
	};

	class KeyboardEventHandler : public osgGA::GUIEventHandler
	{
	public:
		KeyboardEventHandler(Viewer *viewer) : viewer_(viewer) {}
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);

	private:
		Viewer * viewer_;
	};
}



#endif  // VIEWER_HPP_

