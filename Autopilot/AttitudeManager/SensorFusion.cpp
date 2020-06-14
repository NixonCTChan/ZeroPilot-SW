/*
* Sensor Fusion Algorithms - uses Madgwick Algorithm to fuse IMU data
* then converts the quaternion representation into Yaw/Pitch/Roll angles
* Author: Lucy Gong
*/
#include "SensorFusion.hpp"
#include "IMU.hpp"
#include "airspeed.hpp"
//#include "IMU_Mock.hpp"
//#include "airspeed_Mock.hpp"
#include "MadgwickAHRS.h"
#include <math.h>

IMUData_t imudata;
airspeedData_t airspeeddata;

IMU imusns;
airspeed airspeedsns;

SFError_t SF_GetResult(SFOutput_t *Output){
    
    //Error output
    SFError_t SFError;

    SFError.errorCode = 0;

    //IMU integration outputs
    float imu_RollAngle = 0;
    float imu_PitchAngle = 0;
    float imu_YawAngle = 0;

    float imu_RollRate = 0;
    float imu_PitchRate = 0;
    float imu_YawRate = 0;

    //Retrieve raw IMU and Airspeed data
    imusns.GetResult(&imudata);
    airspeedsns.GetResult(&airspeeddata);

    //Abort if both sensors are busy or failed data collection
    if(imudata.sensorStatus != 0 && airspeeddata.sensorStatus != 0)
    {
        SFError.errorCode = -1;
        return SFError;
    }

    //Check if data is old
    if(!imudata.isDataNew || !airspeeddata.isDataNew){
        SFError.errorCode = 1;
    }

    //Madgwick Algorithm for raw IMU Data
    if(imudata.sensorStatus == 0){

        MadgwickAHRSupdate(imudata.gyrx, imudata.gyry, imudata.gyrz, imudata.accx, imudata.accy, imudata.accz, imudata.magx, imudata.magy, imudata.magz);

        //Convert quaternion output to angles (in deg)
        imu_RollAngle = atan2f(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2) * 57.29578f;
        imu_PitchAngle = asinf(-2.0f * (q1 * q3 - q0 * q2)) * 57.29578f;
        imu_YawAngle = atan2f(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3) * 57.29578f + 180.0f;

        //Convert rate of change of quaternion to angular velocity (in deg/s)
        imu_RollRate = atan2f(qDot1 * qDot2 + qDot3 * qDot4, 0.5f - qDot2 * qDot2 - qDot3 * qDot3) * 57.29578f;
        imu_PitchRate = asinf(-2.0f * (qDot2 * qDot4 - qDot1 * qDot3)) * 57.29578f;
        imu_YawRate = atan2f(qDot2 * qDot3 + qDot1 * qDot4, 0.5f - qDot3 * qDot3 - qDot4 * qDot4) * 57.29578f + 180.0f;

        //Transfer Fused IMU data into SF Output struct
        Output->IMUpitch = imu_PitchAngle;
        Output->IMUroll = imu_RollAngle;
        Output->IMUyaw = imu_YawAngle;

        Output->IMUpitchrate = imu_PitchRate;
        Output->IMUrollrate = imu_RollRate;
        Output->IMUyawrate = imu_YawRate;
    }
    else{
        SFError.errorCode = 2;
    }

    //Transfer Airspeed data if ready
    if(airspeeddata.sensorStatus == 0){
        Output->Airspeed = airspeeddata.airspeed;
    }
    else{
        SFError.errorCode = 3;
    }
    

    return SFError;
}

