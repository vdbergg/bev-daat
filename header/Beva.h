//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "Trie.h"
#include "ActiveNode.h"
#include "utils.h"
#include "C.h"

#define CHAR_SIZE 128

class Beva {
public:
    Trie* trie;
    unsigned editVectorSize;
    Experiment *experiment;
    int editDistanceThreshold;

    int bitmapSize;
    unsigned bitmapZero;

    Beva(Trie*, Experiment*, int);
    ~Beva();
    
    void process(char, int, vector<ActiveNode>& oldActiveNodes, vector<ActiveNode>& currentActiveNodes,
            unsigned (&bitmaps)[CHAR_SIZE]);
    void findActiveNodes(unsigned, ActiveNode&, vector<ActiveNode>&, unsigned (&bitmaps)[CHAR_SIZE]);
    
    inline unsigned buildBitmap(unsigned queryLength, unsigned lastPosition, char c, unsigned (&bitmaps)[CHAR_SIZE]) {
        int k = (int) queryLength - (int) lastPosition;
        return utils::leftShiftBitInDecimal(bitmaps[c], this->editDistanceThreshold - k, this->bitmapSize);
    }

    void updateBitmap(char, unsigned (&bitmaps)[CHAR_SIZE]);

    inline unsigned char* buildInitialEditVector() {
        unsigned char *vet = new unsigned char[this->editVectorSize];

        unsigned countNegative = this->editDistanceThreshold;
        unsigned countPositive = 1;

        for (int i = 0; i < this->editVectorSize; i++) {
            if (i < this->editDistanceThreshold) {
                vet[i] = countNegative;
                countNegative--;
            } else if (i == this->editDistanceThreshold) {
                vet[i] = 0;
            } else {
                vet[i] = countPositive;
                countPositive++;
            }
        }

        return vet;
    }

    inline unsigned char* buildEditVectorWithBitmap(unsigned bitmap, unsigned char* previousEditVector) {
        unsigned char *vet = new unsigned char[this->editVectorSize];

        for (int i = 0; i < this->editVectorSize; i++) {
            unsigned char item = utils::min(
                    previousEditVector[i] + (1 - utils::getKthBitFromDecimal(bitmap, this->editVectorSize - 1 - i)),
                    i + 1 >= this->editVectorSize ? C::MARKER : previousEditVector[i + 1] + 1,
                    i - 1 < 0 ? C::MARKER : vet[i - 1] + 1
            );
            vet[i] = item;
        }

        return vet;
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
