/* Include the controller definition */
#include "crazyflie_sensing.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
/* Logging */
#include <argos3/core/utility/logging/argos_log.h>


// sockets includes
#include <sys/socket.h>
#include <thread>

// random walk includes
#include <random>

// to send data
#include <string> 

#include <cstdlib>

// for sleep
#include <unistd.h>

#define PORT 8080
#define MIN_BATTERY 30


/****************************************/
/****************************************/

CCrazyflieSensing::CCrazyflieSensing() :
   m_pcDistance(NULL),
   m_pcPropellers(NULL),
   m_pcRNG(NULL),
   m_pcRABA(NULL),
   m_pcRABS(NULL),
   m_pcPos(NULL),
   m_pcBattery(NULL),
   m_uiCurrentStep(0){}
   

/****************************************/
/****************************************/

void CCrazyflieSensing::Init(TConfigurationNode& t_node) {
   try {
      /*
       * Initialize sensors/actuators
       */
      m_pcDistance   = GetSensor  <CCI_CrazyflieDistanceScannerSensor>("crazyflie_distance_scanner");
      m_pcPropellers = GetActuator  <CCI_QuadRotorPositionActuator>("quadrotor_position");
      /* Get pointers to devices */
      m_pcRABA   = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
      m_pcRABS   = GetSensor  <CCI_RangeAndBearingSensor  >("range_and_bearing");
      try {
         m_pcPos = GetSensor  <CCI_PositioningSensor>("positioning");
      }
      catch(CARGoSException& ex) {}
      try {
         m_pcBattery = GetSensor<CCI_BatterySensor>("battery");
      }
      catch(CARGoSException& ex) {}      
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing the crazyflie sensing controller for robot \"" << GetId() << "\"", ex);
   }
   /*
    * Initialize other stuff
    */
   /* Create a random number generator. We use the 'argos' category so
      that creation, reset, seeding and cleanup are managed by ARGoS. */
   m_pcRNG = CRandom::CreateRNG("argos");

   m_uiCurrentStep = 0;
   std::fill_n(FdServers, 10, -1);
   InitializesSockets();
   std::thread readThread(&CCrazyflieSensing::ReadInformation,this);
   readThread.detach();
 
   Reset();
   
   GetNodeAttribute(t_node, "nb_drones",nbDrones);
   
   

}


////////// MULTI CLIENT////////////
void CCrazyflieSensing::InitializesSockets(){
   int server_fd;
	struct sockaddr_in address;
	int opt = 1;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	// Forcefully attaching socket to the port 8081
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	} 
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT + GetDroneNumber());

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
   FdServers_pre[GetDroneNumber()]=server_fd;
   std::thread listenThread(&CCrazyflieSensing::listenForConnection,this);
   listenThread.detach();
   

}

