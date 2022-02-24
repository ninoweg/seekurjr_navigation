#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <rosaria/BumperState.h>

void bumperstateCallback(rosaria::BumperState::ConstPtr msg)
{
  for(int i = 0; i<4; i++)
  {
    if(msg->front_bumpers.at(i)==1 || msg->rear_bumpers.at(i)==1)
    {
      ROS_INFO("Bumper was activated!");
      ROS_INFO("Wait for RosAria disable_motors service ... ");
      motor.waitForExistence();
      ROS_INFO("Call RosAria disable_motors service ... ");
      motor.call(srv);
      ROS_INFO("Motors deactivated.");
    }
  }
}

class BumperDisableMotor 
{
private:
  ros::ServiceClient motor;
  ros::Subscriber bumper_state;
  std_srvs::Empty srv;
public:
  void BumperDisableMotor(ros::NodeHandle& n)
  {
    bumper_state = n.subscribe("/rosaria/bumper_state", 10, &bumperstateCallback, this);
    motor = n.serviceClient<std_srvs::Empty>("/rosaria/disable_motors"); 
  }
}

int main(int argc, char** argv){
  ros::init(argc, argv, "bumpers_disable_motor");
  ros::NodeHandle n;
  BumperDisableMotor bdm = BumperDisableMotor(n);
  ros::Spin();
  return 0;
}
