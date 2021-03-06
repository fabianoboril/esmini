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

 /*
  * The purpose of this application is to support development of the RoadManager by visualizing the road network and moving objects on top.
  * Bascially it loads an OpenDRIVE file, and optionally a corresponding 3D model, and then populate vehicles at specified density. The 
  * vehicles will simply follow it's lane until a potential junction where the choice of route is randomized.
  *
  * The application can be used both to debug the RoadManager and to check OpenDRIVE files, e.g. w.r.t. gemoetry, lanes and connectivity.
  *
  * New road/track segments is indicated by a yellow large dot. Geometry segments within a road are indicated by red dots.
  * Red line is the reference lane, blue lines shows drivable lanes. Non-drivable lanes are currently not indicated. 
  */

#include <random>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "viewer.hpp"
#include "RoadManager.hpp"
#include "vehicle.hpp"
#include "CommonMini.hpp"


#define DEFAULT_SPEED   70  // km/h
#define DEFAULT_DENSITY 1   // Cars per 100 m
#define ROAD_MIN_LENGTH 30
#define SIGN(X) ((X<0)?-1:1)


static const double stepSize = 0.01;
static const double maxStepSize = 0.1;
static const double minStepSize = 0.01;
static const bool freerun = true;
static std::mt19937 mt_rand;
static double density = DEFAULT_DENSITY;
static double speed = DEFAULT_SPEED;

double deltaSimTime;  // external - used by Viewer::RubberBandCamera

typedef struct
{
	int road_id_init;
	int lane_id_init;
	roadmanager::Position *pos;
	double speed;  // Velocity along road reference line, m/s
	viewer::CarModel *model;
	int id;
} Car;

std::vector<Car*> cars;

int SetupCars(roadmanager::OpenDrive *odrManager, viewer::Viewer *viewer)
{
	if (density < 1E-10)
	{
		// Basically no scenario vehicles
		return 0;
	}

	for (int r = 0; r < odrManager->GetNumOfRoads(); r++)
	{
		roadmanager::Road *road = odrManager->GetRoadByIdx(r);
		roadmanager::LaneSection *lane_section = road->GetLaneSectionByIdx(0);

		double average_distance = 100.0 / density;

		if (road->GetLength() > ROAD_MIN_LENGTH)
		{
			for (int l = 0; l < lane_section->GetNumberOfLanes(); l++)
			{
				int lane_id = lane_section->GetLaneIdByIdx(l);
				roadmanager::Lane *lane = lane_section->GetLaneById(lane_id);

				if (lane->IsDriving())
				{
					for (double s = 0; s < road->GetLength() - average_distance;)
					{
						int carModelID;

						// Higher speeds in lanes closer to reference lane
						double lane_speed = speed * (0.9 + 0.7*(1.0 / abs(lane_id)));

						// left lanes reverse direction
						double s_aligned = lane->GetId() > 0 ? road->GetLength() - s : s;

						// randomly choose model
						carModelID = (double(viewer->carModels_.size()) * mt_rand()) / (mt_rand.max)();
						LOG("Adding car of model %d to road %d", carModelID, r);

						Car *car_ = new Car;
						car_->road_id_init = odrManager->GetRoadByIdx(r)->GetId();
						car_->lane_id_init = lane_id;						
						car_->pos = new roadmanager::Position(odrManager->GetRoadByIdx(r)->GetId(), lane_id, s_aligned, 0);
						car_->model = viewer->AddCar(carModelsFiles_[carModelID]);
						car_->speed = lane_speed;
						car_->id = cars.size();
						cars.push_back(car_);

						// Add space to next vehicle
						s += average_distance + (0.2 * average_distance * mt_rand()) / (mt_rand.max)();
					}
				}
			}
		}
	}

	return 0;
}

