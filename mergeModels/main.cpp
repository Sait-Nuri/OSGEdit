#include <iostream>
#include <string>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>

osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);

int main(int argc, char* argv[])
{

	// Check command-line parameters
	if (argc < 4)
	{
	  std::cerr << "Usage: " << argv[0] << " <model files> ... <name of output file>\n";
	  exit(1);
	}

	osg::Group* root = NULL;
	osg::Node* modelNodes[argc-2];
	osgViewer::Viewer viewer;

	root = new osg::Group();

	// Read all models from files
	for (int i = 0; i < argc-2; ++i)
	{
		modelNodes[i] = osgDB::readNodeFile(argv[i+1]);
		
		if(modelNodes[i] == NULL){
			std::cerr << "Problem opening '" << argv[i+1] << "'\n";
      		exit(1);
		}else{
			root->addChild(modelNodes[i]);
		}
	}

	// Create new osg file of edited model
	if(osgDB::writeNodeFile( *root, ""+std::string( argv[argc-1]) ) ){
		std::cout << "Merging has been successfully done\n";	
	}else{
		std::cout << "Merging has been failed!\n";
	}
	
	return 0;
}
