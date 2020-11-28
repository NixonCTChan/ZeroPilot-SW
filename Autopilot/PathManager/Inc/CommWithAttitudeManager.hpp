/**
 * Communicates with the attitude manager to send commands and receive useful data.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_ATTITUDE_MANAGER_HPP
#define COMM_WITH_ATTITUDE_MANAGER_HPP

extern "C"
{
#include "cmsis_os.h"
}

//Commands to send to attitude manager.
typedef struct CommandsForAM{
	float roll,pitch,yaw;	// commanded orientation (radians)
	float airspeed;			// commanded airspeed (m/s)
} CommandsForAM;

//Data to receive from attitude manager.
typedef struct AttitudeData{
	float roll,pitch,yaw;	// current orientation (radians)
	float airspeed;			// current airspeed (m/s)
	float accX, accY, accZ; // accelerometer data
	float gyrX, gyrY, gyrZ; // gyroscope data
} AttitudeData;

//Set up a mail queue for sending commands to the attitude manager
const uint8_t COMMANDS_MAIL_Q_SIZE = 10;
osMailQDef(commandsMailQ, COMMANDS_MAIL_Q_SIZE, CommandsForAM);
osMailQId commandsMailQ;

/**
 * Initiate communication with the attitude manager.
 */
void CommWithAMInit();

/**
 * Sends commands to the attitude manager.
 * @param[in]   Commands   Pointer to struct with commands.
 */
void SendCommandsForAM(CommandsForAM *Commands);

/**
 * Retrieves attitude/airspeed data from attitude manager.
 * @return Attitude/airspeed data.
 */
AttitudeData GetAttitudeData();

#endif