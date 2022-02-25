#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Float32.h>
#include <utility>
#include <vector>

using actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> = MoveBaseClient;
using actionlib::SimpleClientGoalState = GoalState;

class MoveBetweenGoals 
{
private:
  const std::vector<double> goals;
  const std::vector<double> home;
  float battery_state;
  ros::Subscriber battery_state_of_charge;
  int count, i;
  move_base_msgs::MoveBaseGoal goal;

public:
  void MoveBetweenGoals(ros::NodeHandle& n)
  {
    ros::param::get("/simple_navigation_goals/home", home);
    ros::param::get("/simple_navigation_goals/goals", goals);
    count = goals.size();
    battery_state_of_charge = n.subscribe("/rosaria/battery_state_of_charge", 1000, batterystateCallback);
    MoveBaseClient ac("move_base", true);
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the move_base action server to come up");
    }
  }
  
  void setBatteryState(float& bs){battery_state = bs;}
  
  float getBatteryState(){return battery_state;}
  
  GoalState sendToNextGoal()
  {
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = goals[i];
    goal.target_pose.pose.position.y = goals[i+1];
    goal.target_pose.pose.orientation.w = goals[i+2];
    ac.sendGoal(goal);
    if(i+4 > count){i=0;}
    else{i+4;}
    return ac.getState(); // actionlib::SimpleClientGoalState::SUCCEEDED
  }

  GoalState sendHome()
  {
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = home[0];
    goal.target_pose.pose.position.y = home[1];
    goal.target_pose.pose.orientation.z = home[2];
    goal.target_pose.pose.orientation.w = home[3];
    ac.sendGoal(goal);
    ac.waitForResult();
    i = 0;
    return ac.getState();
  }

  void batterystateCallback(std_msgs::Float32::ConstPtr msg)
  {
    setBatteryState(msg->data);
    if(getBatteryState > 0.2){sendToNextGoal();}
    else{sendHome();}
  }
}

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");
  ros::NodeHandle n;
  MoveBetweenGoals mbg = MoveBetweenGoals(n);
  ros::spin();
  return 0;
}#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Float32.h>
#include <utility>
#include <vector>

using actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> = MoveBaseClient;
using actionlib::SimpleClientGoalState = GoalState;

class MoveBetweenGoals 
{
private:
  const std::vector<double> goals;
  const std::vector<double> home;
  float battery_state;
  ros::Subscriber battery_state_of_charge;
  int count, i;
  move_base_msgs::MoveBaseGoal goal;

public:
  void MoveBetweenGoals(ros::NodeHandle& n)
  {
    ros::param::get("/simple_navigation_goals/home", home);
    ros::param::get("/simple_navigation_goals/goals", goals);
    count = goals.size();
    battery_state_of_charge = n.subscribe("/rosaria/battery_state_of_charge", 1000, batterystateCallback);
    MoveBaseClient ac("move_base", true);
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the move_base action server to come up");
    }
  }
  
  void setBatteryState(float& bs){battery_state = bs;}
  
  float getBatteryState(){return battery_state;}
  
  GoalState sendToNextGoal()
  {
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = goals[i];
    goal.target_pose.pose.position.y = goals[i+1];
    goal.target_pose.pose.orientation.w = goals[i+2];
    ac.sendGoal(goal);
    if(i+4 > count){i=0;}
    else{i+4;}
    return ac.getState(); // actionlib::SimpleClientGoalState::SUCCEEDED
  }

  GoalState sendHome()
  {
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = home[0];
    goal.target_pose.pose.position.y = home[1];
    goal.target_pose.pose.orientation.z = home[2];
    goal.target_pose.pose.orientation.w = home[3];
    ac.sendGoal(goal);
    ac.waitForResult();
    i = 0;
    return ac.getState();
  }

  void batterystateCallback(std_msgs::Float32::ConstPtr msg)
  {
    setBatteryState(msg->data);
    if(getBatteryState > 0.2){sendToNextGoal();}
    else{sendHome();}
  }
}

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");
  ros::NodeHandle n;
  MoveBetweenGoals mbg = MoveBetweenGoals(n);
  ros::spin();
  return 0;
}
