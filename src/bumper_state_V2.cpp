#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <rosaria/BumperState.h>

int state = 0;

void bumperstateCallback(const rosaria::BumperState::ConstPtr& msg)
{
  for(int k = 0; k<4; k++)
  {
    if(msg->front_bumpers.at(k)==1 || msg->rear_bumpers.at(k)==1)
    {
      ROS_INFO("Bumper activated!");
      state = 1;
    }
  }
}

int main(int argc, char** argv){
  static std::vector<double> goals;
  static std::vector<double> home;
  
  ros::init(argc, argv, "bumpers_disable_motor");

  ros::NodeHandle n;
  // ros::Subscriber bumper_state = n.subscribe<rosaria::BumperState>("/rosaria/bumper_state", 1000, boost::bind(bumperstateCallback, _1, state));
  ros::Subscriber bumper_state = n.subscribe("/rosaria/bumper_state", 1000, bumperstateCallback);
  ros::ServiceClient motor = n.serviceClient<std_srvs::Empty>("/rosaria/disable_motors"); 
  std_srvs::Empty srv;

  while(true)
  {
    if(state==1)
    {
      ROS_INFO("Wait for RosAria disable_motors service ... ");
      motor.waitForExistence();
      ROS_INFO("Call RosAria disable_motors service ... ");
      motor.call(srv);
      ROS_INFO("Bumper activated! Motors deactivated.");
      state = 0;
    }
    ros::spinOnce();
  }

  return 0;
}
