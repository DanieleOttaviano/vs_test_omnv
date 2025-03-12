/**
 * @file VSGAMClassic.cpp
 * @brief Source file for class VSGAMClassic
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
 * the class VSGAMClassic (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "VSGAMClassic.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe2Tutorial {
VSGAMClassic::VSGAMClassic() {  
    // Input Signals
    ivs3 = NULL_PTR(MARTe::float64 *);
    zdot = NULL_PTR(MARTe::float64 *);
    // Output Signals
    vs3_ref = NULL_PTR(MARTe::float64 *);
/* 1 KhZ
    Af = 0.994017946161516;
    Bf = 8.0;
    Cf = -4.435845007350849;
    Df = -1067.796610169491;
*/
    Af = 0.99940018;
    Bf = 2.0;
    Cf = -1.78393;
    Df = -1051.78;
    x_old = 0.0;
    gain = -0.00001;
}

VSGAMClassic::~VSGAMClassic() {

}

bool VSGAMClassic::Initialise(MARTe::StructuredDataI & data) {
    using namespace MARTe;
    bool ok = GAM::Initialise(data);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Could not Initialise the GAM");
    }
    return ok;
}


bool VSGAMClassic::Setup() {
    using namespace MARTe;
    uint32 numberOfInputSignals = GetNumberOfInputSignals();
    uint32 numberOfOutputSignals = GetNumberOfOutputSignals();
    bool ok = true;

    if (ok) {
        ok = (numberOfInputSignals == 2u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError,
                         "The number of input signals shall be equal to 2. numberOfOutputSignals = %d",
                         numberOfOutputSignals);
        }
    }
    if (ok) {
        ok = (numberOfOutputSignals == 1u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError,
                         "The number of output signals shall be equal to 1. numberOfOutputSignals = %d",
                         numberOfOutputSignals);
        }
    }
    if (ok) {
        TypeDescriptor inputSignalType = GetSignalType(InputSignals, 0u); 
        TypeDescriptor input2SignalType = GetSignalType(InputSignals, 1u);
        TypeDescriptor outputSignalType = GetSignalType(OutputSignals, 0u);
        ok = (inputSignalType == outputSignalType);
        if (ok) {
            ok = (input2SignalType == outputSignalType);
        }
        if (ok) {
            ok = (outputSignalType == Float64Bit);
        }
        if (!ok) {
            const char8 * const inputSignalTypeStr = TypeDescriptor::GetTypeNameFromTypeDescriptor(inputSignalType);
            const char8 * const outputSignalTypeStr = TypeDescriptor::GetTypeNameFromTypeDescriptor(outputSignalType);
            REPORT_ERROR(ErrorManagement::ParametersError,
                         "The type of the input and output signal shall be float64. inputSignalType = %s outputSignalType = %s", inputSignalTypeStr,
                         outputSignalTypeStr);
        }
    }
    if (ok) {
        uint32 numberOfInputSamples = 0u;
        uint32 numberOfOutputSamples = 0u;
        ok = GetSignalNumberOfSamples(InputSignals, 0u, numberOfInputSamples);
        if (ok) {
            ok = GetSignalNumberOfSamples(OutputSignals, 0u, numberOfOutputSamples);
        }
        if (ok) {
            ok = (numberOfInputSamples == numberOfOutputSamples);
        }
        if (ok) {
            ok = (numberOfInputSamples == 1u);
        }
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError,
                         "The number of input and output signals samples shall be equal to 1. numberOfInputSamples = %d numberOfOutputSamples = %d",
                         numberOfInputSamples, numberOfOutputSamples);
        }
    }
    if (ok) {
        uint32 numberOfInputDimensions = 0u;
        uint32 numberOfInput2Dimensions = 0u;
        uint32 numberOfOutputDimensions = 0u;
        ok = GetSignalNumberOfDimensions(InputSignals, 0u, numberOfInputDimensions);
        if (ok) {
            ok = GetSignalNumberOfDimensions(InputSignals, 1u, numberOfInput2Dimensions);
        }
        if (ok) {
            ok = GetSignalNumberOfDimensions(OutputSignals, 0u, numberOfOutputDimensions);
        }
        if (ok) {
            ok = (numberOfInputDimensions == numberOfOutputDimensions);
        }
        if (ok) {
            ok = (numberOfInput2Dimensions == numberOfOutputDimensions);
        }
        if (ok) {
            ok = (numberOfOutputDimensions == 0u);
        }
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError,
                         "The number of input, input2 and output signals dimensions shall be equal to 0. numberOfInputDimensions = %d numberOfInput2Dimensions = %d numberOfOutputDimensions = %d",
                         numberOfInputDimensions, numberOfInput2Dimensions, numberOfOutputDimensions);
        }
    }
    if (ok) {
        uint32 numberOfInputElements = 0u;
        uint32 numberOfInput2Elements = 0u;
        uint32 numberOfOutputElements = 0u;
        ok = GetSignalNumberOfElements(InputSignals, 0u, numberOfInputElements);
        if (ok) {
            ok = GetSignalNumberOfElements(InputSignals, 1u, numberOfInput2Elements);
        }
        if (ok) {
            ok = GetSignalNumberOfElements(OutputSignals, 0u, numberOfOutputElements);
        }
        if (ok) {
            ok = (numberOfInputElements == numberOfOutputElements);
        }
        if (ok) {
            ok = (numberOfInput2Elements == numberOfOutputElements);
        }
        if (ok) {
            ok = (numberOfOutputElements == 1u);
        }
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError,
                         "The number of input and output signals elements shall be equal to 2 and 1 respectively. numberOfInputElements = %d numberOfOutputElements = %d",
                         numberOfInputElements, numberOfOutputElements);
        }
    }
    if (ok) {
        ivs3  = reinterpret_cast<float64 *>(GetInputSignalMemory(0u));
        zdot  = reinterpret_cast<float64 *>(GetInputSignalMemory(1u));

        vs3_ref = reinterpret_cast<float64 *>(GetOutputSignalMemory(0u));
    }
    return ok;

}



bool VSGAMClassic::Execute() {
    
    *vs3_ref = ( (Cf) * (x_old) ) + ( (Df) * ( (*ivs3)*(gain) + (*zdot) ) );
    x_old = ( (Af) * (x_old) ) + ( (Bf) * ( (*ivs3)*(gain) + (*zdot) ) );
    
    return true;
}

CLASS_REGISTER(VSGAMClassic, "")
}
