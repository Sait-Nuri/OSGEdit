#ifndef WorkingModel_H
#define WorkingModel_H

#include <osg/AutoTransform>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>

#define INITIAL_POS_X 5000.0
#define INITIAL_POS_Y 1000.0

class WorkingModel
{
      private :
      	osgGA::TrackballManipulator *_trballman; 	// Camera settings
		osg::AutoTransform* _model;			// Model node
		osg::Matrixd _modelM;
		osg::Quat _firstRot;
		osg::Vec3d _firstSpeed;
		osg::Matrixd _R;					// rotation matrix accumulator
		osg::Matrixd _translateM1;
		osg::Matrixd _translateM2;
		osg::Vec3d _speedV;					// speed vector
		osg::Vec3d _eye;
		osg::Vec3d _center;
		osg::Vec3d _up;
		osg::Vec3d _first_eye;
		osg::Vec3d _first_center;
		osg::Vec3d _first_up;
		
		void rotateSpeedV();
		void setRotate();
	    void rotateModel();
	    void rotateCamera();
	    void setPositionModel();
	    void setPositionCamera();


      public :
	    //Constructor
	    WorkingModel(osg::AutoTransform*, osgGA::TrackballManipulator *);

	    // Some calibration staff
	    void init_model();

	    // Rotation function
	    void rotate(double, osg::Vec3f);
	    void rotateX(double);
	    void rotateY(double);

	    // sets next position of the model
	    void nextPosition();
	    
	    void yaw(double angle);
};
 
#endif 
