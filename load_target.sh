#!/bin/bash

rsync -avz ${MARTe2_DIR}/Build ${target}/MARTe2
rsync -avz ${MARTe2_Components_DIR}/Build ${target}/MARTe2-components
rsync -avz target/*  ${target}