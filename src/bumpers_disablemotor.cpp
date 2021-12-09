#include <ros/ros.h>
#include <rosaria/BumperState.h>
#include <geometry_msgs/Twist.h>
#include <std_srvs/Empty.h>

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
  
  ros::NodeHandle n;
  ros::Subscriber bumper_state = n.subscribe("/rosaria/bumper_state", 1000, bumperstateCallback);
  // ros::Subscriber bumper_state = n.subscribe<rosaria::BumperState>("/rosaria/bumper_state", 1000, boost::bind(bumperstateCallback, _1, state));
  ros::Publisher cmd_bumper = n.advertise<geometry_msgs::Twist>("/rosaria/cmd_vel", 100);
  ros::ServiceClient disable_motor = n.serviceClient<std_srvs::Empty>("/rosaria/disable_motors"); 
  // ros::ServiceClient enable_motor = n.serviceClient<std_srvs::Empty>("/rosaria/enable_motors"); 
  std_srvs::Empty srv;

  while(true){
    for(int k = 0; k<front_bumper_state.size(); k++)
    {
      if(front_bumper_state.at(k)==1){
        // disable_motor.waitForExistence();
        // disable_motor.call(srv);
        ROS_INFO("Front Bumper %i activated! Motors deactivated.", k);
      }
    }

    for(int k = 0; k<rear_bumper_state.size(); k++)
    {
      if(rear_bumper_state.at(k)==1){
        // disable_motor.waitForExistence();
        // disable_motor.call(srv);
        ROS_INFO("Rear Bumper %i activated! Motors deactivated.", k);
      }
    }
    
    ros::spinOnce();
  }

  return 0;

  
}