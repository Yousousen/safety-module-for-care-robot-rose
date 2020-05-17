////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "RTIMULib.h"
#include <math.h>

// yaw = z
// pitch = x
// roll = y

void fuseToCartesian(RTVector3& fuse, double* cart) {
    double yaw = fuse.z();
    double pitch = fuse.x();
    double roll = fuse.y();

    double R[3][3] = {
    {
        cos(pitch)*cos(yaw), -cos(roll)*sin(yaw) + sin(roll)*sin(pitch)*cos(yaw),
        sin(roll)*sin(yaw) + cos(roll)*sin(pitch)*cos(yaw)
    },
    {
        cos(pitch)*sin(yaw), cos(roll)*cos(yaw) + sin(roll)*sin(pitch)*sin(yaw),
      -sin(roll)*cos(yaw) + cos(roll)*sin(pitch)*sin(yaw)
    },
    {
        -sin(pitch), sin(roll)*cos(pitch), cos(roll)*cos(pitch)
    }
    };

    // Matrix multiplication
    cart[0] = pitch * R[0][0] + pitch * R[1][0] + pitch * R[2][0];
    cart[1] = roll * R[0][1] + roll * R[1][1] + roll * R[2][1];
    cart[2] = yaw * R[0][2] + yaw * R[1][2] + yaw * R[2][2];
}

//void fuseToCartesian(RTVector3& fuse, double* cart) {
//    double yaw = fuse.z();
//    double pitch = fuse.x();
//    double roll = fuse.y();
//
////    double R[3][3] = {
////    {
////        cos(pitch)*cos(yaw), -cos(roll)*sin(yaw) + sin(roll)*sin(pitch)*cos(yaw),
////        sin(roll)*sin(yaw) + cos(roll)*sin(pitch)*cos(yaw)
////    },
////    {
////        cos(pitch)*sin(yaw), cos(roll)*cos(yaw) + sin(roll)*sin(pitch)*sin(yaw),
////      -sin(roll)*cos(yaw) + cos(roll)*sin(pitch)*sin(yaw)
////    },
////    {
////        -sin(pitch), sin(roll)*cos(pitch), cos(roll)*cos(pitch)
////    }
////    };
//
//    // x
//    //cart[0] = cos(yaw) * cos(pitch);
//    // y
//    //cart[1] = sin(yaw) * cos(pitch);
//    // z
//    //cart[2] = sin(pitch);
//    
//    // x
//    cart[0] = -cos(yaw) * sin(pitch) * sin(roll) * -sin(yaw) * cos(roll);
//    // y
//    cart[1] = -sin(yaw) * sin(pitch) * sin(roll) * +cos(yaw) * cos(roll);
//    // z
//    cart[2] =  cos(pitch) * sin(roll);
//
////    double gravity = 9.81;
////    cart[2] -= gravity;
//}

