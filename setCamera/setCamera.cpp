#include <osg/NodeCallback>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgGA/TrackballManipulator>
#include "KeyboardHandler.h"
#include <iostream>
#include <string>

#define INITIAL_POS_X 5000.0
#define INITIAL_POS_Y 1000.0

osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);

void setPosition(osg::Vec4d& speed, osg::PositionAttitudeTransform* model, osgViewer::Viewer& viewer);
void setAttitude(double angle, osg::Vec3f axis, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void yaw(double angle, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void roll(double angle, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void pitch(double angle, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void setCamera(osg::Vec4d& speed, osgViewer::Viewer& viewer);
void attitudeCamera(double angle, osg::Vec3f axis, osg::PositionAttitudeTransform* model, osgViewer::Viewer& viewer);

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
	osg::PositionAttitudeTransform* f16TrasformNode = NULL;
	osgViewer::Viewer viewer;

	osg::Vec3d  eye = osg::Vec3d(INITIAL_POS_X+25, 0.0, INITIAL_POS_Y+10);
	osg::Vec3d  center = osg::Vec3d(0.0,0.0,0.0);
	osg::Vec3d  up = osg::Vec3d(0.0, 0.0, 1.0);
	osg::Quat f16_attitude(osg::DegreesToRadians(90.0), osg::X_AXIS);
	osg::Vec4d speed(-2.0,0.0,0.0, 1.0);

	combinedModel = osgDB::readNodeFile("converted.osg");
	f16 = findNamedNode(std::string("f16base"), combinedModel);

	f16TrasformNode = dynamic_cast<osg::PositionAttitudeTransform*>(f16->asGroup()->getChild(0));
	
	if(!f16TrasformNode){
		std::cout << "PositionAttitudeTransform not found!";
		return -1;
	}

	f16TrasformNode->setPosition( osg::Vec3d(INITIAL_POS_X, 0.0, INITIAL_POS_Y) );
	f16TrasformNode->setAttitude(f16_attitude);
	
	//osg::Quat(osg::DegreesToRadians(90.0), osg::Vec3(1,0,0) ) );

	osgGA::TrackballManipulator *Tman = new osgGA::TrackballManipulator();
	viewer.setCameraManipulator(Tman);
	viewer.setSceneData( combinedModel );
	viewer.realize();
	Tman->setHomePosition(eye, center, up);

	//keyboardEventHandler* keh = new keyboardEventHandler();
	//keh->addFunction('v', &toggleView);
	//viewer.addEventHandler(keh); 
	viewer.home();

	yaw(90.0, f16TrasformNode, speed, viewer);

	while( !viewer.done() )
	{		
		setPosition(speed, f16TrasformNode, viewer);

		viewer.home();
		
		viewer.frame();
	}

}

void setPosition(osg::Vec4d& speed, osg::PositionAttitudeTransform* model, osgViewer::Viewer& viewer){
	double x, y, z;

	x = model->getPosition().x() + speed.x();
	y = model->getPosition().y() + speed.y();
	z = model->getPosition().z() + speed.z();

	//std::cout << speed.x() << " " << speed.y() << " " << speed.z() << std::endl;
	
	model->setPosition(osg::Vec3d(x, y, z));

	setCamera(speed, viewer);
}

void setAttitude(double angle, osg::Vec3f axis, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
	osg::Quat attitude = model->getAttitude();
	osg::Quat rotation(0,0,0,0);
	osg::Matrixd rotSpeed;

	rotation.makeRotate(osg::DegreesToRadians(angle), axis);

	attitude *= rotation;
	model->setAttitude(attitude);
	
	//std::cout << speed.x() << " " << speed.y() << " " << speed.z() << std::endl;
	
	//rotSpeed.setRotate(osg::DegreesToRadians(angle), axis);
	rotSpeed.makeRotate(osg::DegreesToRadians(angle), axis);
	//std::cout << "old rotSpeed " << speed.x() << " " << speed.y() << " " << speed.z() << " " << speed.w() << std::endl;
	speed = speed*rotSpeed;
	//std::cout << "new rotSpeed " << speed.x() << " " << speed.y() << " " << speed.z() << " " << speed.w() << std::endl;

	attitudeCamera(angle, axis, model, viewer);
}

void setCamera(osg::Vec4d& speed, osgViewer::Viewer& viewer){
	osg::Vec3d eye, center, up;
	osgGA::CameraManipulator* manipulator = viewer.getCameraManipulator();

	if(manipulator == NULL){
		return;
	}

	manipulator->getHomePosition(eye, center, up);
	//std::cout << center.x() << std::endl;
	//std::cout << eye.x() << std::endl;

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

	manipulator->setHomePosition(eye, center, up);
}

void attitudeCamera(double angle, osg::Vec3f axis, osg::PositionAttitudeTransform* model, osgViewer::Viewer& viewer){
	osg::Vec3d modelPos = model->getPosition();
	osg::Vec3d camPos();
	osg::Matrixd translateM1;
	osg::Matrixd translateM2;
	osg::Matrixd rotateM;

	osg::Vec3d eye, center, up;
	osgGA::CameraManipulator* manipulator = viewer.getCameraManipulator();

	if(manipulator == NULL){
		return;
	}

	manipulator->getHomePosition(eye, center, up);

	translateM2.setTrans(modelPos.x(), modelPos.y(), modelPos.z());
	translateM1.setTrans(-modelPos.x(), -modelPos.y(), -modelPos.z());
	rotateM.makeRotate(osg::DegreesToRadians(angle), axis);

	std::cout << modelPos.x() << " " << modelPos.y() << " " << modelPos.z() << std::endl;
	std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;

	//eye = translateM1*rotateM*translateM2*eye;
	eye = eye*translateM1;
	std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	eye = eye*rotateM;
	std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	eye = eye*translateM2;
	std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;

	center = center*translateM1;
	center = rotateM*center;
	center = center*translateM2;

	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

	//manipulator->setHomePosition(eye, center, up);
}

void yaw(double angle, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::Z_AXIS, model, speed, viewer);
}

void roll(double angle, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::X_AXIS, model, speed, viewer);
}

void pitch(double angle, osg::PositionAttitudeTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
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