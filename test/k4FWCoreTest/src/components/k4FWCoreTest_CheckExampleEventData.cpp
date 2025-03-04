/*
 * Copyright (c) 2014-2023 Key4hep-Project.
 *
 * This file is part of Key4hep.
 * See https://key4hep.github.io/key4hep-doc/ for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "k4FWCoreTest_CheckExampleEventData.h"
#include <GaudiAlg/GaudiAlgorithm.h>

// datamodel
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/TrackerHitCollection.h"

DECLARE_COMPONENT(k4FWCoreTest_CheckExampleEventData)

k4FWCoreTest_CheckExampleEventData::k4FWCoreTest_CheckExampleEventData(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("mcparticles", m_mcParticleHandle, "Dummy Particle collection (output)");
  declareProperty("vectorfloat", m_vectorFloatHandle, "Dummy collection (output)");
}

StatusCode k4FWCoreTest_CheckExampleEventData::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode k4FWCoreTest_CheckExampleEventData::execute() {
  auto floatVector = m_vectorFloatHandle.get();
  if (floatVector->size() != 3 || (*floatVector)[2] != m_event) {
    fatal() << "Contents of vectorfloat collection is not as expected: size = " << floatVector->size()
            << " (expected 3), contents = " << *floatVector << " (expected [125., 25., " << m_event << "]) "
            << std::endl;
    return StatusCode::FAILURE;
  }

  auto particles = m_mcParticleHandle.get();
  auto particle  = (*particles)[0];
  if ((particle.getMomentum().x != m_magicNumberOffset + m_event + 5) ||
      (particle.getMass() != m_magicNumberOffset + m_event + 8)) {
    fatal() << "Contents of mcparticles collection is not as expected: momentum.x = " << particle.getMomentum().x
            << " (expected " << m_magicNumberOffset + m_event + 5 << "), mass = " << particle.getMass() << " (expected "
            << m_magicNumberOffset + m_event + 8 << ")" << std::endl;
    return StatusCode::FAILURE;
  }

  m_event++;
  return StatusCode::SUCCESS;
}

StatusCode k4FWCoreTest_CheckExampleEventData::finalize() { return GaudiAlgorithm::finalize(); }
