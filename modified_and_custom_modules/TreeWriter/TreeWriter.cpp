/**
 * @file
 * @brief Implementation of module TreeWriter
 *
 * @copyright Copyright (c) 2017-2022 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "TreeWriter.h"
#include <vector>

using namespace corryvreckan;
using namespace std;

//TreeWriter::TreeWriter(Configuration& config, std::shared_ptr<Detector> detector)
//  : Module(config, detector), m_detector(detector) {

TreeWriter::TreeWriter(Configuration& config, std::vector<std::shared_ptr<Detector>> detectors)
  : Module(config, std::move(detectors)) {

    config_.setDefault<std::string>("file_name", "outputTuples.root");
    config_.setDefault<std::string>("tree_name", "tree");

    m_fileName = config_.get<std::string>("file_name");
    m_treeName = config_.get<std::string>("tree_name");
  }

void TreeWriter::initialize() {

  LOG(DEBUG) << "Initialised TreeWriter";

  // Create output file and directories
  auto path = createOutputFile(m_fileName, "root");
  m_outputFile = new TFile(path.c_str(), "RECREATE");
  LOG(DEBUG) << "Made and moved to output file: " << path;
  gDirectory->Delete("tree;*");
  m_outputTree = new TTree(m_treeName.c_str(), m_treeName.c_str());
  LOG(DEBUG) << "Created tree: " << m_treeName;

  eventID = 0;
	filledEvents = 0;
	emptyEvents = 0;

  // Create the output branches
  m_outputTree->Branch("eventID", &eventID, "eventID/I");

	// Branches for cluster data
	
	m_outputTree->Branch("cluster_XYcharge_gem1", &xyCharge_gem1);
	m_outputTree->Branch("cluster_XYcharge_gem2", &xyCharge_gem2);
	m_outputTree->Branch("cluster_XYcharge_gem3", &xyCharge_gem3);

	m_outputTree->Branch("clustPos_x_gem1", &clustPos_x_gem1);
	m_outputTree->Branch("clustPos_x_gem2", &clustPos_x_gem2);
	m_outputTree->Branch("clustPos_x_gem3", &clustPos_x_gem3);
	
	m_outputTree->Branch("clustPos_y_gem1", &clustPos_y_gem1);
	m_outputTree->Branch("clustPos_y_gem2", &clustPos_y_gem2);
	m_outputTree->Branch("clustPos_y_gem3", &clustPos_y_gem3);

	// Branches for track data
	m_outputTree->Branch("trackIntercept_x_gem1", &trackIntercept_x_gem1);
	m_outputTree->Branch("trackIntercept_x_gem2", &trackIntercept_x_gem2);
	m_outputTree->Branch("trackIntercept_x_gem3", &trackIntercept_x_gem3);

	m_outputTree->Branch("trackIntercept_y_gem1", &trackIntercept_y_gem1);
	m_outputTree->Branch("trackIntercept_y_gem2", &trackIntercept_y_gem2);
	m_outputTree->Branch("trackIntercept_y_gem3", &trackIntercept_y_gem3);

	m_outputTree->Branch("intercept_at_800mm", &intercept_at_800mm);

	m_outputTree->Branch("trackDirection", &trackDirection);
}

StatusCode TreeWriter::run(const std::shared_ptr<Clipboard>& clipboard) {

  // Clear data vectors before storing the cluster information for this event



	// Getting tracks from the clipboard
  auto tracks = clipboard->getData<Track>();

	if(tracks.empty()) {
		LOG(DEBUG) << "No clusters on the clipboard";

		
		// If no tracks, pushing back empty entries.
		

		xyCharge_gem1 = std::numeric_limits<double>::quiet_NaN();
		xyCharge_gem2 = std::numeric_limits<double>::quiet_NaN();
		xyCharge_gem3 = std::numeric_limits<double>::quiet_NaN();

		clustPos_x_gem1 = std::numeric_limits<double>::quiet_NaN();
		clustPos_x_gem2 = std::numeric_limits<double>::quiet_NaN();
		clustPos_x_gem3 = std::numeric_limits<double>::quiet_NaN();

		clustPos_y_gem1 = std::numeric_limits<double>::quiet_NaN();
		clustPos_y_gem2 = std::numeric_limits<double>::quiet_NaN();
		clustPos_y_gem3 = std::numeric_limits<double>::quiet_NaN();

		trackIntercept_x_gem1 = std::numeric_limits<double>::quiet_NaN();
		trackIntercept_x_gem2 = std::numeric_limits<double>::quiet_NaN();
		trackIntercept_x_gem3 = std::numeric_limits<double>::quiet_NaN();

		trackIntercept_y_gem1 = std::numeric_limits<double>::quiet_NaN();
		trackIntercept_y_gem2 = std::numeric_limits<double>::quiet_NaN();
		trackIntercept_y_gem3 = std::numeric_limits<double>::quiet_NaN();

		// Creating empty ROOT objects
		ROOT::Math::XYZPoint P;
		ROOT::Math::XYZVector V;

		intercept_at_800mm = P;
		trackDirection = V;

		m_outputTree->Fill();
		emptyEvents++;
		return StatusCode::NoData;
		}

  for(auto& detector : get_detectors()) {

		// Getting cluster data, stored with a detector name key
		auto clusters = clipboard->getData<Cluster>(detector->getName());

				for (auto& cluster : clusters){
					LOG(DEBUG) << "detector__ " << detector->getName() << " cluster: " << cluster->charge();
					LOG(DEBUG) << "cluster timestamp: " << cluster->timestamp();
					
					// add clust pos charge, when charge in cluster
				
				
				if(strcmp(detector->getName().c_str(), "GEMXY1")==0){
					clustPos_x_gem1 = cluster->local().x();
					clustPos_y_gem1 = cluster->local().y();
					xyCharge_gem1 = cluster->charge();
					eventID=int(cluster->timestamp());
				}
				if(strcmp(detector->getName().c_str(), "GEMXY2")==0){
					clustPos_x_gem2 = cluster->local().x();
					clustPos_y_gem2 = cluster->local().y();
					xyCharge_gem2 = cluster->charge();
					eventID=int(cluster->timestamp());
				}
				if(strcmp(detector->getName().c_str(), "GEMXY3")==0){
					clustPos_x_gem3 = cluster->local().x();
					clustPos_y_gem3 = cluster->local().y();
					xyCharge_gem3 = cluster->charge();
					eventID=int(cluster->timestamp());
				}

			}



			for(auto& track : tracks) {

			LOG(DEBUG) << "evtID: " << int(track->timestamp());
			eventID=int(track->timestamp());

			if(strcmp(detector->getName().c_str(), "dut")==0){
      }

      if(strcmp(detector->getName().c_str(), "GEMXY1")==0){
				auto tIntercept = detector->getIntercept(track.get());
				trackIntercept_x_gem1 = tIntercept.X();
				trackIntercept_y_gem1 = tIntercept.Y();

				// Track direction the same for all detectors, thus only
				// adding it to the tree once. Same applies to intercept 
				// at 800 mm
				trackDirection = track->getDirection(detector->getName().c_str());
				intercept_at_800mm = track->getIntercept(800);
      }

      if(strcmp(detector->getName().c_str(), "GEMXY2")==0){
				auto tIntercept = detector->getIntercept(track.get());
				trackIntercept_x_gem2 = tIntercept.X();
				trackIntercept_y_gem2 = tIntercept.Y();
      }

      if(strcmp(detector->getName().c_str(), "GEMXY3")==0){
				auto tIntercept = detector->getIntercept(track.get());
				trackIntercept_x_gem3 = tIntercept.X();
				trackIntercept_y_gem3 = tIntercept.Y();
      }
			
			
    }
  }

  filledEvents++;
  if(filledEvents % 100 == 0) {
    LOG(DEBUG) << "Events with single associated cluster: " << filledEvents;
  }

  // Fill the tree with the information for this event
  m_outputTree->Fill();

  // Return value telling analysis to keep running
  return StatusCode::Success;
}

void TreeWriter::finalize(const std::shared_ptr<ReadonlyClipboard>&) {

  LOG(DEBUG) << "Finalise";
  auto directory = m_outputFile->mkdir("Directory");

  directory->cd();
  LOG(STATUS) << filledEvents << " real + " << emptyEvents << " empty = " << filledEvents+emptyEvents << " tracks written to file " << m_fileName;

  // Writing out outputfile
  m_outputFile->Write();
  delete(m_outputFile);

}
