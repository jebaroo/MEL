// MIT License
//
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab, Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Core/Joint.hpp>
#include <MEL/Core/Actuator.hpp>
#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Utility/Types.hpp>
#include <vector>

namespace mel {

class Robot : public Device {

public:

    /// Constructor.
    Robot(const std::string& name);

    /// Destructor
    virtual ~Robot() { }

    /// Virtual function to enable all robot members.
    virtual bool enable() override;

    /// Virtual function to disable all robot members.
    virtual bool disable() override;

public:

    /// Adds a new joint to the robot
    void add_joint(const Joint& joint);

    /// Gets a reference to a Robot's Joint
    Joint& get_joint(uint32 joint_number);

    /// Gets a reference to a Robot's Joint
    Joint& operator[](uint32 joint_number);

    /// Get the most recently read robot joint positions.
    std::vector<double> get_joint_positions();

    /// Get the most recently read robot joint velocities.
    std::vector<double> get_joint_velocities();

    /// Set the desired actuator torques.
    void set_joint_torques(std::vector<double> new_torques);

    /// Checks position limits of all joints and returns true if any exceeded, false otherwise
    bool check_all_joint_position_limits();

    /// Checks velocity limits of all joints and returns true if any exceeded, false otherwise
    bool check_all_joint_velocity_limits();

    /// Checks torque limits of all joints and returns true if any exceeded, false otherwise
    bool check_all_joint_torque_limits();

    /// Checks position, velocity, and torque limits of all joints and returns true if any exceeded, false otherwise
    bool check_all_joint_limits();


protected:

    std::vector<Joint> joints_; ///< Vector of RobotJoints.

    std::vector<double> joint_positions_; ///< Stores the robot joint positions since the last call of get_robot_joint_positions().
    std::vector<double> joint_velocities_; ///< Stores the robot joint velocities since the last call of get_robot_joint_velocities().
    std::vector<double> joint_torques_; ///< Does this need to exist?

};

}
