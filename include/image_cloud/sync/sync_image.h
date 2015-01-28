/*
 * image_pointcloud_nodelet.h
 *
 *  Created on: 11.08.2014
 *      Author: fnolden
 */

#ifndef SYNC_IMAGE_NODELET_H_
#define SYNC_IMAGE_NODELET_H_

#include <ros/ros.h>
#include <sstream>
#include <string.h>
#include <map>
#include <iostream>
#include <functional>

#include <image_transport/image_transport.h>
#include <tf/transform_broadcaster.h>
#include <nodelet/nodelet.h>

#include <pcl_ros/point_cloud.h>

#include <cv_bridge/cv_bridge.h>
#include <opencv/cv.h>
#include <eigen3/Eigen/Core>

#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include <sensor_msgs/Image.h>
#include <sensor_msgs/PointCloud2.h>

#include <dynamic_reconfigure/server.h>
#include <image_cloud/sync_imageConfig.h>

namespace image_cloud {

class Sync_image : public nodelet::Nodelet {

	typedef image_cloud::sync_imageConfig Config;
	typedef dynamic_reconfigure::Server<Config> ReconfigureServer;
	typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> Image_sync_filter;
public:
	virtual void onInit();
	virtual ~Sync_image();
	virtual void callback(const sensor_msgs::ImageConstPtr& input1, const sensor_msgs::ImageConstPtr& input2);
	virtual void reconfigure_callback(Config &config, uint32_t level);
	virtual void init_params();
	virtual void init_sub();
	virtual void init_pub();
	virtual void params();
private:
	boost::shared_ptr<ReconfigureServer> reconfigure_server_;

	boost::shared_ptr<message_filters::Subscriber<sensor_msgs::Image> > image_sub2;
	boost::shared_ptr<message_filters::Subscriber<sensor_msgs::Image> > image_sub;
	boost::shared_ptr<message_filters::Synchronizer<Image_sync_filter> > sync;

	boost::shared_ptr<image_transport::ImageTransport> it_;

	image_transport::Publisher pub_image_;
	image_transport::Publisher pub_image2_;

	std::string node_name_;
	boost::mutex config_lock_;
	Config config_;
	ros::NodeHandle nh_;
};
} /* end namespace */

#endif /* SYNC_IMAGE_NODELET_H_ */
