//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "Trie.h"
#include "ActiveNode.h"
#include "utils.h"

#define CHAR_SIZE 128

class Beva {
public:
    Trie* trie;
    EditVector* initialEditVector;
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
    
    inline EditVector* getNewEditVector(unsigned queryLength, EditVector* editVector, unsigned lastPosition, char c,
                                   unsigned (&bitmaps)[CHAR_SIZE]) {
        unsigned bitmap = this->buildBitmap(queryLength, lastPosition, c, bitmaps);

        EditVector* newEditVector = new EditVector(this->editDistanceThreshold);
        newEditVector->buildEditVectorWithBitmap(bitmap, editVector);
        return newEditVector;
    }

    void updateBitmap(char, unsigned (&bitmaps)[CHAR_SIZE]);
};


#endif //BEVA_BEVA_H
