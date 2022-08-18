#include "AlignmentTransformation.h"

#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>

#include <cmath>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHDataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/PHTimer.h>

#include <Acts/Surfaces/PerigeeSurface.hpp>
#include <Acts/Surfaces/PlaneSurface.hpp>
#include <Acts/Surfaces/Surface.hpp>




void AlignmentTransformation::createMap(PHCompositeNode* topNode)
{ 
  std::cout << "Entering AlignmentTransformation::createMap..." << std::endl;

  createNodes(topNode);

  // Define Parsing Variables
  TrkrDefs::hitsetkey hitsetkey = 0;
  float alpha = 0.0, beta = 0.0, gamma = 0.0, dx = 0.0, dy = 0.0, dz = 0.0;
  
  // load alignment constants file
  std::ifstream datafile("data.txt");

  int fileLines = 1808;
  for (int i=0; i<fileLines; i++)
     {
      datafile >> hitsetkey >> alpha >> beta >> gamma >> dx >> dy >> dz;

      // Perturbation translations and angles for stave and sensor
      Eigen::Vector3d sensorAngles (alpha,beta,gamma);  
      Eigen::Vector3d millepedeTranslation(dx,dy,dz); 

      unsigned int trkrId = TrkrDefs::getTrkrId(hitsetkey); // specify between detectors


      if(trkrId == TrkrDefs::tpcId )
	{
	  for(unsigned int subsurfkey=0; subsurfkey<10; subsurfkey++)
	    {
	      Eigen::Matrix4d transform = makeTransform(hitsetkey, millepedeTranslation, sensorAngles, subsurfkey);

	      transformMap->addTransform(hitsetkey,subsurfkey,transform);
	    }
	}
      else 
	{
          unsigned int subsurfkey = 0;

          Eigen::Matrix4d transform = makeTransform(hitsetkey, millepedeTranslation, sensorAngles,subsurfkey);

          transformMap->addTransform(hitsetkey,subsurfkey,transform);
	}

      if(localVerbosity == true )
	{
	  std::cout << i << " " <<hitsetkey << " " <<alpha<< " " <<beta<< " " <<gamma<< " " <<dx<< " " <<dy<< " " <<dz << std::endl;

	  transformMap->identify();
	}

     } 
}


Eigen::Matrix3d AlignmentTransformation::rotateToGlobal(TrkrDefs::hitsetkey hitsetkey, unsigned int subsurfkey)
{  
  /*
   Get ideal geometry rotation, by aligning surface to surface normal vector in global coordinates
   URL: https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
  */  

  unsigned int trkrId      = TrkrDefs::getTrkrId(hitsetkey); // specify between detectors
  ActsSurfaceMaps surfMaps = m_tGeometry->maps();
  Surface surf;

  if(trkrId == TrkrDefs::mvtxId or trkrId == TrkrDefs::inttId)
    {
      surf = surfMaps.getSiliconSurface(hitsetkey);
    }
  else if(trkrId == TrkrDefs::tpcId)
    {
      surf = surfMaps.getTpcSurface(hitsetkey,subsurfkey);
    }
  else
    {
      std::cout << "Invalid hitsetkey";
      exit(1);
    }

  Eigen::Vector3d ylocal(0,1,0);
  Eigen::Vector3d sensorNormal    = -surf->normal(m_tGeometry->geometry().geoContext);
  sensorNormal                    = sensorNormal/sensorNormal.norm(); // make unit vector 
  double cosTheta                 = ylocal.dot(sensorNormal);
  double sinTheta                 = (ylocal.cross(sensorNormal)).norm();
  Eigen::Vector3d vectorRejection = (sensorNormal - (ylocal.dot(sensorNormal))*ylocal)/(sensorNormal - (ylocal.dot(sensorNormal))*ylocal).norm();
  Eigen::Vector3d perpVector      =  sensorNormal.cross(ylocal);

  // Initialize and fill matrices (row,col)
  Eigen::Matrix3d fInverse;
  fInverse(0,0) = ylocal(0);
  fInverse(1,0) = ylocal(1);
  fInverse(2,0) = ylocal(2);
  fInverse(0,1) = vectorRejection(0);
  fInverse(1,1) = vectorRejection(1); 
  fInverse(2,1) = vectorRejection(2);
  fInverse(0,2) = perpVector(0);
  fInverse(1,2) = perpVector(1);
  fInverse(2,2) = perpVector(2);
  
  Eigen::Matrix3d G;
  G(0,0) =  cosTheta;
  G(0,1) = -sinTheta;
  G(0,2) =  0;
  G(1,0) =  sinTheta;
  G(1,1) =  cosTheta;
  G(1,2) =  0;
  G(2,0) =  0;
  G(2,1) =  0;
  G(2,2) =  1;

  Eigen::Matrix3d globalRotation = fInverse * G * (fInverse.inverse()); 

  if(localVerbosity == true)
    {
      std::cout<< " global rotation: "<< std::endl << globalRotation <<std::endl;
    }
  return globalRotation;
}



