#ifndef CRC16_H
#define CRC16_H 1


#include <iostream>
#include <vector>

namespace balloon {

uint16_t calcCRC16(const std::vector<uint8_t>& pbuffer);

} /* namespace balloon */

#endif /* CRC16_H */
