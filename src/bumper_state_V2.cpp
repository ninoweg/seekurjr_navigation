#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_srvs/Empty.h>
#include <rosaria/BumperState.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

void bumperstateCallback(const rosaria::BumperState::ConstPtr& msg, bool &state)
{
  for(int k = 0; k<4; k++)
  {
    if(msg->front_bumpers.at(k)==1 || msg->rear_bumpers.at(k)==1)
    {
      state = "true";
    }
  }
}

int main(int argc, char** argv){
  static std::vector<double> goals;
  static std::vector<double> home;
  bool state;

  ros::init(argc, argv, "simple_navigation_goals");
  
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  ROS_INFO("move_base action server is up");

  ros::NodeHandle n;
  ros::Subscriber bumper_state = n.subscribe<rosaria::BumperState>("/rosaria/bumper_state", 1000, boost::bind(bumperstateCallback, _1, state));
  ros::ServiceClient motor = n.serviceClient<std_srvs::Empty>("/rosaria/disable_motors"); 
  std_srvs::Empty srv;

  while(true)
  {
    if(state==1)
    {
      motor.waitForExistence();
      motor.call(srv);
      ROS_INFO("Bumper activated! Canceled all Goals and deactivated Motors.");
    }
    ros::spinOnce();
  }

  return 0; 
}