#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Float32.h>
#include <rosaria/BumperState.h>
#include <geometry_msgs/Twist.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
std::vector<uint8_t, std::allocator<uint8_t>> front_bumper_state;
std::vector<uint8_t, std::allocator<uint8_t>> rear_bumper_state;

void bumperstateCallback(const rosaria::BumperState::ConstPtr& msg)
{
  front_bumper_state = msg->front_bumpers;
  rear_bumper_state = msg->rear_bumpers;
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
  ros::Subscriber bumper_state = n.subscribe("/rosaria/bumper_state", 1000, bumperstateCallback);
  ros::Publisher cmd_bumper = n.advertise<geometry_msgs::Twist>("/rosaria/cmd_vel", 100);
  geometry_msgs::Twist cmd_msg;

  while(true){
    for(int k = 0; k<front_bumper_state.size(); k++)
    {
      if(front_bumper_state.at(k)==1){
        ac.cancelAllGoals();
        cmd_msg.linear.x = 0;
        cmd_msg.linear.y = 0;
        cmd_msg.linear.z = 0;
        cmd_msg.angular.x = 0;
        cmd_msg.angular.y = 0;
        cmd_msg.angular.z = 0;
        ROS_INFO("Front Bumper [%i] activated! Canceled all Goals.", k);
      }
    }

    for(int k = 0; k<rear_bumper_state.size(); k++)
    {
      if(rear_bumper_state.at(k)==1){
        ac.cancelAllGoals();
        cmd_msg.linear.x = 0;
        cmd_msg.linear.y = 0;
        cmd_msg.linear.z = 0;
        cmd_msg.angular.x = 0;
        cmd_msg.angular.y = 0;
        cmd_msg.angular.z = 0;
        ROS_INFO("Rear Bumper [%i] activated! Canceled all Goals.", k);
      }
    }
    
    ros::spinOnce();
  }

  return 0;

  
}