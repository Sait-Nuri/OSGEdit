#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/Transform>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgGA/TrackballManipulator>
#include "KeyboardHandler.h"
#include <iostream>
#include <string>
#include <osgDB/WriteFile>

#define INITIAL_POS_X 7000.0
#define INITIAL_POS_Z 1000.0



osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);

void setPosition(osg::Vec3d& speed, osg::MatrixTransform* model, osgViewer::Viewer& viewer);
void setAttitude(double angle, osg::Vec3f axis, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer);
void yaw(double angle, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer);
void roll(double angle, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer);
void pitch(double angle, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer);
void setCamera(osg::Matrixd& modelMatrix, osgViewer::Viewer& viewer);
void attitudeCamera(double angle, osg::Vec3f axis, osg::MatrixTransform* model, osgViewer::Viewer& viewer);

bool manuallyPlaceCamera = true;

void toggleView()
{
   if (! manuallyPlaceCamera)
      manuallyPlaceCamera = true;
   else
      manuallyPlaceCamera = false;
}

int main()
{
	osg::Node* combinedModel = NULL;
	osg::Node* f16 = NULL;
	osgViewer::Viewer viewer;
	osg::MatrixTransform* f16MatrixNode;
	osg::Matrixd translateInit;
	osg::Matrixd rotateInit;
	osg::Vec3d  eye = osg::Vec3d(INITIAL_POS_X+25, 0.0, INITIAL_POS_Z+7);
	osg::Vec3d  center = osg::Vec3d(0.0,0.0,0.0);
	osg::Vec3d  up = osg::Vec3d(0.0, 0.0, 1.0);
	osg::Vec3d speed(-4.0,0.0,0.0);
	osg::Vec3d initial(INITIAL_POS_X, 0.0, INITIAL_POS_Z);

	combinedModel = osgDB::readNodeFile("merged.osg");
	f16 = findNamedNode(std::string("f16base"), combinedModel);

	f16MatrixNode = dynamic_cast<osg::MatrixTransform*>(f16->asGroup()->getChild(0));
	
	if(!f16MatrixNode){
		std::cout << "MatrixTransform not found!";
		return -1;
	}
	
	rotateInit.makeRotate(osg::inDegrees(90.0), osg::X_AXIS );
	translateInit.makeTranslate( initial );
	f16MatrixNode->setMatrix(rotateInit*translateInit) ;

	//osg::Matrixd modelMatrix = f16MatrixNode->getMatrix();
	//osg::Matrixd translateM;

	osgGA::TrackballManipulator *Tman = new osgGA::TrackballManipulator();
	Tman->setHomePosition(eye, center, up);

	viewer.setCameraManipulator(Tman);
	viewer.setSceneData( combinedModel );
	viewer.realize();
	viewer.home();

	//yaw(-180.0, f16MatrixNode, speed, viewer);
	//pitch(-90.0, f16MatrixNode, speed, viewer);
	//yaw(-10.0, f16MatrixNode, speed, viewer);
	yaw(90.0, f16MatrixNode, speed, viewer);
	//roll(-10.0, f16MatrixNode, speed, viewer);

	while( !viewer.done() )
	{		
		//translateM.setTrans(speed.x(), speed.y(), speed.z());
		//std::cout << "Speed " << speed.x() << " " << speed.y() << " " << speed.z() <<  std::endl;
		//modelMatrix = modelMatrix*translateM;
		//std::cout << "Speed " << speed.x() << " " << speed.y() << " " << speed.z() <<  std::endl;
		//f16MatrixNode->setMatrix(modelMatrix);

		setPosition(speed, f16MatrixNode, viewer);

		//yaw(0.1, f16MatrixNode, speed, viewer);
		viewer.home();		
		viewer.frame();
	}

	//osgDB::writeNodeFile( *combinedModel, "result.osg");

}

void setPosition(osg::Vec3d& speed, osg::MatrixTransform* model, osgViewer::Viewer& viewer){
	osg::Matrixd translateM;
	osg::Matrixd modelMatrix = model->getMatrix();

	translateM.setTrans(speed.x(), speed.y(), speed.z());

	modelMatrix = modelMatrix*translateM;
	
	model->setMatrix(modelMatrix);

	osg::Vec3d loc = modelMatrix.getTrans();

	std::cout << "location: " << loc.x() << " " << loc.y() << " " << loc.z() <<  std::endl;

	setCamera(modelMatrix, viewer);
}

