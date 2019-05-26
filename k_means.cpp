#include "stdafx.h"
#include "common.h"
#include <iostream>
#include <random>
#include <time.h>

using namespace std;

#define MAX_ITER 10000

#define ALPHA 0

/*
Euclidean distance between two points (both coordinates and color)
*/
float dist(Point a, Point b, Vec3b color1, Vec3b color2)
{
	return sqrt(ALPHA * pow(a.x - b.x, 2) + ALPHA * pow(a.y - b.y, 2) + pow(color1[0] - color2[0], 2) + pow(color1[1] - color2[1], 2) + pow(color1[2] - color2[2], 2));
}

void k_means()
{
	char fname[MAX_PATH];

	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src, clone;
		src = imread(fname, CV_LOAD_IMAGE_UNCHANGED);
		clone = src.clone();

		int no_of_points = src.rows * src.cols;
		cout << "Total no of points = " << no_of_points << endl;

		imshow("image", src);

		int no_clusters;
		cout << "#Clusters = ";
		cin >> no_clusters;

		// Get all the points in the image
		std::vector<pair <Point, Vec3b>> points;
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				points.push_back(make_pair(Point(j, i), src(i, j)));
			}
		}

		default_random_engine gen;
		uniform_int_distribution<int> dist_img(0, no_of_points);

		// Generate random centroids
		vector<int> randoms;
		for (int i = 0; i < no_clusters; i++)
		{
			randoms.push_back(dist_img(gen));
		}
		// Print random centroids
		for (int i = 0; i < no_clusters; i++)
		{
			cout << " " << randoms.at(i);
		}

		// Generate centers
		vector<pair <Point, Vec3b>> centers;
		for (int i = 0; i < no_clusters; i++)
		{
			centers.push_back(points.at(randoms.at(i)));
		}
		// Print centers
		for (int i = 0; i < no_clusters; i++)
		{
			cout << "Center [" << centers.at(i).first.x << ", " << centers.at(i).first.y << "] - " << centers.at(i).second << endl;
		}

		vector<Vec3b> labels(no_of_points);
		int iter_counter = 0;
		vector<pair<Vec3b, int>> cl_sizes(no_clusters);

		labels[0] = -1; // Magic
		bool labeling_changed;

		const clock_t begin_time = clock();
		// ... Actual processing ...

		do
		{
			labeling_changed = false;
			// Assign each point to a cluster
			for (int points_index = 0; points_index < no_of_points; points_index++)
			{
				// Compute the distance from the current point to each cluster center
				float min_distance = dist(points[points_index].first, centers[0].first, points[points_index].second, centers[0].second);
				Vec3b label = centers[0].second;
				for (int cluster_index = 1; cluster_index < no_clusters; cluster_index++)
				{
					float current_distance = dist(points[points_index].first, centers[cluster_index].first, points[points_index].second, centers[cluster_index].second);
					if (current_distance < min_distance)
					{
						min_distance = current_distance;
						label = centers[cluster_index].second;	
					}
				}

				if (label != labels[points_index])
					labeling_changed = true;

				labels[points_index] = label;
			}

			// Recompute cluster centers
			for (int cluster_index = 0; cluster_index < no_clusters; cluster_index++)
			{
				centers[cluster_index].first = Point(0, 0);
				cl_sizes[cluster_index].second = 0;
			}

			// Go through all points again
			for (int points_index = 0; points_index < no_of_points; points_index++)
			{
				Vec3b c_label = labels[points_index];
				Point& p(points[points_index].first);

				for (int center_index = 0; center_index < no_clusters; center_index++)
				{
					if (centers[center_index].second == c_label)
					{
						centers[center_index].first.x += p.x;
						centers[center_index].first.y += p.y;
						cl_sizes[center_index].first = c_label;
						cl_sizes[center_index].second++;

						break;
					}
				}
			}

			// Average values
			for (int cluster_index = 0; cluster_index < no_clusters; cluster_index++)
			{
				centers[cluster_index].first.x /= cl_sizes[cluster_index].second != 0 ? cl_sizes[cluster_index].second : 1;
				centers[cluster_index].first.y /= cl_sizes[cluster_index].second != 0 ? cl_sizes[cluster_index].second : 1;
			}
			iter_counter++;
			
		} while (labeling_changed && iter_counter < MAX_ITER);

		// Assemble the destination image
		for (int points_index = 0; points_index < no_of_points; points_index++)
		{
			clone(points[points_index].first.y, points[points_index].first.x) = labels[points_index];
		}

		// Compute number of seconds spent on computing
		cout << float(clock() - begin_time) / CLOCKS_PER_SEC;
		// Display clone
		imshow("clone", clone);

		waitKey(0);
	}
}