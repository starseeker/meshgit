#ifndef BSONIO_H
#define BSONIO_H

#include "json.h"

jsonValue loadBin(const string& filename);
void saveBin(const string& filename, const jsonValue& json);


#endif
