#ifndef _SDFBASEDCOLLISIONDETECTION_H 
#define _SDFBASEDCOLLISIONDETECTION_H

#include "Discregrid/All"
#include "utility.h"
#include <iostream>
#include <memory>


class SDFbasedCollisionDetection
{
public:
	using Grid = Discregrid::CubicLagrangeDiscreteGrid;
	using GridPtr = std::shared_ptr<Discregrid::CubicLagrangeDiscreteGrid>;

	SDFbasedCollisionDetection(std::string sdfFile = "resources/sdf/tanzhen.sdf");
	virtual ~SDFbasedCollisionDetection();

	bool penetration(const Eigen::Vector3d &x, Eigen::Vector3d &penetration);

	std::string m_sdfFile;
	GridPtr m_sdf;
};



#endif