#include <iostream>
#include <unistd.h>
#include "float32.h"
#include "math.h"
#include "marg.h"

namespace Control {

/* TODO: convert using sensors setup */

namespace Original {
// System constants
float deltat = 0.05f; // sampling period in seconds (shown as 1 ms)
float gyroMeasError = 3.14159265358979f * (5.0f / 180.0f); // gyroscope measurement error in rad/s (shown as 5 deg/s)
float beta = sqrt(3.0f / 4.0f) * gyroMeasError; // compute beta

// Global system variables
float SEq_1 = 1.0f, SEq_2 = 0.0f, SEq_3 = 0.0f, SEq_4 = 0.0f; // estimated orientation quaternion elements with initial conditions

void filterUpdate(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z)
{
// Local system variables
float norm; // vector norm
float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; // quaternion derrivative from gyroscopes elements
float f_1, f_2, f_3; // objective function elements
float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; // objective function Jacobian elements
float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4; // estimated direction of the gyroscope error
// Axulirary variables to avoid reapeated calcualtions
float halfSEq_1 = 0.5f * SEq_1;
float halfSEq_2 = 0.5f * SEq_2;
float halfSEq_3 = 0.5f * SEq_3;
float halfSEq_4 = 0.5f * SEq_4;
float twoSEq_1 = 2.0f * SEq_1;
float twoSEq_2 = 2.0f * SEq_2;
float twoSEq_3 = 2.0f * SEq_3;
// Normalise the accelerometer measurement
norm = sqrt(a_x * a_x + a_y * a_y + a_z * a_z);
a_x /= norm;
a_y /= norm;
a_z /= norm;
// Compute the objective function and Jacobian
f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - a_x;
f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - a_y;
f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - a_z;
J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
J_12or23 = 2.0f * SEq_4;
J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
J_14or21 = twoSEq_2;
J_32 = 2.0f * J_14or21; // negated in matrix multiplication
J_33 = 2.0f * J_11or24; // negated in matrix multiplication
// Compute the gradient (matrix multiplication)
SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;
// Normalise the gradient
norm = sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
SEqHatDot_1 /= norm;
SEqHatDot_2 /= norm;
SEqHatDot_3 /= norm;
SEqHatDot_4 /= norm;
// Compute the quaternion derrivative measured by gyroscopes
SEqDot_omega_1 = -halfSEq_2 * w_x - halfSEq_3 * w_y - halfSEq_4 * w_z;
SEqDot_omega_2 = halfSEq_1 * w_x + halfSEq_3 * w_z - halfSEq_4 * w_y;
SEqDot_omega_3 = halfSEq_1 * w_y - halfSEq_2 * w_z + halfSEq_4 * w_x;
SEqDot_omega_4 = halfSEq_1 * w_z + halfSEq_2 * w_y - halfSEq_3 * w_x;
// Compute then integrate the estimated quaternion derrivative
SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;
// Normalise quaternion
norm = sqrt(SEq_1 * SEq_1 + SEq_2 * SEq_2 + SEq_3 * SEq_3 + SEq_4 * SEq_4);
SEq_1 /= norm;
SEq_2 /= norm;
SEq_3 /= norm;
SEq_4 /= norm;
}

}


MARG::MARG() : deltat(0.001f), SEq_1(1.0f), b_x(1.0f) {
}

#define debug_print(x) std::cout << #x << " = " << (float)x << std::endl; 

void MARG::start() {
    /* Update IMU data */
    /* TODO: update magnetometer as well */
    //Sensors::IMU::MPU6050::updateAccelerometerAndGyro();
    /* Convert to SI */
    const float32 GYRO_FACTOR(0.06103515625f * 3.14159f / 180.0f); // [-2000:2000] / 65536
    const float32 ACC_FACTOR(0.00048828125f * 9.81f); // [-16:16] / 65536
    const float32 COMPASS_FACTOR(0.00091743f); // http://www.soc-robotics.com/pdfs/HMC5883L.pdf : page 12, GN0 = 1, GN1 = 0, GN2 = 0, +-1.3Ga, 1090LSB/Gauss, [-2048, 2047] => 1 / 1090.0f
    //const float32 cpu_tick_time(1.0f / CPU_FREQUENCY_HZ);

    /* Measure deltat */
    //unsigned int rtc = *DEV_RTC;
    /* WARNING: test that float32(unsigned int) ctor is correct */
    //deltat = float32(rtc - old_rtc) * cpu_tick_time;
    //old_rtc = rtc;
    deltat = 0.05f;

    /*some random values */
    int wx = 0, wy = 0, wz = 16,
	ax = -1, ay = 1, az = -2000;

    float w_x = float32(wx) * GYRO_FACTOR,
	    w_y = float32(wy) * GYRO_FACTOR,
	    w_z = float32(wz) * GYRO_FACTOR,
	    a_x = float32(ax) * ACC_FACTOR,
	    a_y = float32(ay) * ACC_FACTOR,
	    a_z = float32(az) * ACC_FACTOR,
	    /* These will be optimized out in case we don't use them */
	    m_x = float32(0) * COMPASS_FACTOR,
	    m_y = float32(0) * COMPASS_FACTOR,
	    m_z = float32(0) * COMPASS_FACTOR;

/*    debug_print(w_x);
    debug_print(w_y);
    debug_print(w_z);
    debug_print(a_x);
    debug_print(a_y);
    debug_print(a_z);*/

    /* Do not use magnetometer */
    Original::filterUpdate(w_x, w_y, w_z, a_x, a_y, a_z);
    //filterUpdateIMU(w_x, w_y, w_z, a_x, a_y, a_z);
    /* Use magnetometer */
    //filterUpdateMARG(w_x, w_y, w_z, a_x, a_y, a_z, m_x, m_y, m_z);

    /* Update System::Registry with the new attitude */
    /*System::Registry::set(System::Registry::ORIENTATION_Q1, SEq_1);
    System::Registry::set(System::Registry::ORIENTATION_Q2, SEq_2);
    System::Registry::set(System::Registry::ORIENTATION_Q3, SEq_3);
    System::Registry::set(System::Registry::ORIENTATION_Q4, SEq_4);*/

    /* Convert quaternion to euler angles, in degrees */
    const float32 to_deg(57.295779513f);
    const float32 zero(0.0f), one(1.0f), two(2.0f), minus_two(-2.0f);
    
    // psi - yaw

    const float32 psi = float32(float(atan2((float)(two * Original::SEq_2 * Original::SEq_3 - two * Original::SEq_1 * Original::SEq_4), 
			    (float)(two * Original::SEq_1 * Original::SEq_1 + two * Original::SEq_2 * Original::SEq_2 - one)))) * to_deg;

    const float32 theta = float32(float(f32::asin(two * (Original::SEq_2 * Original::SEq_4 + Original::SEq_1 * Original::SEq_3)) * to_deg));
    const float32 phi = float32(float(atan2((float)(two * Original::SEq_3 * Original::SEq_4 - two * Original::SEq_1 * Original::SEq_2), (float)(two * Original::SEq_1 * Original::SEq_1 + two * Original::SEq_4 * Original::SEq_4 - one)))) * to_deg;

    /*System::Registry::set(System::Registry::ANGLE_PSI, psi);
    System::Registry::set(System::Registry::ANGLE_THETA, theta);
    System::Registry::set(System::Registry::ANGLE_PHI, phi);*/
    //std::cout << (float)Original::SEq_1 << " " <<(float)Original::SEq_2 << " " <<(float)Original::SEq_3 << " " <<(float)Original::SEq_4 << " "  << std::endl;
    std::cout << (float)psi << " " << (float)theta << " " << (float)phi << std::endl;
    usleep(50000);
}
void MARG::filterUpdateIMU(float32 w_x, float32 w_y, float32 w_z, float32 a_x, float32 a_y, float32 a_z)
{
/*
    // System constants
    const float32 gyroMeasError(0.08726646259971647884618453842443f); // gyroscope measurement error in rad/s (shown as 5 deg/s)
    const float32 gyroMeasDrift(0.00349065850398865915384738153698f); // gyroscope measurement error in rad/s/s (shown as 0.2f deg/s/s)
    const float32 beta(0.07557497350239041509830947509372f); // compute beta
    const float32 zeta(0.00302299894009561660393237900375f); // compute zeta
    const float32 deltatzeta(3.02299894009561660393237900375e-6f);

	// Local system variables
	float32 norm; // vector norm
	float32 SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; // quaternion derrivative from gyroscopes elements
	float32 f_1, f_2, f_3; // objective function elements
	float32 J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; // objective function Jacobian elements
	float32 SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4; // estimated direction of the gyroscope error
	
	// Auxlirary variables to avoid reapeated calcualtions
	const float32 half(0.5f), one(1.0f), two(2.0f);
	float32 halfSEq_1 = half * SEq_1;
	float32 halfSEq_2 = half * SEq_2;
	float32 halfSEq_3 = half * SEq_3;
	float32 halfSEq_4 = half * SEq_4;
	float32 twoSEq_1 = two * SEq_1;
	float32 twoSEq_2 = two * SEq_2;
	float32 twoSEq_3 = two * SEq_3;
	
	// Normalise the accelerometer measurement
	float32 norm_squared(a_x * a_x);
	norm_squared += a_y * a_y;
	norm_squared += a_z * a_z;
	norm = f32::rsqrt(norm_squared);
	a_x *= norm;
	a_y *= norm;
	a_z *= norm;
	
	// Compute the objective function and Jacobian
	f_1 = twoSEq_2 * SEq_4;
	f_1 -= twoSEq_1 * SEq_3;
	f_1 -= a_x;
	f_2 = twoSEq_1 * SEq_2;
	f_2 += twoSEq_3 * SEq_4;
	f_2 -= a_y;
	f_3 = one;
	f_3 -= twoSEq_2 * SEq_2;
	f_3 -= twoSEq_3 * SEq_3;
	f_3 -= a_z;
	J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
	J_12or23 = two * SEq_4;
	J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
	J_14or21 = twoSEq_2;
	J_32 = two * J_14or21; // negated in matrix multiplication
	J_33 = two * J_11or24; // negated in matrix multiplication
	
	// Compute the gradient (matrix multiplication)
	SEqHatDot_1 = J_14or21 * f_2;
	SEqHatDot_1 -= J_11or24 * f_1;
	SEqHatDot_2 = J_12or23 * f_1;
	SEqHatDot_2 += J_13or22 * f_2;
	SEqHatDot_2 -= J_32 * f_3;
	SEqHatDot_3 = J_12or23 * f_2;
	SEqHatDot_3 -= J_33 * f_3;
	SEqHatDot_3 -= J_13or22 * f_1;
	SEqHatDot_4 = J_14or21 * f_1;
	SEqHatDot_4 + J_11or24 * f_2;
	
	// Normalise the gradient
	
	norm_squared = SEqHatDot_1 * SEqHatDot_1;
	norm_squared += SEqHatDot_2 * SEqHatDot_2;
	norm_squared += SEqHatDot_3 * SEqHatDot_3;
	norm_squared += SEqHatDot_4 * SEqHatDot_4;
	norm = f32::rsqrt(norm_squared);
	SEqHatDot_1 *= norm;
	SEqHatDot_2 *= norm;
	SEqHatDot_3 *= norm;
	SEqHatDot_4 *= norm;
	
	// Compute the quaternion derrivative measured by gyroscopes
	SEqDot_omega_1 = float32(0.0f);
	SEqDot_omega_1 -= halfSEq_2 * w_x;
	SEqDot_omega_1 -= halfSEq_3 * w_y;
	SEqDot_omega_1 -= halfSEq_4 * w_z;
	SEqDot_omega_2 = halfSEq_1 * w_x;
	SEqDot_omega_2 += halfSEq_3 * w_z;
	SEqDot_omega_2 -= halfSEq_4 * w_y;
	SEqDot_omega_3 = halfSEq_1 * w_y;
	SEqDot_omega_3 -= halfSEq_2 * w_z;
	SEqDot_omega_3 += halfSEq_4 * w_x;
	SEqDot_omega_4 = halfSEq_1 * w_z;
	SEqDot_omega_4 += halfSEq_2 * w_y;
	SEqDot_omega_4 -= halfSEq_3 * w_x;
	
	// Compute then integrate the estimated quaternion derrivative
	SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
	SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
	SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
	SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;
	
	// Normalise quaternion
	norm_squared = SEq_1 * SEq_1;
	norm_squared += SEq_2 * SEq_2;
	norm_squared += SEq_3 * SEq_3;
	norm_squared += SEq_4 * SEq_4;
	norm = f32::rsqrt(norm_squared);
	SEq_1 *= norm;
	SEq_2 *= norm;
	SEq_3 *= norm;
	SEq_4 *= norm;
*/
}

void MARG::filterUpdateMARG(float32 w_x, float32 w_y, float32 w_z, float32 a_x, float32 a_y, float32 a_z, float32 m_x, float32 m_y, float32 m_z)
{
/*
    // System constants
    const float32 gyroMeasError(0.08726646259971647884618453842443f); // gyroscope measurement error in rad/s (shown as 5 deg/s)
    const float32 gyroMeasDrift(0.00349065850398865915384738153698f); // gyroscope measurement error in rad/s/s (shown as 0.2f deg/s/s)
    const float32 beta(0.07557497350239041509830947509372f); // compute beta
    const float32 zeta(0.00302299894009561660393237900375f); // compute zeta
    const float32 deltatzeta(3.02299894009561660393237900375e-6f);

	// local system variables
	float32 norm; // vector norm
	float32 SEqDot_omega_1(0.0f), SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; // quaternion rate from gyroscopes elements
	float32 f_1, f_2, f_3, f_4, f_5, f_6; // objective function elements
	float32 J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33, // objective function Jacobian elements
	J_41, J_42, J_43, J_44, J_51, J_52, J_53, J_54, J_61, J_62, J_63, J_64; //
	float32 SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4; // estimated direction of the gyroscope error
	float32 w_err_x, w_err_y, w_err_z; // estimated direction of the gyroscope error (angular)
	float32 h_x, h_y, h_z; // computed flux in the earth frame

	// axulirary variables to avoid reapeated calcualtions
	const float32 half(0.5f), two(2.0f);
	const float32 halfSEq_1 = half * SEq_1;
	const float32 halfSEq_2 = half * SEq_2;
	const float32 halfSEq_3 = half * SEq_3;
	const float32 halfSEq_4 = half * SEq_4;
	const float32 twoSEq_1 = two * SEq_1;
	const float32 twoSEq_2 = two * SEq_2;
	const float32 twoSEq_3 = two * SEq_3;
	const float32 twoSEq_4 = two * SEq_4;
	const float32 twob_x = two * b_x;
	const float32 twob_z = two * b_z;
	const float32 twob_xSEq_1 = twob_x * SEq_1;
	const float32 twob_xSEq_2 = twob_x * SEq_2;
	const float32 twob_xSEq_3 = twob_x * SEq_3;
	const float32 twob_xSEq_4 = twob_x * SEq_4;
	const float32 twob_zSEq_1 = twob_z * SEq_1;
	const float32 twob_zSEq_2 = twob_z * SEq_2;
	const float32 twob_zSEq_3 = twob_z * SEq_3;
	const float32 twob_zSEq_4 = twob_z * SEq_4;
	float32 SEq_1SEq_2;
	float32 SEq_1SEq_3 = SEq_1 * SEq_3;
	float32 SEq_1SEq_4;
	float32 SEq_2SEq_3;
	float32 SEq_2SEq_4 = SEq_2 * SEq_4;
	float32 SEq_3SEq_4;
	const float32 twom_x = two * m_x;
	const float32 twom_y = two * m_y;
	const float32 twom_z = two * m_z;

	// normalise the accelerometer measurement
	float32 norm_squared = a_x * a_x;
	norm_squared += a_y * a_y;
	norm_squared += a_z * a_z;
	norm = f32::rsqrt(norm_squared);
	a_x *= norm;
	a_y *= norm;
	a_z *= norm;

	// normalise the magnetometer measurement
	norm_squared = m_x * m_x;
	norm_squared += m_y * m_y;
	norm_squared += m_z * m_z;
	norm = f32::rsqrt(norm_squared);
	m_x *= norm;
	m_y *= norm;
	m_z *= norm;

	// compute the objective function and Jacobian
	float32 SEq_3_squared = SEq_3 * SEq_3, SEq_2_squared, SEq_4_squared;
	f_1 = twoSEq_2 * SEq_4;
	f_1 -= twoSEq_1 * SEq_3;
	f_1 -= a_x;
	f_2 = twoSEq_1 * SEq_2;
	f_2 += twoSEq_3 * SEq_4;
	f_2 -= a_y;
	f_3 = float32(1.0f); 
	f_3 -= twoSEq_2 * SEq_2;
	f_3 -= twoSEq_3 * SEq_3; 
	f_3 -= a_z;
	f_4 = twob_x * (half - SEq_3_squared - SEq_4 * SEq_4);
	f_4 += twob_z * (SEq_2SEq_4 - SEq_1SEq_3);
	f_4 -= m_x;
	f_5 = twob_x * (SEq_2 * SEq_3 - SEq_1 * SEq_4);
	f_5 += twob_z * (SEq_1 * SEq_2 + SEq_3 * SEq_4); 
	f_5 -= m_y;
	f_6 = twob_x * (SEq_1SEq_3 + SEq_2SEq_4); 
	f_6 += twob_z * (half - SEq_2 * SEq_2 - SEq_3_squared);
	f_6 -= m_z;
	J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
	J_12or23 = two * SEq_4;
	J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
	J_14or21 = twoSEq_2;
	J_32 = two * J_14or21; // negated in matrix multiplication
	J_33 = two * J_11or24; // negated in matrix multiplication
	J_41 = twob_zSEq_3; // negated in matrix multiplication
	J_42 = twob_zSEq_4;
	J_43 = two * twob_xSEq_3;
	J_43 += twob_zSEq_1; // negated in matrix multiplication
	J_44 = two * twob_xSEq_4; 
	J_44 -= twob_zSEq_2; // negated in matrix multiplication
	J_51 = twob_xSEq_4 - twob_zSEq_2; // negated in matrix multiplication
	J_52 = twob_xSEq_3 + twob_zSEq_1;
	J_53 = twob_xSEq_2 + twob_zSEq_4;
	J_54 = twob_xSEq_1 - twob_zSEq_3; // negated in matrix multiplication
	J_61 = twob_xSEq_3;
	J_62 = twob_xSEq_4;
	J_62 -= two * twob_zSEq_2;
	J_63 = twob_xSEq_1;
	J_63 -= two * twob_zSEq_3;
	J_64 = twob_xSEq_2;

	// compute the gradient (matrix multiplication)
	SEqHatDot_1 = J_14or21 * f_2;
	SEqHatDot_1 -= J_11or24 * f_1;
	SEqHatDot_1 -= J_41 * f_4;
	SEqHatDot_1 -= J_51 * f_5;
	SEqHatDot_1 += J_61 * f_6;
	SEqHatDot_2 = J_12or23 * f_1;
	SEqHatDot_2 += J_13or22 * f_2;
	SEqHatDot_2 -= J_32 * f_3;
	SEqHatDot_2 += J_42 * f_4;
	SEqHatDot_2 += J_52 * f_5;
	SEqHatDot_2 += J_62 * f_6;
	SEqHatDot_3 = J_12or23 * f_2;
	SEqHatDot_3 -= J_33 * f_3;
	SEqHatDot_3 -= J_13or22 * f_1;
	SEqHatDot_3 -= J_43 * f_4;
	SEqHatDot_3 += J_53 * f_5;
	SEqHatDot_3 += J_63 * f_6;
	SEqHatDot_4 = J_14or21 * f_1;
	SEqHatDot_4 += J_11or24 * f_2;
	SEqHatDot_4 -= J_44 * f_4;
	SEqHatDot_4 -= J_54 * f_5;
	SEqHatDot_4 += J_64 * f_6;

	// normalise the gradient to estimate direction of the gyroscope error
	float32 SEqHatDotVectorSquared = SEqHatDot_1 * SEqHatDot_1;
	SEqHatDotVectorSquared += SEqHatDot_2 * SEqHatDot_2;
	SEqHatDotVectorSquared += SEqHatDot_3 * SEqHatDot_3; 
	SEqHatDotVectorSquared += SEqHatDot_4 * SEqHatDot_4;
	norm = f32::rsqrt(SEqHatDotVectorSquared);
	SEqHatDot_1 = SEqHatDot_1 * norm;
	SEqHatDot_2 = SEqHatDot_2 * norm;
	SEqHatDot_3 = SEqHatDot_3 * norm;
	SEqHatDot_4 = SEqHatDot_4 * norm;

	// compute angular estimated direction of the gyroscope error
	w_err_x = twoSEq_1 * SEqHatDot_2;
	w_err_x -= twoSEq_2 * SEqHatDot_1;
	w_err_x -= twoSEq_3 * SEqHatDot_4;
	w_err_x += twoSEq_4 * SEqHatDot_3;
	w_err_y = twoSEq_1 * SEqHatDot_3;
	w_err_y += twoSEq_2 * SEqHatDot_4;
	w_err_y -= twoSEq_3 * SEqHatDot_1;
	w_err_y -= twoSEq_4 * SEqHatDot_2;
	w_err_z = twoSEq_1 * SEqHatDot_4;
	w_err_z -= twoSEq_2 * SEqHatDot_3;
	w_err_z += twoSEq_3 * SEqHatDot_2;
	w_err_z -= twoSEq_4 * SEqHatDot_1;

	// compute and remove the gyroscope baises
	w_bx += w_err_x * deltatzeta;
	w_by += w_err_y * deltatzeta;
	w_bz += w_err_z * deltatzeta;
	w_x -= w_bx;
	w_y -= w_by;
	w_z -= w_bz;

	// compute the quaternion rate measured by gyroscopes
	
	SEqDot_omega_1 -= halfSEq_2 * w_x;
	SEqDot_omega_1 -= halfSEq_3 * w_y;
	SEqDot_omega_1 -= halfSEq_4 * w_z;
	SEqDot_omega_2 = halfSEq_1 * w_x;
	SEqDot_omega_2 += halfSEq_3 * w_z;
	SEqDot_omega_2 -= halfSEq_4 * w_y;
	SEqDot_omega_3 = halfSEq_1 * w_y;
	SEqDot_omega_3 -= halfSEq_2 * w_z;
	SEqDot_omega_3 += halfSEq_4 * w_x;
	SEqDot_omega_4 = halfSEq_1 * w_z;
	SEqDot_omega_4 += halfSEq_2 * w_y;
	SEqDot_omega_4 -= halfSEq_3 * w_x;

	// compute then integrate the estimated quaternion rate
	SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
	SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
	SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
	SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;

	// normalise quaternion
	float32 SEq_squared = SEq_1 * SEq_1;
	SEq_squared += SEq_2 * SEq_2;
	SEq_squared += SEq_3 * SEq_3;
	SEq_squared += SEq_4 * SEq_4;
	norm = f32::rsqrt(SEq_squared);
	SEq_1 *= norm;
	SEq_2 *= norm;
	SEq_3 *= norm;
	SEq_4 *= norm;

	// compute flux in the earth frame
	SEq_1SEq_2 = SEq_1 * SEq_2; // recompute axulirary variables
	SEq_1SEq_3 = SEq_1 * SEq_3;
	SEq_1SEq_4 = SEq_1 * SEq_4;
	SEq_3SEq_4 = SEq_3 * SEq_4;
	SEq_2SEq_3 = SEq_2 * SEq_3;
	SEq_2SEq_4 = SEq_2 * SEq_4;
	SEq_2_squared = SEq_2 * SEq_2;
	SEq_3_squared = SEq_3 * SEq_3;
	SEq_4_squared = SEq_4 * SEq_4;
	h_x = twom_x * (half - SEq_3_squared - SEq_4_squared);
	h_x += twom_y * (SEq_2SEq_3 - SEq_1SEq_4);
	h_x += twom_z * (SEq_2SEq_4 + SEq_1SEq_3);
	h_y = twom_x * (SEq_2SEq_3 + SEq_1SEq_4);
	h_y += twom_y * (half - SEq_2_squared - SEq_4_squared);
	h_y += twom_z * (SEq_3SEq_4 - SEq_1SEq_2);
	h_z = twom_x * (SEq_2SEq_4 - SEq_1SEq_3);
	h_z += twom_y * (SEq_3SEq_4 + SEq_1SEq_2);
	h_z += twom_z * (half - SEq_2_squared - SEq_3_squared);

	// normalise the flux vector to have only components in the x and z
	b_x = sqrt((h_x * h_x) + (h_y * h_y));
	b_z = h_z;
*/
}

}
