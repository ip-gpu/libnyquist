/*
Copyright (c) 2015, Dimitri Diakopoulos All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "WavEncoder.h"
#include <fstream>

using namespace nqr;

// Big-endian convert
inline void toBytes(int value, char * arr)
{
    arr[0] = (value) & 0xFF;
    arr[1] = (value >> 8) & 0xFF;
    arr[2] = (value >> 16) & 0xFF;
    arr[3] = (value >> 24) & 0xFF;
}

WavEncoder::WavEncoder()
{
    
}

WavEncoder::~WavEncoder()
{
    
}

//@todo check for max file length, sanity checks, etc.
void WavEncoder::WriteFile(const EncoderParams p, const std::vector<float> & data, const std::string & path)
{
    
    std::ofstream fout(path.c_str(), std::ios::out | std::ios::binary);
    
    if (!fout.is_open())
    {
        throw std::runtime_error("File cannot be opened");
    }
    
    char * chunkSizeBuff = new char[4];
    
    // Initial size
    toBytes(36, chunkSizeBuff);

    // RIFF File Header
    fout.write(GenerateChunkCodeChar('R', 'I', 'F', 'F'), 4);
    fout.write(chunkSizeBuff, 4);
    
    fout.write(GenerateChunkCodeChar('W', 'A', 'V', 'E'), 4);
    
    // Fmt Header
    auto header = MakeWaveHeader(p);
    fout.write(reinterpret_cast<char*>(&header), sizeof(WaveChunkHeader));
    
    // Data Header
    fout.write(GenerateChunkCodeChar('d', 'a', 't', 'a'), 4);
    
    // + data chunk size
    auto numSamplesBytes =  data.size() * sizeof(float);
    toBytes(numSamplesBytes, chunkSizeBuff);
    fout.write(chunkSizeBuff, 4);
    
    // Debugging -- assume IEEE_Float
    fout.write(reinterpret_cast<const char*>(data.data()), numSamplesBytes);
    
    // Find size
    long totalSize = fout.tellp();
    
    // Modify RIFF header
    fout.seekp(4);
    
    // Total size of the file, less 8 for the RIFF header
    toBytes(totalSize - 8 , chunkSizeBuff);
    
    fout.write(chunkSizeBuff, 4);

    delete[] chunkSizeBuff;
}