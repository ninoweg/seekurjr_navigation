#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Float32.h>

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
    ROS_INFO("Number Of Goals: [%d]",count/4);
    battery_state_of_charge = n.subscribe("/rosaria/battery_state_of_charge", 1000, batterystateCallback);
  }

  void batterystateCallback(std_msgs::Float32::ConstPtr msg)
  {
    ROS_INFO("Battery State was updated!");
    battery_state=msg->data;
    ROS_INFO("Battery state: [%f]", battery_state);
  }

  GoalState sendToNextGoal(int& batteryStateOfCharge)
  {
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = goals[count];
    goal.target_pose.pose.position.y = goals[count+1];
    goal.target_pose.pose.orientation.w = goals[count+2];

    ROS_INFO("Sending goal");
    ac.sendGoal(goal);
    count++;

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

    ROS_INFO("Battery state low. Sending goal to return to home.");
    ac.sendGoal(goal);
    ac.waitForResult();

    return ac.getState();
  }
}

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");
 
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  ROS_INFO("move_base action server is up");

  ros::NodeHandle n;
  MoveBetweenGoals mbg = MoveBetweenGoals(n);

  return 0;
}
