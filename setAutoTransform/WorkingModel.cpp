#include "WorkingModel.h"

WorkingModel::WorkingModel(osg::AutoTransform* _model){
	this->model = _model;
	this->currentRotation = model->getRotation();
}

void WorkingModel::setWorkRotation(osg::Quat& newRotation){
	this->currentRotation = newRotation;
}

osg::Quat& WorkingModel::getWorkRotation(){
	return this->currentRotation;
}

osg::AutoTransform* WorkingModel::getModel(){
	return this->model;
}