void updateCar(roadmanager::OpenDrive *odrManager, Car *car, double deltaSimTime)
{
	double ds = car->speed * deltaSimTime; // right lane is < 0 in road dir;

	if (car->pos->MoveAlongS(ds) != 0)
	{
		// invalid move -> reset position
		double s;
		if (car->lane_id_init > 0)
		{
			s = odrManager->GetRoadById(car->road_id_init)->GetLength();
		}
		else
		{
			s = 0;
		}
//		printf("Reset pos rid: %d lid: %d\n", car->road_id_init, car->lane_id_init);
		car->pos->SetLanePos(car->road_id_init, car->lane_id_init, s, 0, 0);
	}

	if (car->model->txNode_ != 0)
	{
		double heading = car->pos->GetH();
		double pitch = car->pos->GetP();

		car->model->txNode_->setPosition(osg::Vec3(car->pos->GetX(), car->pos->GetY(), car->pos->GetZ()));

		car->model->quat_.makeRotate(
			car->pos->GetR(), osg::Vec3(1, 0, 0),
			pitch, osg::Vec3(0, 1, 0),
			heading, osg::Vec3(0, 0, 1));

		car->model->txNode_->setAttitude(car->model->quat_);
	}
}

int main(int argc, char** argv)
{
	mt_rand.seed(time(0));

	// use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);	

    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName());
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options]\n");
	arguments.getApplicationUsage()->addCommandLineOption("--odr <filename>", "OpenDRIVE filename");
	arguments.getApplicationUsage()->addCommandLineOption("--model <filename>", "3D model filename");
	arguments.getApplicationUsage()->addCommandLineOption("--density <number>", "density (cars / 100 m)", std::to_string((long long) (DEFAULT_DENSITY)));
	arguments.getApplicationUsage()->addCommandLineOption("--speed <number>", "speed (km/h)", std::to_string((long long) (DEFAULT_SPEED)));

	if (arguments.argc() < 2)
	{
		arguments.getApplicationUsage()->write(std::cout, 1, 120, true);
		return -1;
	}

	std::string odrFilename;
	arguments.read("--odr", odrFilename);

	std::string modelFilename;
	arguments.read("--model", modelFilename);

	arguments.read("--density", density);
	printf("density: %.2f\n", density);

	arguments.read("--speed", speed);
	printf("speed: %.2f\n", speed);
	speed /= 3.6;

	roadmanager::Position *lane_pos = new roadmanager::Position();
	roadmanager::Position *track_pos = new roadmanager::Position();

	try
	{
		if (!roadmanager::Position::LoadOpenDrive(odrFilename.c_str()))
		{
			printf("Failed to load ODR %s\n", odrFilename.c_str());
			return -1;
		}
		roadmanager::OpenDrive *odrManager = roadmanager::Position::GetOpenDrive();

		viewer::Viewer *viewer = new viewer::Viewer(
			odrManager, 
			modelFilename.c_str(),
			arguments);

		SetupCars(odrManager, viewer);
		printf("%d cars added\n", (int)cars.size());

		__int64 now, lastTimeStamp = 0;

		while (!viewer->osgViewer_->done())
		{
			// Get milliseconds since Jan 1 1970
			now = SE_getSystemTime();
			deltaSimTime = (now - lastTimeStamp) / 1000.0;  // step size in seconds
			lastTimeStamp = now;
			if (deltaSimTime > maxStepSize) // limit step size
			{
				deltaSimTime = maxStepSize;
			}
			else if (deltaSimTime < minStepSize)  // avoid CPU rush, sleep for a while
			{
				SE_sleep(now - lastTimeStamp);
				deltaSimTime = minStepSize;
			}

			for (size_t i=0; i<cars.size(); i++)
			{
				updateCar(odrManager, cars[i], deltaSimTime);
			}
			//if (cars.size() > 0)
			//{
			//	printf("Curvature: %.6f\n", cars[0]->pos->GetCurvature());
			//}

			viewer->osgViewer_->frame();
		}
	}
	catch (std::logic_error &e)
	{
		printf("%s\n", e.what());
		return 2;
	}
	catch (std::runtime_error &e)
	{
		printf("%s\n", e.what());
		return 3;
	}

	for (size_t i = 0; i < cars.size(); i++)
	{
		delete(cars[i]);
	}

	delete track_pos;
	delete lane_pos;

	return 0;
}