void compensate_gravity(RTIMU_DATA& imuData) {
    //double accel[3]={0.1,0,1.0};// data will come from the accelerometers
    const double x = imuData.accel.x();
    const double y = imuData.accel.y();
    const double z = imuData.accel.z();
    double accel[3]={x,y,z};// data will come from the accelerometers
    //double gravity[3]={0,0,1.0};// always vertically downwards at g = 1.0
    double gravity[3]={0,0,1.3};// always vertically downwards at g = 1.0
    double rG[3],rA[3];
    double mA[3];

    // Angle of rotation will come from RTImuLib
//    double alpha= 0.1;// some aribitar values for testing
//    double beta=  0.2;// some aribitar values for testing
//    double theta = 0.3;// some aribitar values for testing
    double alpha= imuData.fusionPose.x();// some aribitar values for testing
    double beta=  imuData.fusionPose.y();// some aribitar values for testing
    double theta = imuData.fusionPose.z();// some aribitar values for testing

    double R[3][3] = {
        { cos(alpha)*cos(beta) , cos(alpha)*sin(beta)*sin(theta) - sin(alpha)*cos(theta) , cos(alpha)*sin(beta)*cos(theta) + sin(alpha)*sin(theta)},
        { sin(alpha)*cos(beta) , sin(alpha)*sin(beta)*sin(theta) + cos(alpha)*cos(theta) , sin(alpha)*sin(beta)*cos(theta) - cos(alpha)*sin(theta)},
        {     -1* sin(beta)    ,                  cos(beta) * sin(theta)                 ,               cos(beta) * cos(theta)                   }
    };
    double det = +R[0][0]*(R[1][1]*R[2][2]-R[1][2]*R[2][1])\
                 -R[0][1]*(R[1][0]*R[2][2]-R[1][2]*R[2][0])\
                 +R[0][2]*(R[1][0]*R[2][1]-R[1][1]*R[2][0]);

    printf(" det= %f\r\n",det);  //for check, should be +1 exactly

    rG[0]= gravity[0]*R[0][0] + gravity[1]*R[0][1] + gravity[3]*R[0][2] ;
    rG[1]= gravity[1]*R[1][0] + gravity[1]*R[1][1] + gravity[3]*R[1][2] ;
    rG[2]= gravity[2]*R[2][0] + gravity[1]*R[2][1] + gravity[3]*R[2][2] ;

    rA[0]= accel[0]*R[0][0] + accel[1]*R[0][1] + accel[3]*R[0][2] ;
    rA[1]= accel[1]*R[1][0] + accel[1]*R[1][1] + accel[3]*R[1][2] ;
    rA[2]= accel[2]*R[2][0] + accel[1]*R[2][1] + accel[3]*R[2][2] ;

    mA[0]=rA[0]-rG[0];
    mA[1]=rA[1]-rG[1];
    mA[2]=rA[2]-rG[2];

    printf("A-G %8.5f %8.5f %8.5f\r\n",mA[0],mA[1],mA[2]);

}

