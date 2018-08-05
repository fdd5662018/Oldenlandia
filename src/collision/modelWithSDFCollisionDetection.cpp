#include "modelWithSDFCollisionDetection.h"
#include <iostream>

Vector4r GLM2Eigen(const glm::vec3& glm_vector)
{
	return Vector4r(glm_vector[0], glm_vector[1], glm_vector[2], 1.0);
}

Vector4r EigenVec2Matrix(const Eigen::Vector3d& eigen_vector)
{
	return Vector4r(eigen_vector[0], eigen_vector[1], eigen_vector[2], 1.0);
}

Vector3r EigenV42V3(const Vector4r& eigen_vector)
{
	return Vector3r(eigen_vector[0], eigen_vector[1], eigen_vector[2]);
}


glm::vec3 Eigen2GLM(const Eigen::Vector3d& eigen_vector)
{
	return glm::vec3(eigen_vector[0], eigen_vector[1], eigen_vector[2]);
}

modelWithSDFCollisionDetection::modelWithSDFCollisionDetection(ClassObject *f_p_SDFObject, SDFbasedCollisionDetection *f_p_CSdf, ClassObject *f_p_envObject)
{
	m_p_SDFObject = f_p_SDFObject;
	m_p_CSdf = f_p_CSdf;
	m_p_envObject = f_p_envObject;
}

modelWithSDFCollisionDetection::~modelWithSDFCollisionDetection()
{
}

void modelWithSDFCollisionDetection::traverseAllverticesInModelAndProcessPenetration()
{
	// std::cout << "------------------------->" << __LINE__ << std::endl;
	Matrix4r modelEnv(&(m_p_envObject->mModelMatrix[0][0]));
	Matrix4r modelSDF(&(m_p_SDFObject->mModelMatrix[0][0]));
	Matrix4r inverseModelSDF = modelSDF.inverse();
	Matrix4r model = inverseModelSDF * modelEnv;
	// for penetration
	Matrix4r inverseModelEnv = modelEnv.inverse();
	Matrix4r modelTransformPenetrationFromSDFLocalToEnvLocal = inverseModelEnv * modelSDF;

	vector<Mesh> &meshes = m_p_envObject->mModelRenderer->mpModel->meshes;
	//std::cout << "mesh size "<<meshes.size() << std::endl;
	for (int i = 0; i < meshes.size(); i++) {
		vector<Vertex> &vertices = meshes[i].vertices;
		for (int j = 0; j < vertices.size(); j++) {
			Vector4r v4Eigen = GLM2Eigen(vertices[j].Position);
			v4Eigen = model * v4Eigen;
			Eigen::Vector3d v3Eigen(v4Eigen[0], v4Eigen[1], v4Eigen[2]);
			//std::cout << v3Eigen << ",";
			Eigen::Vector3d penetration;
			if (m_p_CSdf->penetration(v3Eigen, penetration)) {
				std::cout << std::endl << "penetration: " << std::endl << penetration << std::endl;
				Vector4r v4Eigen = modelTransformPenetrationFromSDFLocalToEnvLocal * EigenVec2Matrix(penetration);
				Eigen::Vector3d v3Eigen(v4Eigen[0], v4Eigen[1], v4Eigen[2]);
				vertices[j].Position += Eigen2GLM(v3Eigen);
			}
		}
	}
}