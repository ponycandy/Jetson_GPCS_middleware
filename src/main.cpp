#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

#include "gpcsnode.h"

ros::Publisher cmd_pub;

struct motioncommand
{
    float linear;
    float omega;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& linear;
        ar& omega;
    }
};

void certaincallback(const std::string& data)
{
    motioncommand recievedData = gpcs::struct_load<motioncommand>(data);
    geometry_msgs::Twist cmd;
    cmd.linear.x=recievedData.linear;
    cmd.linear.y=0;
    cmd.linear.z=0;
    cmd.angular.x=0;
    cmd.angular.y=0;
    cmd.angular.z=recievedData.omega;
    cmd_pub.publish(cmd);
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "GPCS_system");
    ros::NodeHandle nh;
    cmd_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel",5);
    ros::Rate rate(100);

    gpcs::gpcsnode gpcsnh;

    gpcsnh.init("GPCS_system");
    gpcsnh.subscribe("motion_command", certaincallback);

    while(ros::ok())
    {
        gpcsnh.spinonce();
        rate.sleep();
        ros::spinOnce();
    }
    return 0;
}

