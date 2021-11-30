#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Float32.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
float battery_state = 0.0;

void batterystateCallback(const std_msgs::Float32::ConstPtr& msg)
{
  ROS_INFO("Battery State was updated!");
  battery_state=msg->data;
  ROS_INFO("Battery state: [%f]", battery_state);
}

int main(int argc, char** argv){
  static std::vector<double> goals;
  static std::vector<double> home;

  ros::init(argc, argv, "simple_navigation_goals");
  
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  ROS_INFO("move_base action server is up");

  ros::NodeHandle n;
  ros::Subscriber battery_state_of_charge = n.subscribe("/rosaria/battery_state_of_charge", 1000, batterystateCallback);

  //making sure we get valid battery state
  while(battery_state==0.0){
     ROS_INFO("Waiting for valid battery state");
     ros::Duration(5).sleep();
     ros::spinOnce();
  }
  ROS_INFO("Battery state: [%f]", battery_state);

  ros::param::get("/simple_navigation_goals/goals", goals);
  ros::param::get("/simple_navigation_goals/home", home);
  int count = goals.size();

  ROS_INFO("Number Of Goals: [%d]",count/4);

  move_base_msgs::MoveBaseGoal goal;

  while(battery_state > 0.1){
    for(int i = 0; i<count; i=i+4){
      goal.target_pose.header.frame_id = "map";
      goal.target_pose.header.stamp = ros::Time::now();
      goal.target_pose.pose.position.x = goals[i];
      goal.target_pose.pose.position.y = goals[i+1];
      goal.target_pose.pose.orientation.w = goals[i+2];

      ROS_INFO("Sending goal");
      ac.sendGoal(goal);

      ac.waitForResult();

      if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Hooray, the next goal was reached!");
      else
        ROS_INFO("The base failed to move to the next point");

      if(battery_state < 0.1)
        return 0;
      
      ros::spinOnce();
    }
  }
  
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();
  goal.target_pose.pose.position.x = home[0];
  goal.target_pose.pose.position.y = home[1];
  goal.target_pose.pose.orientation.z = home[2];
  goal.target_pose.pose.orientation.w = home[3];

  ROS_INFO("Battery state low. Sending goal to return to home.");
  ac.sendGoal(goal);

  ac.waitForResult();

  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, home was reached in time! Please plug in your robot");
  else
    ROS_INFO("The robot failed to get home.");

  return 0;
}