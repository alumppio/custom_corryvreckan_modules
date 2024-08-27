/**
 * @file
 * @brief Implementation of module ClusteringGeneric
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "ClusteringGeneric.h"
#include "objects/Pixel.hpp"

#include <TBranch.h>
#include <TKey.h>
#include <TObjArray.h>
#include <TProcessID.h>
#include <TTree.h>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

#include "objects/Object.hpp"
#include "objects/objects.h"

#include "objects/Event.hpp"
#include "objects/Event.cpp"

#include "tools/cuts.h"
#include "tools/kdtree.h"

using namespace corryvreckan;

ClusteringGeneric::ClusteringGeneric(Configuration& config, std::shared_ptr<Detector> detector)
  : Module(config, detector), m_detector(detector) {

    config_.setAlias("spatial_cut_abs", "spatial_cut", true);
    config_.setDefault<std::string>("file_input", "Ttree.root");
    fileInput = config_.get<std::string>("file_input");
    spatial_cut_ = corryvreckan::calculate_cut<XYVector>("spatial_cut", config_, m_detector);
		noise_cut = config_.get<double>("noise_cut");

  }

ClusteringGeneric::~ClusteringGeneric(){
}

void ClusteringGeneric::initialize() {

  // Cluster plots
  std::string title = m_detector->getName() + " Cluster Charge;cluster charge [e];events";
  clusterCharge = new TH1F("clusterCharge", title.c_str(), 5000, -0.5, 49999.5);


  title = m_detector->getName() + " Cluster size;Size;events";
  clusterSize_x = new TH1F("clusterSize_x", title.c_str(), 22, 0, 22);

  title = m_detector->getName() + " Cluster size;Size;events";
  clusterSize_y = new TH1F("clusterSize_y", title.c_str(), 22, 0, 22);

  title = m_detector->getName() + " Cluster Position (Global);x [mm];y [mm];events";
  clusterPositionGlobal = new TH2F("clusterPositionGlobal",
      title.c_str(),
      (5*(fabs(spatial_cut_.x())+fabs(spatial_cut_.y()))),
      spatial_cut_.x(),
      spatial_cut_.y(),
      (5*(fabs(spatial_cut_.x())+fabs(spatial_cut_.y()))),
      spatial_cut_.x(),
      spatial_cut_.y());

  title = m_detector->getName() + " Cluster Position (Local);x [mm];y [mm];events";
  clusterPositionLocal = new TH2F("clusterPositionLocal",
      title.c_str(),
      (5*(fabs(spatial_cut_.x())+fabs(spatial_cut_.y()))),
      spatial_cut_.x(),
      spatial_cut_.y(),
      (5*(fabs(spatial_cut_.x())+fabs(spatial_cut_.y()))),
      spatial_cut_.x(),
      spatial_cut_.y());

  title = m_detector->getName() + "Raw Cluster Position ;x [mm];y [mm];events";
	rawClusters = new TH2F("rawClusterPosition", title.c_str(), 1000, 100, 100, 1000, 100, 100);

  title = m_detector->getName() + " Cluster multiplicity;clusters;events";
  clusterMultiplicity = new TH1F("clusterMultiplicity", title.c_str(), 50, -0.5, 49.5);
	
	// Cluster charge histograms

	
	title = m_detector->getName() + " Cluster charge in X;Charge[e];events";
	ClusterXCharge = new TH1F("clusterChargeX", title.c_str(), 5000, -0.5, 49999.5);

	title = m_detector->getName() + " Cluster charge in X;Charge[e];events";
	ClusterYCharge = new TH1F("clusterChargeY", title.c_str(), 5000, -0.5, 49999.5);

	title = m_detector->getName() + " Cluster charge in X vs. Cluster charge in Y;Charge[e];events";
	ClusterXvsYCharge = new TH1F("clusterChargeXvsY", title.c_str(), 5000, -0.5, 49999.5);

	LOG(DEBUG) << "spatial_cut_x: " << fabs(spatial_cut_.x()); 
	LOG(DEBUG) << "spatial_cut_y: " << fabs(spatial_cut_.y()); 

  // Checking Geometric file inputs
  for(auto& detector : get_detectors()) {
    LOG(DEBUG) << "Initialise for detector " + detector->getName();
  }

  LOG(DEBUG) << "Detector Name: " << m_detector->getName();
  LOG(DEBUG) << "size X: " << m_detector->getSize().X();
  LOG(DEBUG) << "size Y: " << m_detector->getSize().Y();
  LOG(DEBUG) << "Pixel size X: " << m_detector->nPixels().X();
  LOG(DEBUG) << "Pixel size Y: " << m_detector->nPixels().Y();
  LOG(DEBUG) << "Pitch size X: " << m_detector->getPitch().X();
  LOG(DEBUG) << "Pitch size Y: " << m_detector->getPitch().Y();

  // Initialise member variables
  m_eventNumber = 0;
	number_of_misses=0;
	LOG(DEBUG) << "INPUT_FILE:: " << fileInput.c_str();
	
  TFile *myFile = TFile::Open(fileInput.c_str());
  reader = new TTreeReader("TCluster", myFile);

  evtID = new TTreeReaderArray<int>(*reader, "evtID");
  detID = new TTreeReaderArray<int>(*reader, "detID");
  planeID = new TTreeReaderArray<int>(*reader, "planeID");
  clustPos = new TTreeReaderArray<float>(*reader, "clustPos");
  clustADCs = new TTreeReaderArray<float>(*reader, "clustADCs");
	//clustSize = new TTreeReaderArray<int>(*reader, "clustSize");
}

StatusCode ClusteringGeneric::run(const std::shared_ptr<Clipboard>& clipboard) {

	// clear the temporary vectors for the next detector
	temp_cluster_Xpos_container.clear();
	temp_cluster_Ypos_container.clear();

	temp_cluster_Xcharge_container.clear();
	temp_cluster_Ycharge_container.clear();

  reader->SetLocalEntry(m_eventNumber);

  LOG(DEBUG) << "evt Corryvreckan___: " << m_eventNumber;

  // Make the cluster container and the maps for clustering
  ClusterVector deviceClusters;
  auto cluster = std::make_shared<Cluster>();

	// Get the detector nam. Probably DAQ/Config file related.e
	std::string detectorName = m_detector->getName();
	std::string detectorType = m_detector->getType();
	LOG(DEBUG) << "Detector name: " << detectorName << "  and type: " << detectorType;
	LOG(DEBUG) << "File Input: " << fileInput;
	
	// Excluding DUT detector, since no position
	int detectorID=-1;
	if(strcmp(detectorName.c_str(), "GEMXY1")==0) detectorID=0;
	if(strcmp(detectorName.c_str(), "GEMXY2")==0) detectorID=1;
	if(strcmp(detectorName.c_str(), "GEMXY3")==0) detectorID=2;
	if(strcmp(detectorName.c_str(), "dut")==0) detectorID=3;
	if(strcmp(detectorName.c_str(), "diamond")==0) detectorID=4;


	// Considering if one or more hit points (x,y) per telescope
  if(detID->GetSize()==6){
		number_of_clusters++;
		for (size_t i = 0; i < detID->GetSize(); i++) {

			if(detID->At(i)!=detectorID) continue;
			
			if(planeID->At(i)==0){
				if (clustADCs->At(i)>noise_cut){
					temp_cluster_Xpos_container.push_back(clustPos->At(i));
					temp_cluster_Xcharge_container.push_back(clustADCs->At(i));
				}
				
			}
			if(planeID->At(i)==1){
				if (clustADCs->At(i)>noise_cut){
					temp_cluster_Ypos_container.push_back(clustPos->At(i));
					temp_cluster_Ycharge_container.push_back(clustADCs->At(i));
					}
				}
			} // for (detID size)
	

		// Initialize clustering variables
		int cluster_pair_idx_y = 0;
		double x_y_ratio = 999;

		for (size_t j=0; j<temp_cluster_Xpos_container.size(); j++){	
				for (size_t k=0; k<temp_cluster_Ypos_container.size(); k++){
					
					LOG(DEBUG) << "POS ratio X" << j << "/Y" << k << " =" << temp_cluster_Xpos_container[j] << "/" << temp_cluster_Ypos_container[k] << 
					" = " << temp_cluster_Xpos_container[j]/temp_cluster_Ypos_container[k] << 
					",  ADC ratio X" << j << "/Y" << k << " =" << temp_cluster_Xcharge_container[j] << "/" << temp_cluster_Ycharge_container[k] << 
					" = " << temp_cluster_Xcharge_container[j]/temp_cluster_Ycharge_container[k];


					x = temp_cluster_Xpos_container[j], y = temp_cluster_Ypos_container[k]; 
					charge_x = temp_cluster_Xcharge_container[j], charge_y = temp_cluster_Ycharge_container[k]; 

					// Create object with local cluster position
					auto positionLocal = ROOT::Math::XYZPoint(x, y, 0);

					// Calculate6global cluster position
					auto positionGlobal = m_detector->localToGlobal(positionLocal);

					cluster->setTimestamp(evtID->At(0));
					cluster->setCharge(charge_x+charge_y);
					cluster->setError(m_detector->getSpatialResolution(x, y));
					cluster->setErrorMatrixGlobal(m_detector->getSpatialResolutionMatrixGlobal(x, y));
					cluster->setDetectorID(detectorName);
					cluster->setClusterCentre(positionGlobal);
					cluster->setClusterCentreLocal(positionLocal);
					deviceClusters.push_back(cluster);
					
					LOG(DEBUG) << "detID == " << detectorID << "; cluster[" << j << k << "]: (x,y) + charge: (" << cluster->global().x() << ", " << cluster->global().y() << ") + " << cluster->charge();
					
					} // for (size tempYpos)
				} // for (size tempXpos)
				
      } // if (detID size)>5	
			else { number_of_misses++;}
			


  clusterMultiplicity->Fill(static_cast<double>(deviceClusters.size()));

  // Increment event counter
  m_eventNumber++;

  // Saving data using corryvreckan format
  clipboard->putData(deviceClusters, m_detector->getName());

	LOG(DEBUG) << std::endl;

  // Return value telling analysis to keep running
  if(reader->GetEntries()==m_eventNumber) return StatusCode::EndRun;
  return StatusCode::Success;
}

void ClusteringGeneric::finalize(const std::shared_ptr<ReadonlyClipboard>&) {
  LOG(DEBUG) << "Analysed " << m_eventNumber << " events";
	LOG(DEBUG) << "number_of_misses: " << number_of_misses;
	LOG(DEBUG) << "number_of_clusters: " << number_of_clusters;
}
