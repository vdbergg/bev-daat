//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


#include "EditVector.h"

class ActiveNode {
public:
    EditVector* editVector;
    unsigned node;
    unsigned level;

    ActiveNode(unsigned, EditVector*, unsigned);
};

#endif //BEVA_ACTIVENODE_H
