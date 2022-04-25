/*
    This file is part of Leela Chess Zero.
    Copyright (C) 2021 The LCZero Authors

    Leela Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leela Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela Chess.  If not, see <http://www.gnu.org/licenses/>.

    Additional permission under GNU GPL version 3 section 7

    If you modify this Program, or any covered work, by linking or
    combining it with NVIDIA Corporation's libraries from the NVIDIA CUDA
    Toolkit and the NVIDIA CUDA Deep Neural Network library (or a
    modified version of those libraries), containing parts covered by the
    terms of the respective license agreement, the licensors of this
    Program grant you additional permission to convey the resulting work.
 */

#import "MetalNetworkBuilder.h"
#import "NetworkGraph.h"

namespace lczero {
namespace metal_backend {

MetalNetworkBuilder::MetalNetworkBuilder(void) : self(NULL) {}

MetalNetworkBuilder::~MetalNetworkBuilder(void)
{
    [(id)self dealloc];
}

//void MetalNetworkBuilder::init(void* weights, void* options)
std::string MetalNetworkBuilder::init(int sub_batch_size, int gpu_id)
{
    // All metal devices.
    NSArray<id<MTLDevice>> * devices = MTLCopyAllDevices();
    
    if ([devices count] <= gpu_id) {
        // No GPU device matching ID.
        [NSException raise:@"Could not find device" format:@"Could not find a GPU or CPU compute device with specified id"];
        return "";
    }
    
    // Initialize the metal MPS Graph executor with the selected device.
    self = [[Lc0NetworkGraph alloc] initWithDevice:devices[gpu_id]
                                   batchesPerSplit:sub_batch_size];
    
    return std::string([devices[gpu_id].name UTF8String]);
}

void* MetalNetworkBuilder::getInputPlaceholder(int width, int height, int channels, std::string label) {
    return [(id)self inputPlaceholderWithInputChannels:channels
                                                height:height
                                                 width:width
                                                 label:[NSString stringWithUTF8String:label.c_str()]];
}

void* MetalNetworkBuilder::makeConvolutionBlock(void * previousLayer, int inputSize, int channelSize, int kernelSize,
                                                float * weights, float * biases, bool withRelu, std::string label) {
    return [(id)self addConvolutionBlockWithParent:(MPSGraphTensor *)previousLayer
                                     inputChannels:inputSize
                                    outputChannels:channelSize
                                        kernelSize:kernelSize
                                           weights:weights
                                            biases:biases
                                           hasRelu:(BOOL)withRelu
                                             label:[NSString stringWithUTF8String:label.c_str()]];
}

void* MetalNetworkBuilder::makeResidualBlock(void * previousLayer, int inputSize, int channelSize, int kernelSize,
                                             float * weights1, float * biases1, float * weights2, float * biases2,
                                             bool withRelu, std::string label, bool withSe, int seFcOutputs,
                                             float * seWeights1, float * seBiases1, float * seWeights2, float * seBiases2) {

    return [(id)self addResidualBlockWithParent:(MPSGraphTensor *)previousLayer
                                  inputChannels:inputSize
                                 outputChannels:channelSize
                                     kernelSize:kernelSize
                                       weights1:weights1
                                        biases1:biases1
                                       weights2:weights2
                                        biases2:biases2
                                          label:[NSString stringWithUTF8String:label.c_str()]
                                          hasSe:withSe ? YES : NO
                                     seWeights1:seWeights1
                                      seBiases1:seBiases1
                                     seWeights2:seWeights2
                                      seBiases2:seBiases2
                                    seFcOutputs:seFcOutputs];
}

void* MetalNetworkBuilder::makeFullyConnectedLayer(void * previousLayer, int inputSize, int outputSize,
                                                   float * weights, float * biases,
                                                   std::string activation, std::string label) {

    return [(id)self addFullyConnectedLayerWithParent:(MPSGraphTensor *)previousLayer
                                        inputChannels:inputSize
                                       outputChannels:outputSize
                                              weights:weights
                                               biases:biases
                                           activation:[NSString stringWithUTF8String:activation.c_str()]
                                                label:[NSString stringWithUTF8String:label.c_str()]];
}

void* MetalNetworkBuilder::makePolicyMapLayer(void * previousLayer, const short * policyMap, int size, std::string label) {
    return [(id)self addPolicyMapLayerWithParent:(MPSGraphTensor *)previousLayer
                                       policyMap:(short *)policyMap
                                 policyMapLength:size
                                           label:[NSString stringWithUTF8String:label.c_str()]];
}

void* MetalNetworkBuilder::setSelectedOutputs(std::vector<void *> * outputs) {
    NSArray<MPSGraphTensor *> * resultTensors = @[];

    for (const auto& output : *outputs) {
        resultTensors = [resultTensors arrayByAddingObject:(MPSGraphTensor *)output];
    }
    [(id)self setResultTensors:resultTensors];

    return (void*) self;
}

void MetalNetworkBuilder::forwardEval(float * inputs, int batchSize, int inputChannels)
{
    NSArray<MPSGraphTensor *> * resultTensors = [(id)self runInferenceWithBatchSize:batchSize
                                                                             inputs:inputs
                                                                      inputChannels:inputChannels];
}


void MetalNetworkBuilder::copyResults(int batchSize, std::vector<float *> output_mems)
{
    [(id)self copyResultsWithBatchSize:batchSize
                         outputBuffers:&output_mems[0]];
}

void MetalNetworkBuilder::saveVariables(std::vector<std::string> names)
{
    for (const std::string name : names) {
        [(id)self addVariable:[NSString stringWithUTF8String:name.c_str()]];
    }
}

void MetalNetworkBuilder::dumpVariables(std::vector<std::string> names, int batches)
{
    for (const std::string name : names) {
        [(id)self dumpVariable:[NSString stringWithUTF8String:name.c_str()] batches:batches];
    }
}

}  // namespace metal_backend
}  // namespace lczero
