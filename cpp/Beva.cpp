//
// Created by berg on 14/02/19.
//

#include "../header/Beva.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, Experiment* experiment, int editDistanceThreshold, long long *preCalculatedExponentiation) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (1 << ((2 * this->editDistanceThreshold) + 1)) - 1; // 2^(2tau + 1) - 1
    this->editVectorSize = (2 * this->editDistanceThreshold) + 1;
    this->trie = trie;
    this->bitmapZero = 0;
    this->experiment = experiment;
    this->preCalculatedExponentiation = preCalculatedExponentiation;
}

Beva::~Beva() {
}

void Beva::processNoErrors(char ch,
                           int prefixQueryLength,
                           vector<ActiveNode>& oldActiveNodes,
                           vector<ActiveNode>& currentActiveNodes) {
    if (prefixQueryLength == 1) {
        oldActiveNodes.resize(currentActiveNodes.size() + 1);
        oldActiveNodes[oldActiveNodes.size() - 1].buildInitialValue(this->trie->root, 0, this->editDistanceThreshold, this->editVectorSize);
    }

    for (ActiveNode oldActiveNode : oldActiveNodes) { // This for loop always will have one or zero item
        unsigned child = this->trie->getNode(oldActiveNode.node).children;
        unsigned endChildren = child + this->trie->getNode(oldActiveNode.node).numChildren;
        unsigned tempSize = oldActiveNode.level + 1;

        for (; child < endChildren; child++) {
            if (ch == this->trie->getNode(child).getValue()) {
                #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
                this->experiment->incrementNumberOfActiveNodes(queryLength);
                #endif
                currentActiveNodes.resize(currentActiveNodes.size() + 1);
                currentActiveNodes[currentActiveNodes.size() - 1].buildInitialValue(child, tempSize,
                                                                                    this->editDistanceThreshold, this->editVectorSize);
                break;
            }
        }
    }
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

void Beva::processWithPruningV2(char ch,
                                int prefixQueryLength,
                                vector<ActiveNode>& oldActiveNodes,
                                vector<ActiveNode>& currentActiveNodes,
                                unsigned (&bitmaps)[CHAR_SIZE],
                                TopKHeap& heap) {
    this->updateBitmap(ch, bitmaps);

    if (prefixQueryLength == 1) {
        double childScore = utils::dynamicScore(this->trie->getNode(this->trie->root).getMaxStaticScore(),
                                                this->preCalculatedExponentiation[this->editDistanceThreshold]);
        unsigned recordIdFromActiveNodeScore = this->trie->getNode(this->trie->root).getRecordIdFromMaxScore();

        if (heap.isFull() && (childScore < heap.topMaxScore() || heap.contains(recordIdFromActiveNodeScore))) return;

        currentActiveNodes.resize(currentActiveNodes.size() + 1);
        currentActiveNodes[currentActiveNodes.size() - 1].buildInitialValue(this->trie->root,0,
                                                                            this->editDistanceThreshold, this->editVectorSize);
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(query.length());
        #endif
    } else if (prefixQueryLength > this->editDistanceThreshold) {
        for (ActiveNode oldActiveNode : oldActiveNodes) {
            this->findActiveNodesWithPruningV2(prefixQueryLength,
                                               oldActiveNode,
                                               currentActiveNodes,
                                               bitmaps,
                                               heap);
        }
    } else {
        swap(currentActiveNodes, oldActiveNodes);
//        for (ActiveNode& activeNode : currentActiveNodes) { // This code is necessary when the edit distance were obtained in fetching phase
//            activeNode.editDistance++;
//        }
    }
}

void Beva::findActiveNodesWithPruningV2(unsigned queryLength,
                                        ActiveNode &oldActiveNode,
                                        vector<ActiveNode> &activeNodes,
                                        unsigned (&bitmaps)[CHAR_SIZE],
                                        TopKHeap& topKHeap) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChildren = child + this->trie->getNode(oldActiveNode.node).numChildren;
    unsigned tempSize = oldActiveNode.level + 1;

    double nodeScore = utils::dynamicScore(this->trie->getNode(oldActiveNode.node).getMaxStaticScore(),
                                           this->preCalculatedExponentiation[this->editDistanceThreshold]);
    unsigned recordIdFromNodeScore = this->trie->getNode(oldActiveNode.node).getRecordIdFromMaxScore();

    if (topKHeap.isFull() && (nodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromNodeScore))) {
        return;
    }

    for (; child < endChildren; child++) {
        ActiveNode newActiveNode;
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        unsigned bitmap = this->buildBitmap(queryLength, tempSize, this->trie->getNode(child).getValue(), bitmaps);
        this->buildEditVectorWithBitmap(bitmap, oldActiveNode.editVector, newActiveNode.editVector);

        if (this->editVectorIsFinal(newActiveNode.editVector)) {
            continue;
        }

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        newActiveNode.node = child;
        newActiveNode.level = tempSize;
        newActiveNode.editDistance = this->getEditDistance((int) queryLength - (int) tempSize, newActiveNode.editVector);

        if (newActiveNode.editDistance <= this->editDistanceThreshold) {
            activeNodes.emplace_back(newActiveNode);
        } else {
            this->findActiveNodesWithPruningV2(queryLength, newActiveNode, activeNodes, bitmaps, topKHeap);
        }
    }
}
