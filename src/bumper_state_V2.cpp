#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <rosaria/BumperState.h>

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

  ros::init(argc, argv, "bumper_state");

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
      ROS_INFO("Bumper activated! Motors deactivated.");
    }
    ros::spinOnce();
  }

  return 0; 
}
