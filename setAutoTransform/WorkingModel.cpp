#include "WorkingModel.h"
#include <iostream>
#include <osgDB/WriteFile>

using namespace std;

WorkingModel::WorkingModel(osg::AutoTransform* model, osgGA::TrackballManipulator *manipulator){
	this->_model = model;
	this->_trballman = manipulator;
	_speedV = osg::Vec3d(-4.0,0.0,0.0);
	_cameraspeed = osg::Vec3d(0.0,0.0,0.0);
	_firstSpeed = osg::Vec3d(-4.0,0.0,0.0);
	init_model();
}

void WorkingModel::init_model(){

	osg::Vec3d  eye = osg::Vec3d(INITIAL_POS_X+25, 0.0, INITIAL_POS_Y+10);
	osg::Vec3d  center = osg::Vec3d(0.0,0.0,0.0);
	osg::Vec3d  up = osg::Vec3d(0.0, 0.0, 1.0);

	osg::Quat f16_rotation(osg::DegreesToRadians(90.0), osg::X_AXIS);
	
	//osgDB::writeNodeFile(*_model, "output.osg");

	_model->setPosition( osg::Vec3d(INITIAL_POS_X, 0.0, INITIAL_POS_Y) );
    _model->setRotation(f16_rotation);

    _trballman->setHomePosition(eye, center, up);

    _firstRot = _model->getRotation();

    _trballman->getHomePosition(_first_eye, _first_center, _first_up);
}

void WorkingModel::nextPosition(){

	//cout << "next position here" << endl;

	setPositionModel();
	setPositionCamera();

}

void WorkingModel::setPositionModel(){
	double x, y, z;

	x = _model->getPosition().x() + _speedV.x();
	y = _model->getPosition().y() + _speedV.y();
	z = _model->getPosition().z() + _speedV.z();

    //std::cout << "x:" << speed.x() << " y:" << speed.y() << " z:" << speed.z() << std::endl;
	
	_model->setPosition(osg::Vec3d(x, y, z));
}

void WorkingModel::setPositionCamera(){
	
	_trballman->getHomePosition(_eye, _center, _up);

	//center part
	double curCx = _center.x() + _speedV.x(); 
	double curCy = _center.y() + _speedV.y();
	double curCz = _center.z() + _speedV.z();
	_center.set(curCx, curCy, curCz);

	//eye part
	double curEx = _eye.x() + _speedV.x(); 
	double curEy = _eye.y() + _speedV.y();
	double curEz = _eye.z() + _speedV.z();
	_eye.set(curEx, curEy, curEz);

	_trballman->setHomePosition(_eye, _center, _up);

	_first_eye.set(
			_first_eye.x() + _speedV.x(),
			_first_eye.y() + _speedV.y(),
			_first_eye.z() + _speedV.z()
		);

	_first_center.set(
			_first_center.x() + _speedV.x(),
			_first_center.y() + _speedV.y(),
			_first_center.z() + _speedV.z()
		);

	//printLocationCamera();
	/*
	
	*/
	//printLocationCamera();
}

void WorkingModel::yaw(double angle){

	osg::Quat model_rot = _model->getRotation();
	
	osg::Quat _rotate;
	osg::Quat _rotate1;
	osg::Quat _rotate2;
	osg::Quat _rotate3;

	_rotate1.makeRotate(osg::DegreesToRadians(-90.0), osg::X_AXIS);
	_rotate2.makeRotate(osg::DegreesToRadians(-90.0), osg::X_AXIS);
	_rotate3.makeRotate(osg::DegreesToRadians(-90.0), osg::Y_AXIS);
	//_rotate.makeRotate(osg::DegreesToRadians(angle), osg::Y_AXIS);

	_rotate *= _rotate1;
	
	_model->setRotation(model_rot*_rotate);

	//model_rot = _model->getRotation();
	
	_rotate =   _rotate2 * _rotate;

	//model_rot *= _rotate;

	_model->setRotation(model_rot * _rotate);

	//model_rot = _model->getRotation();

	_rotate = _rotate3 * _rotate;

	//model_rot *= _rotate;

	_model->setRotation(model_rot * _rotate);
	

	//osg::Quat newrot;

	//newrot.makeRotate(osg::DegreesToRadians(45.0), osg::Y_AXIS);	
	
	//newrot *= _rotate;

	//rot *= (newrot);

	//_speedV =  _rotate * _speedV;

	//rot.makeRotate(osg::DegreesToRadians(angle), osg::Z_AXIS);

	//_model->setRotation(rot);
}
void WorkingModel::rotate(double angle, osg::Vec3f axis){
	osg::Matrixd rotate;
	//osg::Matrixd rotateB;

	rotate.makeRotate(osg::DegreesToRadians(angle), axis);
	//rotateB.makeRotate(osg::DegreesToRadians(B_angle), osg::Y_AXIS);	

	// rotation matrix computed
	_R = rotate * _R;

	rotateModel(); 	// Rotate model 
	rotateCamera();	// Rotate camera
	
	//if(axis != osg::X_AXIS)
	rotateSpeedV();
}

