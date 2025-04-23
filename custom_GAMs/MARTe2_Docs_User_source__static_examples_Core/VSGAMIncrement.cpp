/**
 * @file VSGAMIncrement.cpp
 * @brief Source file for class VSGAMIncrement
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
 * the class VSGAMIncrement (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "VSGAMIncrement.h"
#include <cstdio>
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe2Tutorial {
VSGAMIncrement::VSGAMIncrement() {
    counter = 0u;
    inputCounter = NULL_PTR(MARTe::uint8 *);
    oracleDecision = NULL_PTR(MARTe::uint8 *);
    marteStrategy = NULL_PTR(MARTe::uint8 *);
    outputCounter = NULL_PTR(MARTe::uint8 *);
}

VSGAMIncrement::~VSGAMIncrement() {

}

bool VSGAMIncrement::Initialise(MARTe::StructuredDataI & data) {
    using namespace MARTe;
    bool ok = GAM::Initialise(data);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Could not Initialise the GAM");
    } 
    return ok;
}

bool VSGAMIncrement::Setup() {
    using namespace MARTe;
    uint32 numberOfInputSignals = GetNumberOfInputSignals();
    uint32 numberOfOutputSignals = GetNumberOfOutputSignals();
    bool ok = true;
    if(ok){
        oracleDecision = reinterpret_cast<uint8 *>(GetInputSignalMemory(0u));

        inputCounter = reinterpret_cast<uint8 *>(GetOutputSignalMemory(0u));
        marteStrategy = reinterpret_cast<uint8 *>(GetOutputSignalMemory(1u));
        outputCounter = reinterpret_cast<uint8 *>(GetOutputSignalMemory(2u));
    } 
    return ok;
}

bool VSGAMIncrement::Execute() {
    
    counter++;
    
    //*outputCounter = 0; //is written by the FPGA after inputer counter is incremented
    *inputCounter = counter;
    
    // Single swap
    if (*oracleDecision == 1) {
        *marteStrategy = 1;
    } 
    else if (*oracleDecision == 0) {
        *marteStrategy = 0;
    } 
    return true;
}

CLASS_REGISTER(VSGAMIncrement, "")
}
