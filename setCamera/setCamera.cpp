#include <osg/NodeCallback>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgGA/TrackballManipulator>
#include "KeyboardHandler.h"

osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);

void setVelocity(osg::Quat& speed, osg::PositionAttitudeTransform* model);
void setAttitude(double angle, osg::Vec3f axis, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer);
void yaw(double angle, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer);
void roll(double angle, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer);
void pitch(double angle, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer);

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
	double position_x = 5000.0;
	double centerx = 0;
	double eye_z = 1015.0;
	double speed = 4;
	double angle = 90.0;
	osg::Vec3d* f16posit = new osg::Vec3d(position_x, 0.0, 1007.0);
	osg::Vec3d  eye = osg::Vec3d(position_x+25, 0.0, eye_z);
	osg::Vec3d  centre = osg::Vec3d(0.0,0.0,0.0);
	osg::Vec3d  up = osg::Vec3d(0.0, 0.0, 1.0);
	osg::Quat f16_attitude(osg::DegreesToRadians(90.0), osg::X_AXIS);
	//osg::Quat* speed = new osg::Quat(8,0,0,0);

	combinedModel = osgDB::readNodeFile("converted.osg");
	f16 = findNamedNode(std::string("f16base"), combinedModel);

	f16TrasformNode = dynamic_cast<osg::PositionAttitudeTransform*>(f16->asGroup()->getChild(0));
	
	if(!f16TrasformNode){
		std::cout << "PositionAttitudeTransform not found!";
		return -1;
	}

	f16TrasformNode->setPosition( *f16posit );
	f16TrasformNode->setAttitude(f16_attitude);
	//osg::Quat(osg::DegreesToRadians(90.0), osg::Vec3(1,0,0) ) );

	osgGA::TrackballManipulator *Tman = new osgGA::TrackballManipulator();
	viewer.setCameraManipulator(Tman);
	viewer.setSceneData( combinedModel );
	viewer.realize();

	//keyboardEventHandler* keh = new keyboardEventHandler();
	//keh->addFunction('v', &toggleView);
	//viewer.addEventHandler(keh); 
	viewer.home();
	while( !viewer.done() )
	{

		if (manuallyPlaceCamera)
		{
			position_x -= speed;
			centerx -= speed;
			angle += 3;

			f16posit->set(position_x, 0.0, 1007.0);
			f16TrasformNode->setPosition( *f16posit );
			
			eye.set(position_x+25, 0.0, eye_z);
			centre.set(centerx, 0 , 0);
			
			f16_attitude.makeRotate(osg::DegreesToRadians(angle), osg::X_AXIS);
			f16TrasformNode->setAttitude(f16_attitude);

			Tman -> setHomePosition(eye, centre, up, false);
			viewer.home();
		}

		viewer.frame();
	}


}

void setVelocity(osg::Quat& speed, osg::PositionAttitudeTransform* model){
	double x, y, z;

	x = model->getPosition().x() + speed.x();
	y = model->getPosition().y() + speed.y();
	z = model->getPosition().z() + speed.z();

	model->setPosition(osg::Vec3d(x, y, z));
}

void setAttitude(double angle, osg::Vec3f axis, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer){
	osg::Quat attitude = model->getAttitude();

	attitude.makeRotate(osg::DegreesToRadians(angle), axis);
	speed.makeRotate(osg::DegreesToRadians(angle), axis);

	model->setAttitude(attitude);

	setVelocity(speed, model);
}

void yaw(double angle, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::Z_AXIS, model, speed, viewer);
}

void roll(double angle, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::X_AXIS, model, speed, viewer);
}

void pitch(double angle, osg::PositionAttitudeTransform* model, osg::Quat& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::Y_AXIS, model, speed, viewer);
}

void setCamera(osg::PositionAttitudeTransform* model, osg::Quat& speed , bool cockpitMode, osgViewer::Viewer& viewer){
	osg::Vec3d eye, center, up;
	osgGA::CameraManipulator* manipulator = viewer.getCameraManipulator();

	manipulator->getHomePosition(eye, center, up);

	//center part
	double curx = center.x() + speed.x(); 
	double cury = center.y() + speed.y();
	double curz = center.z() + speed.z();
	center.set(curx, cury, curz);

	//eye part

	//up part

	manipulator->setHomePosition(eye, center, up);
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