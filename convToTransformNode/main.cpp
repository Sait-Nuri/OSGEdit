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
	if (argc != 3)
	{
	  std::cerr << "Usage: " << argv[0] << " <model file> <name of output file>\n";
	  exit(1);
	}

	osg::Group* root = NULL;
	osg::PositionAttitudeTransform* modelXform;
	osg::Node* modelNode;
	osgViewer::Viewer viewer;
	osg::Group* modelGroupNode;

	root = new osg::Group();
	modelXform = new osg::PositionAttitudeTransform();

	modelNode = osgDB::readNodeFile(argv[1]);
	if(modelNode == NULL){
		std::cerr << "Problem opening '" << argv[1] << "'\n";
      	exit(1);
	}


	//convert the node into a const Group pointer if Node is a Group, otherwise return 0. 
	if( (modelGroupNode = modelNode->asGroup()) != 0){
		int childNum = modelGroupNode->getNumChildren();

		//If the model node has one child, we will link it to a PositionAttitudeTransform
		if(childNum == 1){
			osg::Node* child = modelGroupNode->getChild(0); //Get the first child of the model

			osg::PositionAttitudeTransform* childPOTNode = dynamic_cast<osg::PositionAttitudeTransform*>(child);
			
			if( childPOTNode != NULL ){
				std::cout << "Already have PositionAttitudeTransform node\n";
				return 0;
			}else{
				modelXform->addChild(childPOTNode);
				root->addChild(modelXform);
				root->setName(modelNode->getName()); //Assign the original name of model to new root node	
			}
			
		}//If the model node has 
		else if(childNum > 1){ 
			osg::Group* modelBaseGroup = new osg::Group();

			//Travers all childs and add them into the base group
			for(int i = 0; i < childNum; i++){
				osg::Node* currentChildNode = modelGroupNode->getChild(i);
				modelBaseGroup->addChild(currentChildNode);
			}

			modelXform->addChild(modelBaseGroup);
			root->addChild(modelXform);
			root->setName(modelGroupNode->getName());
		}
	}

	// Create new osg file of edited model
	if(osgDB::writeNodeFile( *root, ""+std::string(argv[2]) ) ){
		std::cout << "Replacement has been successfully done\n";	
	}else{
		std::cout << "Replacement has been failed!\n";
	}
	
	return 0;
}

osg::Node* findNamedNode(const std::string& searchName, 
                                          osg::Node* currNode)
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