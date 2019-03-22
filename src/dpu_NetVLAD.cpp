/**
 * 该例程将发布chatter话题，消息类型String
 */
 
#include <sstream>
#include <queue>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <thread>
#include <unistd.h>


class dpu_NetVLAD{
public:
    dpu_NetVLAD(){
        dpu_netvald_sub = n.subscribe("frame", 1, &dpu_NetVLAD::callbackThread,this);
        dpu_netvald_pub = n.advertise<std_msgs::String>("dpu_NetVLAD", 3);
        recent_doing = 0;
        count_since_last = 0;
    }
    void callbackThread(const std_msgs::String::ConstPtr& msg);

private: 
    ros::NodeHandle n;
    ros::Subscriber dpu_netvald_sub;
    ros::Publisher dpu_netvald_pub;
    int recent_doing;
    int count_since_last;
};

void dpu_NetVLAD::callbackThread(const std_msgs::String::ConstPtr& msg)
{   
    
    int tmp;    
    // n.param<int>("running_dpu_NetVLAD",tmp,0);

    int running_dpu_VO;
    int depth_dpu_VO;
    int depth_cpu_NetVLAD;

    n.param<int>("running_dpu_VO", running_dpu_VO, 0);
    n.param<int>("depth_dpu_VO", depth_dpu_VO, 0);
    n.param<int>("depth_cpu_NetVLAD", depth_cpu_NetVLAD, 0);
    ROS_INFO_STREAM("depth_cpu_NetVLAD: " << depth_cpu_NetVLAD << "; depth_dpu_VO: " << depth_dpu_VO << "; running_dpu_VO: " << running_dpu_VO << "; recent_doing: " << recent_doing);
       
    if ( (running_dpu_VO > 0) ||  depth_dpu_VO > 1 || depth_cpu_NetVLAD > 1 || recent_doing>0){
        if( recent_doing){
            count_since_last += 1;
            if ( depth_cpu_NetVLAD ==0 || count_since_last > 5){
                recent_doing = 0;
            }
        }
        return;
    }
    else{
        recent_doing = 1;
        count_since_last = 0;
        n.setParam("running_dpu_NetVLAD", 1);
        ROS_INFO("DPU NetVLAD heard: [%s]", msg->data.c_str());

        // starting time
        ros::Time start = ros::Time::now();
        // doing computation
        usleep(66*1000);
        // finishing time
        ROS_INFO_STREAM("image ID: " << msg->data.substr(12) << "; starting time: " << start << "; finishing time: " << ros::Time::now());
        std::stringstream ss;
        std_msgs::String msg_pub;
        ss << " DPU NetVLAD result: " << msg->data.c_str();
        msg_pub.data = ss.str();
        dpu_netvald_pub.publish(msg_pub);
        n.setParam("running_dpu_NetVLAD", 0);
    }
}

int main(int argc, char **argv)
{
    // ROS节点初始化
    ros::init(argc, argv, "dpu_NetVLAD");
    dpu_NetVLAD dpu_NetVLAD_inst;
    ros::Rate loop_rate(50);
    
    while (ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
