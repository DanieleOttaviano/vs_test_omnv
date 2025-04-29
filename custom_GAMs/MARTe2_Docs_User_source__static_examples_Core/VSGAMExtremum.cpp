/**
 * @file VSGAMExtremum.cpp
 * @brief Source file for class VSGAMExtremum
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
 * the class VSGAMExtremum (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "VSGAMExtremum.h"
#include <cmath>
#include <stdio.h>
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe2Tutorial {
VSGAMExtremum::VSGAMExtremum() {  
    // Input Signals
    pf_voltages = NULL_PTR(MARTe::float64 *);
    vs3 = NULL_PTR(MARTe::float64 *);
    pf_currents = NULL_PTR(MARTe::float64 *);
    ivs3 = NULL_PTR(MARTe::float64 *);
    ip = NULL_PTR(MARTe::float64 *);
    zc = NULL_PTR(MARTe::float64 *);

    // Output Signals
    vs3_ref = NULL_PTR(MARTe::float64 *);

    // Constants
    k = 0.0072;
    omega = 62.831853071795862;
    alpha = 50;
    // State
    x_hat = 0.0;

    // Utility variables
    t = 0.0;
}

VSGAMExtremum::~VSGAMExtremum() {

}

bool VSGAMExtremum::Initialise(MARTe::StructuredDataI & data) {
    using namespace MARTe;
    bool ok = GAM::Initialise(data);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Could not Initialise the GAM");
    }
    return ok;
}


bool VSGAMExtremum::Setup() {
    using namespace MARTe;
    uint32 numberOfInputSignals = GetNumberOfInputSignals();
    uint32 numberOfOutputSignals = GetNumberOfOutputSignals();
    bool ok = true;

    if (ok) {
        pf_voltages  = reinterpret_cast<float64 *>(GetInputSignalMemory(0u));
        vs3  = reinterpret_cast<float64 *>(GetInputSignalMemory(1u));
        pf_currents  = reinterpret_cast<float64 *>(GetInputSignalMemory(2u));
        ivs3  = reinterpret_cast<float64 *>(GetInputSignalMemory(3u));
        ip  = reinterpret_cast<float64 *>(GetInputSignalMemory(4u));
        zc  = reinterpret_cast<float64 *>(GetInputSignalMemory(5u));

        vs3_ref = reinterpret_cast<float64 *>(GetOutputSignalMemory(0u));
    }
    return ok;

}



bool VSGAMExtremum::Execute() {
    
    // Fill u with inputs
    for(int i = 0; i < 11; i++) {
        u[i] = pf_voltages[i];
        u[i+12] = pf_currents[i];
    }
    u[11] = *vs3;
    u[23] = *ivs3;
    u[24] = *ip;
    u[25] = *zc;
    
    x_hat = 0.0;
    // Calculate C * state (dot product)
    for (int i = 0; i < 25; ++i) {
        x_hat += C[i] * state[i];
    }

    // Add D * u (dot product)
    for (int i = 0; i < 26; ++i) {
        x_hat += D[i] * u[i];
    }
 
    //  Calculate state = A*state + B*u;
    for (int i = 0; i < 25; ++i) {
        temp_state[i] = 0.0;
    }
    // Update state: state = A * state + B * u
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            temp_state[i] += A[i][j] * state[j];
        }
    }
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 26; ++j) {
            temp_state[i] += B[i][j] * u[j];
        }
    }
    for (int i = 0; i < 25; ++i) {
        state[i] = temp_state[i];
    } 

    // Compute VS3_ES
    *vs3_ref = alpha * sqrt(omega) * cos(omega * t) - k * sqrt(omega) * sin(omega * t) * x_hat * x_hat;

    // Current Time
    t = t + 0.0002;

    return true;
}

CLASS_REGISTER(VSGAMExtremum, "")
}
