#ifndef WorkingModel_H
#define WorkingModel_H

#include <osg/AutoTransform>
#include <osg/MatrixTransform>

class WorkingModel
{
      private :
		osg::Quat currentRotation;
		osg::AutoTransform* model;
     	osg::Vec3d X_AXIS;
     	osg::Vec3d Y_AXIS;
     	osg::Vec3d Z_AXIS;

      public :
	    //Constructor
	    WorkingModel(osg::AutoTransform*);

		void setWorkRotation(osg::Quat& newRotation);
	    osg::Quat& getWorkRotation();
       
	    osg::AutoTransform* getModel();
       
};
 
#endif 
