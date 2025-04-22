/**
 * @file VSGAMSwap.cpp
 * @brief Source file for class VSGAMSwap
 * @date 06/04/2018
 * @author Andre Neto
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing, 
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class VSGAMSwap (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "VSGAMSwap.h"
#include <cstdio>
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe2Tutorial {
VSGAMSwap::VSGAMSwap() {
    inputCounter = NULL_PTR(MARTe::uint8 *);
    outputCounter = NULL_PTR(MARTe::uint8 *);
    marteStrategy = NULL_PTR(MARTe::uint8 *);
    vs3es_ref = NULL_PTR(MARTe::float64 *);
    vs3fpga_ref = NULL_PTR(MARTe::float64 *);
    vs3_ref = NULL_PTR(MARTe::float64 *);
}

VSGAMSwap::~VSGAMSwap() {

}

bool VSGAMSwap::Initialise(MARTe::StructuredDataI & data) {
    using namespace MARTe;
    bool ok = GAM::Initialise(data);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Could not Initialise the GAM");
    } 
    return ok;
}

bool VSGAMSwap::Setup() {
    using namespace MARTe;
    uint32 numberOfInputSignals = GetNumberOfInputSignals();
    uint32 numberOfOutputSignals = GetNumberOfOutputSignals();
    bool ok = true;
    if(ok){
        inputCounter = reinterpret_cast<uint8 *>(GetInputSignalMemory(0u));
        outputCounter = reinterpret_cast<uint8 *>(GetInputSignalMemory(1u));
        marteStrategy = reinterpret_cast<uint8 *>(GetInputSignalMemory(2u));
        vs3es_ref = reinterpret_cast<float64 *>(GetInputSignalMemory(3u));
        vs3fpga_ref = reinterpret_cast<float64 *>(GetInputSignalMemory(4u));

        vs3_ref = reinterpret_cast<float64 *>(GetOutputSignalMemory(0u));
    } 
    return ok;
}

bool VSGAMSwap::Execute() {
   
    if(*marteStrategy == 0x01) {
        while(*inputCounter != *outputCounter) {
            // Wait for the input and output counters to match
        }
        *vs3_ref = *vs3fpga_ref;
    } 
    else {
        *vs3_ref = *vs3es_ref;
    }

    return true;
}

CLASS_REGISTER(VSGAMSwap, "")
}
