#include <osg/NodeCallback>
#include <osg/AutoTransform>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgGA/TrackballManipulator>
#include "WorkingModel.h"
#include <iostream>
#include <string>

using namespace std;

osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);

void setPosition(osg::Vec4d& speed, osg::AutoTransform* model, osgViewer::Viewer& viewer);
void setAttitude(double angle, osg::Vec3f axis, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void yaw(double angle, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void roll(double angle, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void pitch(double angle, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer);
void setCamera(osg::Vec4d& speed, osgViewer::Viewer& viewer);
void attitudeCamera(double angle, osg::Vec3f axis, osg::AutoTransform* model, osgViewer::Viewer& viewer);

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
    osg::AutoTransform* f16TrasformNode = NULL;
	osgViewer::Viewer viewer;

	combinedModel = osgDB::readNodeFile("converted.osg");
	f16 = findNamedNode(std::string("f16base"), combinedModel);
	osgGA::TrackballManipulator *Tman = new osgGA::TrackballManipulator();

    f16TrasformNode = dynamic_cast<osg::AutoTransform*>(f16->asGroup()->getChild(0));
	
	if(!f16TrasformNode){
        std::cout << "AutoTransform not found!";
		return -1;
	}
	
    WorkingModel model(f16TrasformNode, Tman);

	viewer.setCameraManipulator(Tman);
    viewer.setSceneData( combinedModel );
	viewer.realize();
	

	viewer.home();

    //yaw(-90.0, f16TrasformNode, speed, viewer);
    //pitch(90.0, f16TrasformNode, speed, viewer);
    //roll(-90.0, f16TrasformNode, speed, viewer);

	//model.yaw(-90.0);

	model.rotateX(-90.0);
	model.rotateX(-90.0);
	model.rotateY(10.0);

	//model.rotateY(90.0);
	//model.rotateX(-20.0);
	//model.rotateY(-10.0);
	//model.rotateY(-10.0);
	//model.rotateY(-10.0);
	//model.rotateY(10.0);
	//model.rotateY(10.0);

	while( !viewer.done() )
	{		

		//model.rotateY(-0.0000001);

		model.nextPosition();
		
        viewer.home();
		viewer.frame();
	}

}

void setPosition(osg::Vec4d& speed, osg::AutoTransform* model, osgViewer::Viewer& viewer){
	double x, y, z;

	x = model->getPosition().x() + speed.x();
	y = model->getPosition().y() + speed.y();
	z = model->getPosition().z() + speed.z();

    //std::cout << "x:" << speed.x() << " y:" << speed.y() << " z:" << speed.z() << std::endl;
	
	model->setPosition(osg::Vec3d(x, y, z));

	setCamera(speed, viewer);
}

void setAttitude(double angle, osg::Vec3f axis, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
    osg::Quat rotation = model->getRotation();
    osg::Quat newrotation;
    osg::Matrixd rotSpeed;

    newrotation.makeRotate(osg::DegreesToRadians(angle), axis);

    //std::cout << "x:" << rotation.x() << " y:" << rotation.y() << " z:" << rotation.z() << " w:" << rotation.w() << std::endl;
    rotation = rotation*newrotation;
    //std::cout << "x:" << rotation.x() << " y:" << rotation.y() << " z:" << rotation.z() << " w:" << rotation.w() << std::endl;

    model->setRotation(rotation);

    //std::cout << "x:" << rotation.x() << " y:" << rotation.y() << " z:" << rotation.z() << " w:" << rotation.w() << std::endl;

    //model->setMatrix(Matrix::rotate(angle, axis));

	//std::cout << speed.x() << " " << speed.y() << " " << speed.z() << std::endl;
	
	//rotSpeed.setRotate(osg::DegreesToRadians(angle), axis);
    //rotSpeed.makeRotate(osg::DegreesToRadians(angle), axis);
	//std::cout << "old rotSpeed " << speed.x() << " " << speed.y() << " " << speed.z() << " " << speed.w() << std::endl;
	speed = speed*rotSpeed;
    //std::cout << "Speed " << speed.x() << " " << speed.y() << " " << speed.z() << " " << speed.w() << std::endl;

    //attitudeCamera(angle, axis, model, viewer);
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

void attitudeCamera(double angle, osg::Vec3f axis, osg::AutoTransform* model, osgViewer::Viewer& viewer){
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

	//std::cout << modelPos.x() << " " << modelPos.y() << " " << modelPos.z() << std::endl;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;

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
	

	//eye.set(5000.0, -25.0, 1010.0);
	//center.set(5000.0, 5000.0, 0.0);
	
    up = up*rotateM;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;


	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

	manipulator->setHomePosition(eye, center, up);
}

void yaw(double angle, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::Z_AXIS, model, speed, viewer);
}

void roll(double angle, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
	setAttitude(angle, osg::X_AXIS, model, speed, viewer);
}

void pitch(double angle, osg::AutoTransform* model, osg::Vec4d& speed, osgViewer::Viewer& viewer){
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
