#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

namespace Sensors {

namespace IMU {

namespace HMC5883 {
    constexpr static unsigned char HMC5883_ADDR_READ = 0x3D;
    constexpr static unsigned char HMC5883_ADDR_WRITE = 0x3C;
    constexpr static unsigned char HMC5883_CFGA = 0x00;
    constexpr static unsigned char HMC5883_CFGA_75HZ_8SAMPLES = 0x78;
    constexpr static unsigned char HMC5883_MODE = 0x02;
    constexpr static unsigned char HMC5883_MODE_CONTINUOUS = 0x0;
    constexpr static unsigned char HMC5883_DATA_START = 0x03;
    void init();
    /* Updates the SystemRegistry with new data from the magnetometer */
    void update();
}

}

}

#endif
