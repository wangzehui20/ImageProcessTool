#include "Sam.h"

class Interpretation
{
public:
	bool getInferInput(std::list<cv::Point3i> clickedPoints, std::list<cv::Point>& positvePoints, std::list<cv::Point>& negtivePoints, cv::Rect& roi);
	bool parseDeviceName(const std::string& name, Sam::Parameter::Provider& provider);
	cv::Mat infer(cv::Mat image, int step);
	cv::Mat infer(cv::Mat image, std::list<cv::Point3i> clickedPoints);
};