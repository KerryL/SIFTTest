#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp> //Thanks to Alessandro
#include <opencv2/xfeatures2d/nonfree.hpp>

#include <iostream>

int main(int argc, const char* argv[])
{
	if (argc < 4)
	{
		std::cerr << "Not enough input arguments." << std::endl;
		std::cout << "Usage:  " << argv[0] << " [training image]";
		std::cout << " [search image] [result image]" << std::endl;
		return 1;
	}

    const cv::Mat trainingImage = cv::imread(argv[1], 0); // greyscale
    const cv::Mat searchImage = cv::imread(argv[2], 0); // greyscale

	//cv::Ptr<cv::BRISK> detector = cv::BRISK::create();
	cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();

    std::vector<cv::KeyPoint> trainingKeypoints;
    std::vector<cv::KeyPoint> searchKeypoints;

    detector->detect(trainingImage, trainingKeypoints);
    detector->detect(searchImage, searchKeypoints);

	cv::Mat trainingDescriptors;
	cv::Mat searchDescriptors;
	detector->compute(trainingImage, trainingKeypoints, trainingDescriptors);
	detector->compute(searchImage, searchKeypoints, searchDescriptors);

	// Find matches
	cv::BFMatcher matcher;
	//std::vector<cv::DMatch> matches;
	//matcher.match(trainingDescriptors, searchDescriptors, matches);
//	cv::FlannBasedMatcher(
	std::vector<std::vector<cv::DMatch> > matches;
	const unsigned int nearestNeighbors(2);
	matcher.knnMatch(trainingDescriptors, searchDescriptors, matches,
		nearestNeighbors, 2);

	// Exctract best matches
	unsigned int i;
	std::vector<cv::DMatch> goodMatches;
	const double ratio(0.7);
	for (i = 0; i < matches.size(); i++)
	{
		if (matches[i][0].distance < matches[i][1].distance * ratio)
			goodMatches.push_back(matches[i][0]);
	}

	const unsigned int minGoodMatches(10);
	if (goodMatches.size() < minGoodMatches)
	{
		std::cerr << "Found only " << goodMatches.size() << " matches";
		std::cerr << " (minimum is " << minGoodMatches << ")" << std::endl;
		return 1;
	}

	// Extract matches from the images
	// TODO:  Continue implementing from here:
	// http://docs.opencv.org/trunk/doc/py_tutorials/py_feature2d/py_feature_homography/py_feature_homography.html

    // Add results to image and save
    cv::Mat output;
    cv::drawKeypoints(searchImage, searchKeypoints, output);
    cv::imwrite(argv[3], output);

    return 0;
}
