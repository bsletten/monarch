/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_data_mpeg_Crc16_H
#define monarch_data_mpeg_Crc16_H

#include "monarch/util/Crc16.h"
#include "monarch/data/mpeg/AudioFrameHeader.h"

namespace monarch
{
namespace data
{
namespace mpeg
{

/**
 * An AudioCrc16 is used to *correctly* calculate the CRC-16 for MPEG Audio.
 *
 * The polynomial key used is 0x8005. The starting CRC value is 0xffff.
 *
 * @author Dave Longley
 */
class AudioCrc16 : monarch::util::Crc16
{
public:
   /**
    * Creates a new AudioCrc16.
    */
   AudioCrc16();

   /**
    * Destructs this AudioCrc16.
    */
   virtual ~AudioCrc16();

   /**
    * Resets the CRC value to 0xffff.
    */
   virtual void reset();

   /**
    * Gets the number of audio data bytes required to determine the CRC-16.
    *
    * The CRC-16 for an MPEG audio frame is calculated by using the last 2
    * bytes of the frames header along with a number of bits from the audio
    * data that follows the checksum. The checksum itself must not be included
    * in the calculation.
    *
    * The number of bits used from the audio data is determined by the
    * layer type.
    *
    * For Layer I:
    *
    * The number of bits used is two times (because stereo is 2 channels) the
    * number of stereo subbands plus the number of mono subbands -- all times 4.
    *
    * For Layer II:
    *
    * Unimplemented.
    *
    * For Layer III:
    *
    * The bits used are the side information. This is 32 bytes for
    * MPEG 1/Stereo, 17 bytes for MPEG 1/Mono, 17 bytes for MPEG 2/2.5/Stereo,
    * and 9 bytes for MPEG 2/2.5/Mono.
    *
    * @param header the AudioFrameHeader header to use.
    *
    * @return the number of audio data bytes required to determine the CRC-16.
    */
   virtual int getAudioDataAmount(AudioFrameHeader* header);

   /**
    * Calculates the CRC-16 for this frame given the passed audio data bytes.
    * The appropriate number of audio data bytes can be determined by calling
    * getAudioDataAmount(header).
    *
    * The given AudioFrameHeader data will be used in the CRC calculation.
    *
    * @param header the AudioFrameHeader to use.
    * @param audioData the audio data.
    *
    * @return the calculated CRC.
    */
   virtual int calculateCrc(
      AudioFrameHeader* header, const char* audioData);
};

} // end namespace mpeg
} // end namespace data
} // end namespace monarch
#endif
