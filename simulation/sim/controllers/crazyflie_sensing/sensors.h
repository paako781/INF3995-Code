
#ifndef SENSORS_H
#define SENSORS_H
/*
 * Include some necessary headers.
 */
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the crazyflie distance sensor */
#include <argos3/plugins/robots/crazyflie/control_interface/ci_crazyflie_distance_scanner_sensor.h>
/* Definition of the crazyflie position actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
/* Definition of the crazyflie position sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
/* Definition of the crazyflie range and bearing actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
/* Definition of the crazyflie range and bearing sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
/* Definition of the crazyflie battery sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>
/* Definitions for random number generation */
#include <argos3/core/utility/math/rng.h>

#include <arpa/inet.h> 
/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;

class Sensors {


    public:
    
    /* Class constructor. */
    Sensors();

    /* Class destructor. */
    virtual ~Sensors() {}

    /*
    * this function sets the set leftSensor backSensor rightSensor frontSensor
    */
    void SetSensors(const CCI_CrazyflieDistanceScannerSensor::TReadingsMap readingsMap);
    
    /*
    * this function returns true if an object it detected false if not
    */
    bool ObjectDetected();

    /*
    * this function returns true if an object it detected from the left sensor false if not
    */
    bool ObjectDetectedLeft();

    /*
    * this function returns true if an object it detected from the back sensor false if not
    */
    bool ObjectDetectedBack();

    /*
    * this function returns true if an object it detected from the right sensor false if not
    */
    bool ObjectDetectedRight();

    /*
    * this function returns true if an object it detected from the front sensor false if not
    */
    bool ObjectDetectedFront();

    /*
    * this function implementes the random walk algorithme.
    * the drone avoid obsatacles and follow walls
    */
    CVector3 FollowObstacle(CVector3 cPos,CVector3 goToPosition );

    

    double leftSensor;
    double backSensor;
    double rightSensor;
    double frontSensor;

};
#endif