// compensate the accelerometer readings from gravity. 
// @param q the quaternion representing the orientation of a 9DOM MARG sensor array
// @param acc the readings coming from an accelerometer expressed in g
//
// @return a 3d vector representing dinamic acceleration expressed in gravity
void gravity_compensate(RTIMU_DATA& imuData, double* res) {
      double g[] = {0.0, 0.0, 0.0};
      const double x = imuData.accel.x();
      const double y = imuData.accel.y();
      const double z = imuData.accel.z();
      double acc[3]={x,y,z}; // data will come from the accelerometers.

      const double scalar = imuData.fusionQPose.scalar();
      const double qx = imuData.fusionQPose.x();
      const double qy = imuData.fusionQPose.y();
      const double qz = imuData.fusionQPose.z();

      double q[4] = {scalar, qy, qy, qz};
        
        // get expected direction of g
        g[0] = 2 * (q[1] * q[3] - q[0] * q[2]);
        g[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
        g[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
        

        // compensate accelerometer readings with the expected direction of g
        res[0] = acc[0] - g[0];
        res[1] = acc[1] - g[1];
        res[2] = acc[2] - g[2];
}


int main()
{
    int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;

    //  using RTIMULib here allows it to use the .ini file generated by RTIMULibDemo.

    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");

    RTIMU *imu = RTIMU::createIMU(settings);
    RTPressure *pressure = RTPressure::createPressure(settings);
    RTHumidity *humidity = RTHumidity::createHumidity(settings);

    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }

    //  This is an opportunity to manually override any settings before the call IMUInit

    //  set up IMU

    imu->IMUInit();

    //  this is a convenient place to change fusion parameters

    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    //  set up pressure sensor

    if (pressure != NULL)
        pressure->pressureInit();

    //  set up humidity sensor

    if (humidity != NULL)
        humidity->humidityInit();

    //  set up for rate timer

    rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();

    //  now just process data

    while (1) {
        //  poll at the rate recommended by the IMU

        usleep(imu->IMUGetPollInterval() * 1000);

        while (imu->IMURead()) {
            RTIMU_DATA imuData = imu->getIMUData();

            //  add the pressure data to the structure

            if (pressure != NULL)
                pressure->pressureRead(imuData);

            //  add the humidity data to the structure

            if (humidity != NULL)
                humidity->humidityRead(imuData);

            sampleCount++;

            now = RTMath::currentUSecsSinceEpoch();

            //  display 5 times per second

            if ((now - displayTimer) > 200000) {
                printf("\n");
                printf("accel x %f\n", imuData.accel.x());
                printf("accel y %f\n", imuData.accel.y());
                printf("accel z %f\n", imuData.accel.z());

                printf("deg accel x %f\n", imuData.accel.x() * RTMATH_RAD_TO_DEGREE);
                printf("deg accel y %f\n", imuData.accel.y() * RTMATH_RAD_TO_DEGREE);
                printf("deg accel z %f\n", imuData.accel.z() * RTMATH_RAD_TO_DEGREE);

                printf("gyro x %f\n", imuData.gyro.x());
                printf("gyro y %f\n", imuData.gyro.y());
                printf("gyro z %f\n", imuData.gyro.z());

                printf("deg gyro x %f\n", imuData.gyro.x() * RTMATH_RAD_TO_DEGREE);
                printf("deg gyro y %f\n", imuData.gyro.y() * RTMATH_RAD_TO_DEGREE);
                printf("deg gyro z %f\n", imuData.gyro.z() * RTMATH_RAD_TO_DEGREE);

                printf("compass x %f\n", imuData.compass.x());
                printf("compass y %f\n", imuData.compass.y());
                printf("compass z %f\n", imuData.compass.z());
                
                printf("%-15s%-s\n", "fusionpose", RTMath::displayRadians("",
                            imuData.fusionPose));
                printf("%-15s%-s\n", "degrees fusionpose", RTMath::displayDegrees("",
                            imuData.fusionPose));

                printf("fusionpose fusion pose x in %f\n", imuData.fusionPose.x());
                printf("fusionpose fusion pose y in %f\n", imuData.fusionPose.y());
                printf("fusionpose fusion pose z in %f\n", imuData.fusionPose.z());

                printf("deg fusionpose fusion pose x in %f\n",
                        imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE);
                printf("deg fusionpose fusion pose y in %f\n",
                        imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE);
                printf("deg fusionpose fusion pose z in %f\n",
                        imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE);


                imuData.fusionPose.accelToEuler(imuData.fusionPose);
                //printf("Accel to euler fusionpose fusion pose x in %f\n",
                //        imuData.fusionPose.x());
                //printf("Accel to euler fusionpose fusion pose y in %f\n",
                //        imuData.fusionPose.y());
                //printf("Accel to euler fusionpose fusion pose z in %f\n",
                //        imuData.fusionPose.z());
                //printf("\n");

                double cart[3] = {0, 0, 0};
                fuseToCartesian(imuData.fusionPose, cart);
                //printf("CARTESIAN X: %f\n", cart[0]);
                //printf("CARTESIAN Y: %f\n", cart[1]);
                //printf("CARTESIAN Z: %f\n", cart[2]);
                
                RTVector3 ress = RTMath::poseFromAccelMag(imuData.accel, imuData.compass);
                printf("%-15s%-s\n", "poseFromAccelMag", RTMath::displayDegrees("",
                            ress));
                compensate_gravity(imuData);
                double res[3] = {0.0, 0.0, 0.0};
                gravity_compensate(imuData, res);
                printf("G COMPENSATED X: %f\n", res[0]);
                printf("G COMPENSATED Y: %f\n", res[1]);
                printf("G COMPENSATED Z: %f\n", res[2]);

                printf("PRECISION\n");
                printf("G COMPENSATED X: %.2f\n", res[0]);
                printf("G COMPENSATED Y: %.2f\n", res[1]);
                printf("G COMPENSATED Z: %.2f\n", res[2]);

                fflush(stdout);
                displayTimer = now;
            }

            //  update rate every second

            if ((now - rateTimer) > 1000000) {
                sampleRate = sampleCount;
                sampleCount = 0;
                rateTimer = now;
            }
        }
    }
}
