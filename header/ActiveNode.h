//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


class ActiveNode {
public:
    unsigned char* editVector;
    unsigned node;
    unsigned level;

    ActiveNode(unsigned, unsigned char*, unsigned);
};

#endif //BEVA_ACTIVENODE_H