Eigen::Matrix4d AlignmentTransformation::makeAffineMatrix(Eigen::Matrix3d rotationMatrix, Eigen::Vector3d translationVector)
{
  // Creates 4x4 affine matrix given rotation angles about each axis and translationVector 
  Eigen::Matrix4d affineMatrix;
  affineMatrix.setIdentity();  // set bottom row of matrix
  affineMatrix.block<3,3>(0,0) = rotationMatrix;
  affineMatrix.block<3,1>(0,3) = translationVector;
  return affineMatrix.matrix();
}


 Eigen::Matrix4d AlignmentTransformation::makeTransform(TrkrDefs::hitsetkey hitsetkey, Eigen::Vector3d millepedeTranslation, Eigen::Vector3d sensorAngles, unsigned int subsurfkey)
{
  unsigned int trkrId      = TrkrDefs::getTrkrId(hitsetkey); // specify between detectors
  ActsSurfaceMaps surfMaps = m_tGeometry->maps();
  
  // Create aligment rotation matrix
  Eigen::AngleAxisd alpha(sensorAngles(0), Eigen::Vector3d::UnitX());
  Eigen::AngleAxisd beta(sensorAngles(1), Eigen::Vector3d::UnitY());
  Eigen::AngleAxisd gamma(sensorAngles(2), Eigen::Vector3d::UnitZ());
  Eigen::Quaternion<double> q       = gamma*beta*alpha;
  Eigen::Matrix3d millepedeRotation = q.matrix();

  // Create ideal rotation matrix from ActsGeometry
  Eigen::Matrix3d globalRotation    = AlignmentTransformation::rotateToGlobal(hitsetkey,subsurfkey);
  Eigen::Matrix3d combinedRotation  = globalRotation * millepedeRotation;
  Surface surf;
  if(trkrId == TrkrDefs::mvtxId or trkrId == TrkrDefs::inttId)
    {
      surf = surfMaps.getSiliconSurface(hitsetkey);
    }
  else if(trkrId == TrkrDefs::tpcId)
    {
      surf = surfMaps.getTpcSurface(hitsetkey,subsurfkey);

    }
  else
    {
      std::cout << "Invalid hitsetkey";
      exit(1);
    }

  Eigen::Vector3d sensorCenter      = surf->center(m_tGeometry->geometry().geoContext)*0.1;
  Eigen::Vector3d globalTranslation = sensorCenter + millepedeTranslation;
  Eigen::Matrix4d transformation    = AlignmentTransformation::makeAffineMatrix(combinedRotation,globalTranslation);

  
  if(localVerbosity == true)
    {
      std::cout << "sensor center: " << sensorCenter << " millepede translation: " << millepedeTranslation <<std::endl;
      std::cout << "Transform: "<< std::endl<< transformation  <<std::endl;
    }


  return transformation;
   
}


int AlignmentTransformation::createNodes(PHCompositeNode* topNode)
{
  //​ Get a pointer to the top of the node tree
  PHNodeIterator iter(topNode);
 
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cerr << "DST node is missing, quitting" << std::endl;
    throw std::runtime_error("Failed to find DST node in AlignmentTransformation::createNodes");
  }

  transformMap = findNode::getClass<alignmentTransformationContainer>(topNode, "alignmentTransformationContainer");
  if(!transformMap)
    {
      transformMap = new alignmentTransformationContainer;
      auto node    = new PHDataNode<alignmentTransformationContainer>(transformMap, "alignmentTransformationContainer");
      dstNode->addNode(node);
    }

  m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if(!m_tGeometry)
    {
      std::cout << "ActsGeometry not on node tree. Exiting."
		<< std::endl;
      
      return Fun4AllReturnCodes::ABORTEVENT;
    }

 return 0; 
}