void CCrazyflieSensing::listenForConnection(){
  
   int server_fd = FdServers_pre[GetDroneNumber()];
   int addrlen = sizeof(address);
   int new_socket;

   while(true){

      if ((listen(server_fd, 10)) < 0) {
         perror("listen");
         exit(EXIT_FAILURE);
	   }

	   if ((new_socket= accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) {
         perror("accept");
         exit(EXIT_FAILURE);
	   } 

      FdServers[GetDroneNumber()]=new_socket;
   } 
}


void CCrazyflieSensing::SendInformations(std::string message){
   
   send(FdServers[GetDroneNumber()], message.c_str() , strlen(message.c_str()), 0);
   sleep(0.5);
}
void CCrazyflieSensing::ReadInformation(){
   while(true){
      read(FdServers[GetDroneNumber()], buffer, 1024);
      
   }

}
void CCrazyflieSensing::ControlStep() {
   LOG<<GetBatteryPourcentage()<<std::endl;
  
   int valread;
   //char buffer[1024] = { 0 };
   std::string message ="";
   
     
   //current position of the drone
   CVector3 cPos = m_pcPos->GetReading().Position;

   //number of steps the drone will take to take off 
   const int nInitSteps = 10;

   const CCI_CrazyflieDistanceScannerSensor::TReadingsMap readingsMap = m_pcDistance->GetReadingsMap();          
   sensors.SetSensors(readingsMap); // set leftSensor backSensor rightSensor frontSensor
  
   // if connection from client on socket
   // deal with command sent from the "station au sol" 
      
   if (FdServers[GetDroneNumber()]!=-1){

      if (std::string(buffer)!=""){

         state = std::string(buffer);
            
      }
      
      if(state[0] == 's'){
         if (nb < 1 ){
            SendInformations(std::to_string(nbDrones));
            nb++;
         }
         
      }
      if (state[0] == 't' && GetBatteryPourcentage()> MIN_BATTERY) {
         
         started = true;
         LOG << "Take off : " << std::to_string(GetDroneNumber())<<std::endl;
         
         // print current position
         LOG<<" X: " << cPos.GetX() << " Y: " << cPos.GetY() << " Z: " << cPos.GetZ() << std::endl;
         
     
         if ( m_uiCurrentStep < nInitSteps ) {
            
            TakeOff();

            m_cGoToPosition = RandomPosition(); //generate random position to strat mission
         
            LOG<<" FIRST RANDOM POSITON X :"<< m_cGoToPosition.GetX()<<" Y: "<< m_cGoToPosition.GetY()<<std::endl;
            
            if(m_uiCurrentStep==(nInitSteps-1)){
               positions.push_back(cPos);
            }
            
         }  else {
         
            // random walk

            if (sensors.ObjectDetected()){
               MoveTo(cPos);
               m_cGoToPosition = sensors.FollowObstacle(cPos,m_cGoToPosition);
               LOG<<" GO TO POSITION  X :"<< m_cGoToPosition.GetX()<<" Y: "<< m_cGoToPosition.GetY()<<" Z: "<< m_cGoToPosition.GetZ()<<std::endl;
               MoveTo(m_cGoToPosition);
            } 

         
            // verifie if the drone arrived to the position 
            if (abs(m_cGoToPosition.GetX() - cPos.GetX())< 0.03f && abs(m_cGoToPosition.GetY() - cPos.GetY()) < 0.03f ){
               
               LOG<<" ARRIVED !! "<< std::endl;

               // if no object is detected generate a random position to go to
               if (!(sensors.ObjectDetected())){

                  m_cGoToPosition = RandomPosition();
                  LOG<< "GENERATING RANDOM POSITION "<<std::endl;
               }  
      
   

            }
            positions.push_back(m_cGoToPosition);
            LOG<<"go to position "<< std::to_string(GetDroneNumber())<< "   " <<m_cGoToPosition<<std::endl;
            // go to specified position 
            MoveTo(m_cGoToPosition);
            
            
         }
         m_uiCurrentStep++;

         message= "/EnMission,"+std::to_string(GetDroneNumber())+","+std::to_string(cPos.GetX())+","+std::to_string(cPos.GetY())+","+std::to_string(cPos.GetZ())+"," + std::to_string(sensors.leftSensor)+"," + std::to_string(sensors.backSensor)+"," + std::to_string(sensors.rightSensor)+"," + std::to_string(sensors.frontSensor)+","+std::to_string(GetBatteryPourcentage())+"," ;
         SendInformations(message);
      } 
      if ( state[0] == 'l') {
        
         LOG << "Landing : " << std::to_string(GetDroneNumber())<<std::endl;
         LOG<<" X: " << cPos.GetX() << " Y: " << cPos.GetY() << " Z: " << cPos.GetZ() << std::endl;
         Land();
         message= "/Atterrissage,"+std::to_string(GetDroneNumber())+","+std::to_string(cPos.GetX())+","+std::to_string(cPos.GetY())+","+std::to_string(cPos.GetZ())+"," + std::to_string(sensors.leftSensor)+"," + std::to_string(sensors.backSensor)+"," + std::to_string(sensors.rightSensor)+"," + std::to_string(sensors.frontSensor)+","+std::to_string(GetBatteryPourcentage())+"," ;
         SendInformations(message);
      } 
      if ( (state[0] == 'b') || (state[0] == 't' && GetBatteryPourcentage()< MIN_BATTERY&& started) ) { 
         LOG<<"retour base " << std::to_string(GetDroneNumber())<<std::endl;
         message = "/RetourBase,"+std::to_string(GetDroneNumber())+","+std::to_string(cPos.GetX())+","+std::to_string(cPos.GetY())+","+std::to_string(cPos.GetZ())+"," + std::to_string(sensors.leftSensor)+"," + std::to_string(sensors.backSensor)+"," + std::to_string(sensors.rightSensor)+"," + std::to_string(sensors.frontSensor)+","+std::to_string(GetBatteryPourcentage())+"," ;
         SendInformations(message);
         ReturnToBase(cPos);
         
      }

   } 
 
}

/****************************************/
/****************************************/
int CCrazyflieSensing::GetBatteryPourcentage(){
   
   return m_pcBattery->GetReading().AvailableCharge*100;

}


void CCrazyflieSensing::ReturnToBase(CVector3 cPos){
   
   LOG<<"size"<<positions.size()<<std::endl;
   if (positions.size()>0) {
      m_cGoToPosition = *(positions.end()-1);
      LOG<<"return position "<<m_cGoToPosition<<std::endl;
      MoveTo(m_cGoToPosition);
      positions.pop_back();
   }

      

   if (abs(m_cGoToPosition.GetX() - cPos.GetX())< 0.03f && abs(m_cGoToPosition.GetY() - cPos.GetY()) < 0.03f ){
      LOG<< "arrived to base !!!"<<std::endl;
      Land();
   }
  
   LOG<<" X: " << cPos.GetX() << " Y: " << cPos.GetY() << " Z: " << cPos.GetZ() << std::endl;
 

}

int CCrazyflieSensing::GetDroneNumber(){
   return std::stoi(GetId().substr(1));
}
void CCrazyflieSensing::MoveTo(CVector3 finalPos) {

   m_pcPropellers->SetAbsolutePosition(finalPos);

}

CVector3  CCrazyflieSensing::RandomPosition(){
   ///https://stackoverflow.com/questions/7560114/random-number-c-in-some-range

   CVector3 cPos = m_pcPos->GetReading().Position;

   std::random_device rd; // obtain a random number from hardware
   std::mt19937 gen(rd()); // seed the generator
   std::uniform_real_distribution<> distr(-1.0,1.0 );

   
   CVector3 add(distr(gen),distr(gen),cPos.GetZ());

   //return position
   return cPos + add;
}


/****************************************/
/****************************************/

bool CCrazyflieSensing::TakeOff() {
   CVector3 cPos = m_pcPos->GetReading().Position;
   if(Abs(cPos.GetZ() - (2.0f + (GetDroneNumber()*2))) < 0.01f) return false;
   cPos.SetZ(2.0f+(GetDroneNumber()*2));
   m_pcPropellers->SetAbsolutePosition(cPos);
   return true;
}

/****************************************/
/****************************************/

bool CCrazyflieSensing::Land() {
   CVector3 cPos = m_pcPos->GetReading().Position;
   if(Abs(cPos.GetZ()) < 0.01f) return false;
   cPos.SetZ(0.0f);
   m_pcPropellers->SetAbsolutePosition(cPos);
   return true;
}

/****************************************/
/****************************************/

void CCrazyflieSensing::Reset() {

}

/****************************************/
/****************************************/

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the XML configuration file to refer to
 * this controller.
 * When ARGoS reads that string in the XML file, it knows which controller
 * class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CCrazyflieSensing, "crazyflie_sensing_controller")
