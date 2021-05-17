//
// Created by berg on 09/02/19.
//

#include "../header/ActiveNode.h"

ActiveNode::ActiveNode(unsigned node, unsigned char* editVector, unsigned level) {
    this->node = node;
    this->editVector = editVector;
    this->level = level;
}
