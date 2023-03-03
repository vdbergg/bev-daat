//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


class ActiveNode {
public:
    unsigned char editVector[16];
    unsigned node;
    unsigned level;
    unsigned editDistance;

    inline void buildInitialValue(unsigned node, unsigned level, unsigned editDistanceThreshold, unsigned editVectorSize) {
        this->node = node;
        this->level = level;

        unsigned countNegative = editDistanceThreshold;
        unsigned countPositive = 1;

        for (int i = 0; i < editVectorSize; i++) {
            if (i < editDistanceThreshold) {
                this->editVector[i] = countNegative;
                countNegative--;
            } else if (i == editDistanceThreshold) {
                this->editVector[i] = 0;
            } else {
                this->editVector[i] = countPositive;
                countPositive++;
            }
        }
    }

};

#endif //BEVA_ACTIVENODE_H
