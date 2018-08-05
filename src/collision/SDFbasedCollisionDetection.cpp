#include "SDFbasedCollisionDetection.h"

SDFbasedCollisionDetection::SDFbasedCollisionDetection(std::string sdfFile)
{
	m_sdfFile = sdfFile;
	m_sdf = std::make_shared<Grid>(m_sdfFile);
}

SDFbasedCollisionDetection::~SDFbasedCollisionDetection()
{

}

bool SDFbasedCollisionDetection::penetration(const Eigen::Vector3d &x, Eigen::Vector3d &penetration)
{
	Eigen::Vector3d grad;
	auto val = m_sdf->interpolate(0, x, &grad);
	grad.normalize();
	if (val < 0) {
		std::cout << std::endl << "val: " << val << std::endl;
		std::cout << std::endl << "grad: " << std::endl << grad << std::endl;
		penetration = grad * val;
		return true;
	}
	return false;
}