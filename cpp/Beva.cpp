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
        vector<ActiveNode>& currentActiveNodes, unsigned *bitmaps) {
    this->updateBitmap(ch, bitmaps);

    if (prefixQueryLength == 1) {
        currentActiveNodes.resize(currentActiveNodes.size() + 1);
        currentActiveNodes[currentActiveNodes.size() - 1].buildInitialValue(this->trie->root,0,
                this->editDistanceThreshold, this->editVectorSize);
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

void Beva::updateBitmap(char ch, unsigned *bitmaps) { // query is equivalent to Q' with the last character c
    for (unsigned x = 0; x < CHAR_SIZE; x++) {
        bitmaps[x] = utils::leftShiftBitInDecimal(bitmaps[x], 1, this->bitmapSize);
    }
  
    bitmaps[ch] = bitmaps[ch] | 1;
}

void Beva::findActiveNodes(unsigned queryLength, ActiveNode &oldActiveNode,
			   vector<ActiveNode> &activeNodes, unsigned *bitmaps) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChilds = child + this->trie->getNode(oldActiveNode.node).numChildren;

    unsigned tempSize = oldActiveNode.level + 1;
    for (; child < endChilds; child++) {
        ActiveNode newActiveNode;
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        unsigned bitmap = this->buildBitmap(queryLength, tempSize, this->trie->getNode(child).getValue(), bitmaps);
        this->buildEditVectorWithBitmap(bitmap, oldActiveNode.editVector, newActiveNode.editVector);

        if (this->editVectorIsFinal(newActiveNode.editVector)) continue;

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        newActiveNode.node = child;
        newActiveNode.level = tempSize;
        if (this->getEditDistance((int) queryLength - (int) tempSize, newActiveNode.editVector) <= this->editDistanceThreshold) {
            activeNodes.emplace_back(newActiveNode);
        } else {
            this->findActiveNodes(queryLength, newActiveNode, activeNodes, bitmaps);
        }
    }
}
