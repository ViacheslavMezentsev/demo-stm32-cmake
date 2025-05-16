/*
 * ros.h
 *
 *  Created on: Nov 2, 2019
 *      Author: xav-jann1
 */

#ifndef _ROS_H_
#define _ROS_H_

#include "STM32Hardware.h"
#include "ros/node_handle.h"

namespace ros
{
    // default 25, 25, 512, 512
    typedef NodeHandle_<STM32Hardware> NodeHandle;
}

#endif
