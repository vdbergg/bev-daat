//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "Trie.h"
#include "ActiveNode.h"
#include "utils.h"
#include "C.h"
#include "TopKHeap.h"

#define CHAR_SIZE 128

class Beva {
public:
    Trie* trie;
    unsigned editVectorSize;
    Experiment *experiment;
    int editDistanceThreshold;

    int bitmapSize;
    unsigned bitmapZero;
    long long *preCalculatedExponentiation;

    Beva(Trie*, Experiment*, int, long long *preCalculatedExponentiation);
    ~Beva();

    void processNoErrors(char ch,
                         int prefixQueryLength,
                         vector<ActiveNode>& oldNoErrorActiveNodes,
                         vector<ActiveNode>& currentNoErrorActiveNodes);
    
    void process(char, int, vector<ActiveNode>& oldActiveNodes, vector<ActiveNode>& currentActiveNodes,
            unsigned *bitmaps);
    void findActiveNodes(unsigned queryLength, ActiveNode &oldActiveNode,
			 vector<ActiveNode> &activeNodes, unsigned *bitmaps);

    void processWithPruningV2(char ch,
                              int prefixQueryLength,
                              vector<ActiveNode>& oldActiveNodes,
                              vector<ActiveNode>& currentActiveNodes,
                              unsigned (&bitmaps)[CHAR_SIZE],
                              TopKHeap& topKHeap);

    void findActiveNodesWithPruningV2(unsigned queryLength,
                                      ActiveNode &oldActiveNode,
                                      vector<ActiveNode> &activeNodes,
                                      unsigned (&bitmaps)[CHAR_SIZE],
                                      TopKHeap& topKHeap);
      
    inline unsigned buildBitmap(unsigned queryLength, unsigned lastPosition, char c, unsigned *bitmaps) {
        int k = (int) queryLength - (int) lastPosition;
        return utils::leftShiftBitInDecimal(bitmaps[c], this->editDistanceThreshold - k, this->bitmapSize);
    }

    void updateBitmap(char, unsigned *bitmaps);

    inline void buildEditVectorWithBitmap(unsigned bitmap, unsigned char* previousEditVector, unsigned char *editVector) {
        unsigned limit = this->editVectorSize - 1;
        int i;

        editVector[0] = utils::min(
                previousEditVector[0] + (1 - utils::getKthBitFromDecimal(bitmap, this->editVectorSize - 1)),
                previousEditVector[1] + 1
                );
        for (i = 1; i < limit; i++) {
            editVector[i] = utils::min(
                    previousEditVector[i] + (1 - utils::getKthBitFromDecimal(bitmap, this->editVectorSize - 1 - i)),
                    previousEditVector[i + 1] + 1,
                    editVector[i - 1] + 1
            );
        }
        editVector[i] = utils::min(
                previousEditVector[i] + (1 - utils::getKthBitFromDecimal(bitmap, this->editVectorSize - 1 - i)),
                editVector[i - 1] + 1
                );
    }

    inline bool editVectorIsFinal(const unsigned char* editVector) {
        for (int i = 0; i < this->editVectorSize; i++) {
            if (editVector[i] <= this->editDistanceThreshold) return false;
        }
        return true;
    }

    inline unsigned getEditDistance(int k, unsigned char* editVector) {
        int result = (this->editDistanceThreshold + k);
        if (result < this->editVectorSize) return editVector[result];
        return C::MARKER;
    }
};


#endif //BEVA_BEVA_H
