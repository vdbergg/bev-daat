//
// Created by berg on 14/02/19.
//

#include "../header/Beva.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, Experiment* experiment, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (1 << ((2 * this->editDistanceThreshold) + 1)) - 1; // 2^(2tau + 1) - 1
    this->editVectorSize = (2 * this->editDistanceThreshold) + 1;
    this->trie = trie;
    this->bitmapZero = 0;
    this->experiment = experiment;
}

Beva::~Beva() {
}

void Beva::process(char ch, int prefixQueryLength, vector<ActiveNode>& oldActiveNodes,
        vector<ActiveNode>& currentActiveNodes, unsigned (&bitmaps)[CHAR_SIZE]) {
    this->updateBitmap(ch, bitmaps);

    if (prefixQueryLength == 1) {
        currentActiveNodes.emplace_back(this->trie->root, this->buildInitialEditVector(), 0);
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(query.length());
        #endif
    } else if (prefixQueryLength > this->editDistanceThreshold) {
        for (ActiveNode oldActiveNode : oldActiveNodes) {
            this->findActiveNodes(prefixQueryLength, oldActiveNode, currentActiveNodes, bitmaps);
        }
    } else {
        swap(currentActiveNodes, oldActiveNodes);
    }
}

void Beva::updateBitmap(char ch, unsigned (&bitmaps)[CHAR_SIZE]) { // query is equivalent to Q' with the last character c
    for (auto &bitmap : bitmaps) {
        bitmap = utils::leftShiftBitInDecimal(bitmap, 1, this->bitmapSize);
    }
  
    bitmaps[ch] = bitmaps[ch] | 1;
}

void Beva::findActiveNodes(unsigned queryLength, ActiveNode &oldActiveNode,
        vector<ActiveNode> &activeNodes, unsigned (&bitmaps)[CHAR_SIZE]) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChilds = child + this->trie->getNode(oldActiveNode.node).numChildren;

    unsigned tempSize = oldActiveNode.level + 1;
    for (; child < endChilds; child++) {
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        unsigned bitmap = this->buildBitmap(queryLength, tempSize, this->trie->getNode(child).getValue(), bitmaps);
        unsigned char * newEditVector = this->buildEditVectorWithBitmap(bitmap, oldActiveNode.editVector);

        if (this->editVectorIsFinal(newEditVector)) continue;

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        if (this->getEditDistance((int) queryLength - (int) tempSize, newEditVector) <= this->editDistanceThreshold) {
            activeNodes.emplace_back(child, newEditVector, tempSize);
        } else {
            ActiveNode tmp(child, newEditVector, tempSize);
            this->findActiveNodes(queryLength, tmp, activeNodes, bitmaps);
        }
    }
}
