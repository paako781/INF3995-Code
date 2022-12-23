//Include the controller definition 
#include "sensors.h"
//Function definitions for XML parsing 
#include <argos3/core/utility/configuration/argos_configuration.h>
//2D vector definition 
#include <argos3/core/utility/math/vector2.h>
// Logging 
#include <argos3/core/utility/logging/argos_log.h>

#define OBSTACLE_DETECTION 55.0
#define DELTA 0.04
#define SMALL_DELTA 0.02

Sensors::Sensors() : 
    leftSensor(0),
    backSensor(0),
    rightSensor(0),
    frontSensor(0){}




void Sensors::SetSensors(const CCI_CrazyflieDistanceScannerSensor::TReadingsMap readingsMap){

    CCI_CrazyflieDistanceScannerSensor::TReadingsMap::const_iterator  it = readingsMap.begin();
   

    if (readingsMap.size()==4){
      
      // left sensor
      leftSensor = (it++)->second;

      // back sansor
      backSensor = (it++)->second;

      // right sensor
      rightSensor = (it++)->second;

      // front sensor
      frontSensor = (it++)->second;
      }

}


bool Sensors::ObjectDetectedLeft(){

   return leftSensor!=-2 && leftSensor < OBSTACLE_DETECTION;
}
bool Sensors::ObjectDetectedBack(){
   
   return backSensor!=-2 && backSensor < OBSTACLE_DETECTION;
}

bool Sensors::ObjectDetectedRight(){
   
   return rightSensor!=-2 && rightSensor < OBSTACLE_DETECTION;
}

bool Sensors::ObjectDetectedFront(){
   
   return frontSensor!=-2 && frontSensor < OBSTACLE_DETECTION;
}


bool Sensors::ObjectDetected(){

   return ObjectDetectedLeft() || ObjectDetectedBack() || ObjectDetectedRight() || ObjectDetectedFront();

} 

CVector3 Sensors::FollowObstacle(CVector3 cPos, CVector3 goToPosition){

   if ((ObjectDetectedLeft() && !ObjectDetectedBack() && 
      !ObjectDetectedRight() && !ObjectDetectedFront())||
      (ObjectDetectedLeft() && !ObjectDetectedBack() && 
      ObjectDetectedRight() && ObjectDetectedFront())||
      (ObjectDetectedLeft()&&ObjectDetectedRight())) {

      LOG<< "!!!!!!! GO BACK !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX() + DELTA, cPos.GetY()+SMALL_DELTA, cPos.GetZ());
      //return CVector3(cPos.GetX() + DELTA, goToPosition.GetY(), goToPosition.GetZ());

   }else if (!ObjectDetectedLeft() && ObjectDetectedBack() && 
      !ObjectDetectedRight() && !ObjectDetectedFront()||
      (!ObjectDetectedLeft() && ObjectDetectedBack() && 
      ObjectDetectedRight() && ObjectDetectedFront())){

      LOG<< "!!!!!!! GO RIGHT !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX()-SMALL_DELTA,cPos.GetY() + DELTA,cPos.GetZ());
      //return CVector3(goToPosition.GetX(),cPos.GetY() + DELTA,goToPosition.GetZ());

   }else if (!ObjectDetectedLeft() && !ObjectDetectedBack() && 
      ObjectDetectedRight() && !ObjectDetectedFront()||
      (ObjectDetectedLeft() && ObjectDetectedBack() && 
      ObjectDetectedRight() && !ObjectDetectedFront())){

      LOG<< "!!!!!!! GO FRONT !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX() - DELTA,cPos.GetY()-SMALL_DELTA,cPos.GetZ());
      //return CVector3(cPos.GetX() - DELTA,goToPosition.GetY(),goToPosition.GetZ());
      

   } else if(!ObjectDetectedLeft() && !ObjectDetectedBack() && 
      !ObjectDetectedRight() && ObjectDetectedFront()||
      (!ObjectDetectedLeft() && ObjectDetectedBack() && 
      !ObjectDetectedRight() && ObjectDetectedFront())){
      
      LOG<< "!!!!!!! GO LEFT !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX()+SMALL_DELTA,cPos.GetY() - DELTA,cPos.GetZ());
      //return CVector3(goToPosition.GetX(),cPos.GetY() - DELTA,goToPosition.GetZ());

   } else if (ObjectDetectedLeft() && ObjectDetectedBack() && 
      !ObjectDetectedRight() &&!ObjectDetectedFront()){

      LOG<< "!!!!!!! LEFT - BACK !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX() - DELTA ,cPos.GetY() + DELTA,cPos.GetZ());
      //return CVector3(goToPosition.GetX() - DELTA ,cPos.GetY() + DELTA,goToPosition.GetZ());

   } else if (ObjectDetectedLeft() && !ObjectDetectedBack() && 
      !ObjectDetectedRight() && ObjectDetectedFront()){

      LOG<< "!!!!!!! LEFT - FRONT !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX() + DELTA ,cPos.GetY() + DELTA,cPos.GetZ());
      //return CVector3(cPos.GetX() + DELTA ,cPos.GetY() + DELTA,goToPosition.GetZ());
   }
   else if (!ObjectDetectedLeft() && ObjectDetectedBack() && 
      ObjectDetectedRight() && !ObjectDetectedFront()){

      LOG<< "!!!!!!! RIGHT - BACK !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX() - DELTA ,cPos.GetY() - DELTA,cPos.GetZ());
      //return CVector3(cPos.GetX() - DELTA ,cPos.GetY() - DELTA,goToPosition.GetZ());
   }
   else if (!ObjectDetectedLeft() && !ObjectDetectedBack() && 
      ObjectDetectedRight() && ObjectDetectedFront()){

      LOG<< "!!!!!!! RIGHT - FRONT !!!!!!!!!"<< std::endl;
      return CVector3(cPos.GetX() + DELTA ,cPos.GetY() - DELTA,cPos.GetZ());
      //return CVector3(cPos.GetX() + DELTA ,cPos.GetY() - DELTA,goToPosition.GetZ());
   }
}