void WorkingModel::setRotation(double X_angle, double Y_angle){
	rotateX(X_angle);
	rotateY(Y_angle);
}

void WorkingModel::rotateX(double angle){
	rotate(angle, osg::X_AXIS);
}

void WorkingModel::rotateY(double angle){
	rotate(angle, osg::Y_AXIS);
}

void WorkingModel::rotateModel(){
	//osg::Vec3d modelPos = _model->getPosition();
	//osg::Quat model_rot = _model->getRotation();

	//osg::Matrixd modelM(model_rot);
	//_translateM2.setTrans(modelPos.x(), modelPos.y(), modelPos.z());
	//_translateM1.setTrans(-modelPos.x(), -modelPos.y(), -modelPos.z());
	_modelM.makeIdentity();

	_modelM.makeRotate(_firstRot);

	//_modelM * _translateM1;
	//_modelM *= _R;
	//_modelM * _translateM2;

	//model_rot = model_rot * _R; // Rotate model

	_model->setRotation((_modelM*_R).getRotate());
	//_model->setRotation();
}
void WorkingModel::rotateSpeedV(){

	osg::Matrixd rot;

	rot.makeRotate(osg::DegreesToRadians(-90.0), osg::X_AXIS);
	
	if(SPEED_VEC){
		std::cout << "ilk: ";
		std::cout << "x:" << _speedV.x() << " y:" << _speedV.y() << " z:" << _speedV.z()  << std::endl;
		std::cout << "-----------------------" << endl;		
	}
	

	_speedV = _firstSpeed * _R;

	//_speedV = rot.postMult(_speedV);

	if(SPEED_VEC){
		std::cout << "son: ";
		std::cout << "x:" << _speedV.x() << " y:" << _speedV.y() << " z:" << _speedV.z()  << std::endl;
		std::cout << "-----------------------" << endl;
		std::cout << endl;
	}
}
void WorkingModel::rotateCamera(){
	osg::Matrixd rot;
	osg::Vec3d modelPos = _model->getPosition();
	//_trballman->getHomePosition(_eye, _center, _up);
	//rot.makeRotate(osg::DegreesToRadians(-90.0), osg::X_AXIS);

	_translateM2.setTrans(modelPos.x(), modelPos.y(), modelPos.z());
	_translateM1.setTrans(-modelPos.x(), -modelPos.y(), -modelPos.z());

	_eye = _first_eye * _translateM1;
	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	_eye = _eye * _R;

	//_eye = rot.postMult(_eye);

	//std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	_eye = _eye * _translateM2;
    //std::cout << eye.x() << " " << eye.y() << " " << eye.z() << std::endl;
	
	
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	_center = _first_center * _translateM1;
	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	_center = _center * _R;

	//_center = rot.postMult(_center);

	//std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;
	_center = _center * _translateM2;
    //std::cout << center.x() << " " << center.y() << " " << center.z() << std::endl;

	//_up = rot.postMult(_up);
	//_up = _up * _translateM1;

	_up = _first_up * _R ;

	//_up = rot.postMult(_up);

	//_up = _up * _translateM2;

	_trballman->setHomePosition(_eye, _center, _up);
}

void WorkingModel::printLocationModel(){
	osg::Vec3d modelPos = _model->getPosition();

	cout << "x: " << modelPos.x() << " y: " << modelPos.y() << " z: " << modelPos.z() << endl;
}

void WorkingModel::printLocationCamera(){
	osg::Vec3d eye, center, up;

	_trballman->getHomePosition(eye, center, up);

	cout << "------------------------------------------------------------------------" << endl;
	cout << "eye x: " << eye.x() << " eye y: " << eye.y() << " eye z: " << eye.z() << endl;
	cout << "center x: " << center.x() << " center y: " << center.y() << " center z: " << center.z() << endl;
	cout << "up x: " << up.x() << " up y: " << up.y() << " up z: " << up.z() << endl;
	cout << "------------------------------------------------------------------------" << endl;
}