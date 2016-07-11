/**
 * @file trajectory_avoidance.h
 * @brief This defines a cost function for trajectory avoidance.
 *
 * @author Jorge Nicho
 * @date March 30, 2016
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2016, Southwest Research Institute
 *
 * @license Software License Agreement (Apache License)\n
 * \n
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at\n
 * \n
 * http://www.apache.org/licenses/LICENSE-2.0\n
 * \n
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef INDUSTRIAL_MOVEIT_STOMP_MOVEIT_INCLUDE_STOMP_MOVEIT_COST_FUNCTIONS_OBSTACLE_AVOIDANCE_H_
#define INDUSTRIAL_MOVEIT_STOMP_MOVEIT_INCLUDE_STOMP_MOVEIT_COST_FUNCTIONS_OBSTACLE_AVOIDANCE_H_

#include <ros/ros.h>
#include <moveit/robot_model/robot_model.h>
#include "stomp_moveit/cost_functions/stomp_cost_function.h"

/**

  Cost function that keeps new trajectories away from a set of other trajectories.

  Add a function to the stomp planner so that we can add hooks into it.

  **/

namespace stomp_moveit
{
namespace cost_functions
{

class TrajectoryAvoidance : public StompCostFunction
{
public:
  TrajectoryAvoidance();
  virtual ~TrajectoryAvoidance();

  virtual bool initialize(moveit::core::RobotModelConstPtr robot_model_ptr,
                          const std::string& group_name,XmlRpc::XmlRpcValue& config) override;

  virtual bool configure(const XmlRpc::XmlRpcValue& config) override;

  virtual bool setMotionPlanRequest(const planning_scene::PlanningSceneConstPtr& planning_scene,
                   const moveit_msgs::MotionPlanRequest &req,
                   const stomp_core::StompConfiguration &config,
                   moveit_msgs::MoveItErrorCodes& error_code) override;


  /**
   * @brief computes the state costs as a function of the parameters for each time step.
   * @param parameters [num_dimensions] num_parameters - policy parameters to execute
   * @param start_timestep    start index into the 'parameters' array, usually 0.
   * @param num_timesteps     number of elements to use from 'parameters' starting from 'start_timestep'   *
   * @param iteration_number  The current iteration count in the optimization loop
   * @param rollout_number    index of the noisy trajectory whose cost is being evaluated.   *
   * @param costs             vector containing the state costs per timestep.
   * @param validity          whether or not the trajectory is valid
   * @return true if cost were properly computed
   */
  virtual bool computeCosts(const Eigen::MatrixXd& parameters,
                            std::size_t start_timestep,
                            std::size_t num_timesteps,
                            int iteration_number,
                            int rollout_number,
                            Eigen::VectorXd& costs,
                            bool& validity) override;

  virtual std::string getGroupName() const override
  {
    return group_name_;
  }


  virtual std::string getName() const override
  {
    return name_ + "/" + group_name_;
  }

  virtual void done(bool success,int total_iterations,double final_cost) override;

  static void add_trajectory(std::vector<Eigen::Affine3d> &trajectory);

protected:

  std::string name_;

  // robot details
  std::string group_name_;
  moveit::core::RobotModelConstPtr robot_model_ptr_;
  moveit::core::RobotStatePtr robot_state_;

  // planning context information
  planning_scene::PlanningSceneConstPtr planning_scene_;
  moveit_msgs::MotionPlanRequest plan_request_;

  // parameters
  double collision_clearance_;
  double collision_penalty_;

  // The link to use for distance measurement
  std::string tip_link_id_;

  // TODO: define this symbol
  static std::vector<std::vector<Eigen::Affine3d> > accepted_trajectories;

  // ROS structures for getting trajectories
  ros::NodeHandle nh_;
  ros::Subscriber traj_sub_;

  double compute_cost(double distance) {
    return std::min(1.0 / distance, 1000.0);
  }
};

} /* namespace cost_functions */
} /* namespace stomp_moveit */

#endif /* INDUSTRIAL_MOVEIT_STOMP_MOVEIT_INCLUDE_STOMP_MOVEIT_COST_FUNCTIONS_OBSTACLE_AVOIDANCE_H_ */
