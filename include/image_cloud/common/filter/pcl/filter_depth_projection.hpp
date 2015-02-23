#ifndef FILTER_DEPTH_PROJECTION_H_
#define FILTER_DEPTH_PROJECTION_H_

#include <image_cloud/common/filter/pcl/common.hpp>
#include <image_cloud/common/small_helpers.hpp>
#include <image_cloud/common/project2d.hpp>
#include <pcl/common/common.h>

#include <cv_bridge/cv_bridge.h>
#include <opencv/cv.h>
#include <eigen3/Eigen/Core>

//#define DEBUG
//#define USE_Y_THRESHOLD
//#define USE_SQUERE_DISTANCE

namespace filter_3d{

	template <typename PointT>
	inline void
	filter_depth_projection(	const image_geometry::PinholeCameraModel &camera_model,
								const pcl::PointCloud<PointT> &in,
								pcl::PointCloud<PointT> &out,
								int rows,
								int cols,
								int k_neighbors = 8)
	{
		std::vector<std::vector<bool> > hit( cols, std::vector<bool>(rows));
		std::vector<int> points2d_indices;
		pcl::PointCloud<pcl::PointXY> points2d;

#ifdef DEBUG
		std::cout << "in points: "<< in.size() << "\n";
#endif

		// Project points into image space
		for(unsigned int i=0; i < in.points.size();  ++i){
			const PointT* pt = &in.points.at(i);
			if( pt->z > 1) { // min distance from camera 1m

				cv::Point2i point_image = camera_model.project3dToPixel(cv::Point3d(pt->x, pt->y, pt->z));

				if( between<int>(0, point_image.x, cols )
					&& between<int>( 0, point_image.y, rows )
				)
				{
					// Point allready at this position?
					if(!hit[point_image.x][point_image.y]){
						hit[point_image.x][point_image.y] = true;

						pcl::PointXY p_image;
						p_image.x = point_image.x;
						p_image.y = point_image.y;

						points2d.push_back(p_image);
						points2d_indices.push_back(i);
					}

				}
			}
		}

#ifdef DEBUG
		std::cout << "projected 2d points: "<< points2d.size() << " indices: " << points2d_indices.size() << "\n";
#endif

		pcl::search::KdTree<pcl::PointXY> tree_n;
		tree_n.setInputCloud(points2d.makeShared());
		tree_n.setEpsilon(0.1);

		for(unsigned int i=0; i < points2d.size(); ++i){
			std::vector<int> k_indices;
			std::vector<float> square_distance;

			//tree_n.nearestKSearch(points2d.at(i), k_neighbors, k_indices, square_distance);
			tree_n.radiusSearch(points2d.at(i), k_neighbors, k_indices, square_distance);

			look_up_indices(points2d_indices, k_indices);

			if(is_edge(in, points2d_indices.at(i), k_indices, square_distance)){
				out.push_back(in.points.at(points2d_indices.at(i)));
			}
		}

#ifdef DEBUG
		std::cout << "out 2d points: "<< out.size() << "\n";
#endif
	}
}
#endif