// Sets camera position for each frame
void setCamera(osg::Matrixd& modelMatrix, osgViewer::Viewer& viewer){
	osg::Vec3d eye, center, up;
	osg::Matrixd translateEye;
	osg::Matrixd translateCenter;

	osgGA::CameraManipulator* manipulator = viewer.getCameraManipulator();

	if(manipulator == NULL){
		return;
	}

	translateEye.makeTranslate(osg::Vec3d(25.0, 0.0, 7.0));
	translateCenter.makeTranslate(osg::Vec3d(-INITIAL_POS_X, 0, -INITIAL_POS_Z));

	manipulator->getHomePosition(eye, center, up);
	//modelMatrix.getLookAt(eye, center, up);

	eye = (modelMatrix*translateEye).getTrans();
	center = (modelMatrix*translateCenter).getTrans();

	std::cout << "eye: " << eye.x() << std::endl;
	std::cout << "center: " << center.x() << std::endl;

	/*
	//center part
	double curCx = center.x() + speed.x(); 
	double curCy = center.y() + speed.y();
	double curCz = center.z() + speed.z();
	center.set(curCx, curCy, curCz);

	//eye part
	double curEx = eye.x() + speed.x(); 
	double curEy = eye.y() + speed.y();
	double curEz = eye.z() + speed.z();
	eye.set(curEx, curEy, curEz);

	//up part
	*/
	manipulator->setHomePosition(eye, center, up);
}

// transformation of model and velocity matrix
void setAttitude(double angle, osg::Vec3f axis, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer){
	osg::Matrixd modelMatrix = model->getMatrix();
	osg::Matrixd rotSpeed;
	osg::Matrixd rotation;
	//osg::Quad rot;

	rotation.makeRotate(osg::DegreesToRadians(angle), axis);

	modelMatrix = rotation*modelMatrix;

	model->setMatrix(modelMatrix);

	rotSpeed.makeRotate(osg::DegreesToRadians(angle), axis);
	//std::cout << "old rotSpeed " << speed.x() << " " << speed.y() << " " << speed.z() << " " << speed.w() << std::endl;
	//std::cout << "Speed " << speed.x() << " " << speed.y() << " " << speed.z() << " " << speed.w() << std::endl;
	speed = speed*rotSpeed;
	std::cout << "Speed " << speed.x() << " " << speed.y() << " " << speed.z() << std::endl;

	//attitudeCamera(angle, axis, model, viewer);
}


// transform camera matrix
void attitudeCamera(double angle, osg::Vec3f axis, osg::MatrixTransform* model, osgViewer::Viewer& viewer){
	osgGA::CameraManipulator* manipulator = viewer.getCameraManipulator();
	osg::Matrixd modelMatrix = model->getMatrix();

	osg::Vec3d modelPos = modelMatrix.getTrans();
	osg::Matrixd translateM1;
	osg::Matrixd translateM2;
	osg::Matrixd rotateM;

	osg::Vec3d eye, center, up;

	if(manipulator == NULL){
		return;
	}

	manipulator->getHomePosition(eye, center, up);

	translateM2.setTrans(modelPos.x(), modelPos.y(), modelPos.z());
	translateM1.setTrans(-modelPos.x(), -modelPos.y(), -modelPos.z());
	rotateM.makeRotate(osg::DegreesToRadians(angle), axis);


	
	//std::cout << "model position: "<< modelPos.x() << " " << modelPos.y() << " " << modelPos.z() << std::endl;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << "center: " << center.x() << " " << center.y() << " " << center.z() << std::endl;
	//std::cout << "eye: " << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//center.set(center.x()+10, center.y(), center.z());
	
	eye = eye*translateM1;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	eye = eye*rotateM;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	eye = eye*translateM2;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	
	
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	center = center*translateM1;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	center = center*rotateM;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	center = center*translateM2;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	
	//std::cout << "model position: "<< modelPos.x() << " " << modelPos.y() << " " << modelPos.z() << std::endl;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << "center: " << center.x() << " " << center.y() << " " << center.z() << std::endl;
	//std::cout << "eye: " << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;

	//eye.set(0.0, 7025.0, 1007.0);
	//center.set(5000.0, 5000.0, 0.0);
	
	//up = rotateM*up;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;


	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

	manipulator->setHomePosition(eye, center, up);
}

void yaw(double angle, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::Z_AXIS, model, speed, viewer);
}

void roll(double angle, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::X_AXIS, model, speed, viewer);
}

void pitch(double angle, osg::MatrixTransform* model, osg::Vec3d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::Y_AXIS, model, speed, viewer);
}



osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode)
{
   osg::Group* currGroup;
   osg::Node* foundNode;

   // check to see if we have a valid (non-NULL) node.
   // if we do have a null node, return NULL.
   if ( !currNode)
   {
      return NULL;
   }

   // We have a valid node, check to see if this is the node we 
   // are looking for. If so, return the current node.
   if (currNode->getName() == searchName)
   {
      return currNode;
   }

   // We have a valid node, but not the one we are looking for.
   // Check to see if it has children (non-leaf node). If the node
   // has children, check each of the child nodes by recursive call.
   // If one of the recursive calls returns a non-null value we have
   // found the correct node, so return this node.
   // If we check all of the children and have not found the node,
   // return NULL
   currGroup = currNode->asGroup(); // returns NULL if not a group.
   if ( currGroup ) 
   {
      for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
      { 
         foundNode = findNamedNode(searchName, currGroup->getChild(i));
         if (foundNode)
            return foundNode; // found a match!
      }
      return NULL; // We have checked each child node - no match found.
   }
   else 
   {
      return NULL; // leaf node, no match 
   }
}