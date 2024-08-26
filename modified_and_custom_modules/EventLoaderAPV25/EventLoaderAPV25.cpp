/**
 * @file
 * @brief Implementation of module EventLoaderAPV25
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "EventLoaderAPV25.h"
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

EventLoaderAPV25::EventLoaderAPV25(Configuration& config, std::shared_ptr<Detector> detector)
  : Module(config, detector), m_detector(detector) {

    m_inputFile = config_.get<std::string>("file_input");
	}

EventLoaderAPV25::~EventLoaderAPV25(){
}

void EventLoaderAPV25::initialize() {


	std::string title = m_detector->getName() + "cluster_max_adc_full_waveform;ADC_timebin; ADC_counts";
	maxHitWaveform = new TH1F("maxHitWaveform", title.c_str(), 20, 0, 20);

	data_file = TFile::Open(m_inputFile.c_str());
	if (!data_file || data_file->IsZombie()){
		LOG(DEBUG) << "Failed to open the data file: " << m_inputFile;
		throw ModuleError("Error in opening TFile");
	}

	data_tree = dynamic_cast<TTree*>(data_file->Get("THit"));
	if (!data_tree) {
			LOG(ERROR) << "Failed to retrieve TTree 'hits' from file";
			throw ModuleError("Failed to retrieve TTree 'cluster_detector'");
	}

	reader = new TTreeReader("THit", data_file);

  evtID = new TTreeReaderValue<int>(*reader, "evtID");
	nch = new TTreeReaderValue<int>(*reader, "nch");


	// Peak adc bin number == hitTimebin
	hitTimebin = new TTreeReaderArray<int>(*reader, "hitTimebin.hitTimeBin");
	
  planeID = new TTreeReaderArray<int>(*reader, "planeID");
	strip = new TTreeReaderArray<int>(*reader, "strip");	
	detID = new TTreeReaderArray<int>(*reader, "detID");


	adcs = new std::vector<TTreeReaderArray<int16_t>*> {
		new TTreeReaderArray<int16_t>(*reader, "adc0"),
		new TTreeReaderArray<int16_t>(*reader, "adc1"),
		new TTreeReaderArray<int16_t>(*reader, "adc2"),
		new TTreeReaderArray<int16_t>(*reader, "adc3"),
		new TTreeReaderArray<int16_t>(*reader, "adc4"),
		new TTreeReaderArray<int16_t>(*reader, "adc5"),
		new TTreeReaderArray<int16_t>(*reader, "adc6"),
		new TTreeReaderArray<int16_t>(*reader, "adc7"),
		new TTreeReaderArray<int16_t>(*reader, "adc8"),
		new TTreeReaderArray<int16_t>(*reader, "adc9"),
		new TTreeReaderArray<int16_t>(*reader, "adc10"),
		new TTreeReaderArray<int16_t>(*reader, "adc11"),
		new TTreeReaderArray<int16_t>(*reader, "adc12"),
		new TTreeReaderArray<int16_t>(*reader, "adc13"),
		new TTreeReaderArray<int16_t>(*reader, "adc14")
			};

/*	
		adc2 = new TTreeReaderArray<uint16_t>(*reader, "adc2");
		adc3 = new TTreeReaderArray<uint16_t>(*reader, "adc3");
		adc4 = new TTreeReaderArray<uint16_t>(*reader, "adc4");
		adc5 = new TTreeReaderArray<uint16_t>(*reader, "adc5");
		adc6 = new TTreeReaderArray<uint16_t>(*reader, "adc6");
		adc7 = new TTreeReaderArray<uint16_t>(*reader, "adc7");
		adc8 = new TTreeReaderArray<uint16_t>(*reader, "adc8");
		adc9 = new TTreeReaderArray<uint16_t>(*reader, "adc9");
		adc10 = new TTreeReaderArray<uint16_t>(*reader, "adc10");
		adc11 = new TTreeReaderArray<uint16_t>(*reader, "adc11");
		adc12 = new TTreeReaderArray<uint16_t>(*reader, "adc12");
		adc13 = new TTreeReaderArray<uint16_t>(*reader, "adc13");
		adc14 = new TTreeReaderArray<uint16_t>(*reader, "adc14");
		adc15 = new TTreeReaderArray<uint16_t>(*reader, "adc15");
		adc16 = new TTreeReaderArray<uint16_t>(*reader, "adc16");
		adc17 = new TTreeReaderArray<uint16_t>(*reader, "adc17");
		adc18 = new TTreeReaderArray<uint16_t>(*reader, "adc18");
		adc19 = new TTreeReaderArray<uint16_t>(*reader, "adc19");
		adc20 = new TTreeReaderArray<uint16_t>(*reader, "adc20");
		adc21 = new TTreeReaderArray<uint16_t>(*reader, "adc21");
		adc22 = new TTreeReaderArray<uint16_t>(*reader, "adc22");
		adc23 = new TTreeReaderArray<uint16_t>(*reader, "adc23");
		adc24 = new TTreeReaderArray<uint16_t>(*reader, "adc24");
		adc25 = new TTreeReaderArray<uint16_t>(*reader, "adc25");
		adc26 = new TTreeReaderArray<uint16_t>(*reader, "adc26");
		adc27 = new TTreeReaderArray<uint16_t>(*reader, "adc27");
		adc28 = new TTreeReaderArray<uint16_t>(*reader, "adc28");
		adc29 = new TTreeReaderArray<uint16_t>(*reader, "adc29");
*/
	/// Initialise member variables
  m_eventNumber = 0;
	number_of_entries = data_tree->GetEntries();
		
}

StatusCode EventLoaderAPV25::run(const std::shared_ptr<Clipboard>& clipboard) {

  reader->SetLocalEntry(m_eventNumber);

  LOG(DEBUG) << "evt Corryvreckan___: " << m_eventNumber;

  PixelVector pixel_container;

	// Configure detectors according to your detectors file!
	if(strcmp(m_detector->getName().c_str(), "GEMXY1")==0) detectorID = 0 ;
	if(strcmp(m_detector->getName().c_str(), "GEMXY2")==0) detectorID = 1 ;
	if(strcmp(m_detector->getName().c_str(), "GEMXY3")==0) detectorID = 2 ;


	// Hits_Plane_N < strip, peak_adc >
	Hits_Plane_X.clear();
	Hits_Plane_Y.clear();

	Clusters_Plane_X.clear();
	Clusters_Plane_Y.clear();

	for (size_t i=0; i<**nch; i++){
		if (detID->At(i) == detectorID){
			if (planeID->At(i) == 0){
				Hits_Plane_X.emplace_back(strip->At(i), adcs->at(static_cast<size_t>(hitTimebin->At(i) - 1))->At(i), i);
				LOG(DEBUG) << "X strip" << strip->At(i) << ",   peak adc = " << adcs->at(static_cast<size_t>(hitTimebin->At(i) - 1))->At(i);
			}
			else  { // Y plane
				Hits_Plane_Y.emplace_back(strip->At(i), adcs->at(static_cast<size_t>(hitTimebin->At(i) - 1))->At(i), i);
				LOG(DEBUG) << "Y strip" << strip->At(i) << ",   peak adc = " << adcs->at(static_cast<size_t>(hitTimebin->At(i) - 1))->At(i);
			}
		}
	} // for(nch) 

	std::sort(begin(Hits_Plane_X), end(Hits_Plane_X),
					[](const std::tuple<int16_t, int16_t, int8_t> &t1, const std::tuple<int16_t, int16_t, int8_t> &t2) {
						return std::get<0>(t1) < std::get<0>(t2) ||
									 (std::get<0>(t1) == std::get<0>(t2) &&
										std::get<1>(t1) > std::get<1>(t2));
					});
	
	std::sort(begin(Hits_Plane_Y), end(Hits_Plane_Y),
					[](const std::tuple<int16_t, int16_t, int8_t> &t1, const std::tuple<int16_t, int16_t, int8_t> &t2) {
						return std::get<0>(t1) < std::get<0>(t2) ||
									 (std::get<0>(t1) == std::get<0>(t2) &&
										std::get<1>(t1) > std::get<1>(t2));
					});

	bool res = MakePlaneClusters(Hits_Plane_X, 0);
	

	if (res)  {
		bool res2 = MakePlaneClusters(Hits_Plane_Y, 1);
		
		if (res2){
			auto pixel = std::make_shared<Pixel>(m_detector->getName().c_str(), std::get<0>(Clusters_Plane_X[0]), std::get<0>(Clusters_Plane_Y[0]), 1, std::get<1>(Clusters_Plane_X[0])+std::get<1>(Clusters_Plane_Y[0]), **evtID);
			pixel_container.push_back(pixel);
		}
	}

	m_eventNumber++;

  // Putting data to the clipboard, if empty and empty vector will be filled
  clipboard->putData(pixel_container, m_detector->getName());

	LOG(DEBUG) << std::endl;

  // Return value telling analysis to keep running
  if(reader->GetEntries()==m_eventNumber) return StatusCode::EndRun;
  return StatusCode::Success;
}

bool EventLoaderAPV25::MakePlaneClusters(std::vector<std::tuple<int16_t, int16_t, int8_t>> &hitsPlane, int which_plane){

	// No pedestal value --> 1-2 ADC count error in charge!
	
	TempPlaneClusterHistogram = new TH1F("PlaneCluster", "strip", 256, 0, 256);
	curMaxHitWaveform = new TH1F("curMaxHitWaveform", "timebin", 20, 0, 20);
	int lastStrip=-1;
	int maxAdc=-9999;
	int sumAdcs=0;
	int currentStrip=-1;
	int currentAdc = -9999;
	int hitsCount=0;
	int clusterCount = 0;
	size_t maxIndex;


	for (auto &hits : hitsPlane){
		currentStrip = std::get<0>(hits);
		currentAdc = std::get<1>(hits);
		
		if (lastStrip ==-1 || currentStrip - lastStrip == 1){
			TempPlaneClusterHistogram->Fill(currentStrip, currentAdc);
			if (currentAdc > maxAdc) {
				maxAdc = currentAdc;
				maxIndex = std::get<2>(hits);
			}
			sumAdcs += currentAdc;
			hitsCount++;
			LOG(DEBUG) << "currentStrip = " << currentStrip <<  ",   currentAdcs = " << currentAdc;
		}
		else {
			
			if (hitsCount >= 2){
				TempPlaneClusterHistogram->Fit("gaus", "q");
				LOG(DEBUG) << "Histo gaus mean = " <<  TempPlaneClusterHistogram->GetFunction("gaus")->GetParameter(1) ;
				
				// Cluster_Plane < strip, sumAdcs, cluster_size >
				if (which_plane==0) { // X plane
					Clusters_Plane_X.emplace_back(TempPlaneClusterHistogram->GetFunction("gaus")->GetParameter(1), sumAdcs, hitsCount);
				}
				else if (which_plane==1) { // Y plane
					Clusters_Plane_Y.emplace_back(TempPlaneClusterHistogram->GetFunction("gaus")->GetParameter(1), sumAdcs, hitsCount);
				}
				
				clusterCount++;
				}

			// Reset the clusterization. Hits to 1, histogram reseted and filled with
			// the current hit, otherwise it is lost!
			hitsCount = 1;
			maxAdc = -9999;
			sumAdcs=currentAdc;
			TempPlaneClusterHistogram->Reset();
			TempPlaneClusterHistogram->Fill(currentStrip, currentAdc);	
			}
			
		lastStrip = currentStrip;
	}

	// Check again if there are clusters
	if (hitsCount >= 2){
		TempPlaneClusterHistogram->Fit("gaus", "q");

		// Cluster_Plane < strip, sumAdcs, cluster_size >
		if (which_plane==0){ // X plane
			Clusters_Plane_X.emplace_back(TempPlaneClusterHistogram->GetFunction("gaus")->GetParameter(1), sumAdcs, hitsCount);
		}
		else if (which_plane==1){ // Y plane
			Clusters_Plane_Y.emplace_back(TempPlaneClusterHistogram->GetFunction("gaus")->GetParameter(1), sumAdcs, hitsCount);
		}

		clusterCount++;
	} 

	delete TempPlaneClusterHistogram;
	delete curMaxHitWaveform;
	
	// If there are more than 1 clusters in one plane, tracking is not done
	if (clusterCount==1) return true;
	
	// No clusters
	return false;
}


/*
 *
 * Need to fix if one wants to consider a multiple clusters of clusters in one plane 
 *
int EventLoaderAPV25::MatchPlaneClusters(std::vector<std::tuple<int16_t, int16_t, int16_t>> &clustersX,
                              						std::vector<std::tuple<int16_t, int16_t, int16_t>> &clustersY){


	double ratio_sum = 0;
	double bestRatio = std::numeric_limits<double>::max();
	int index = 0;
	int bestIndex = 0;
	// Best match according to charge matching
	
	// Clear matched cluster containers
	curXYclusters.clear();
	allClusters.clear();
	

	do{
			for(size_t i=0; i< clustersX->size(); i++){
				// XYclusters: < x_strip, y_strip, sum_charge, sum_clustSizes, Y_charge/X_charge >
				curXYclusters.emplace_back(
						std::get<0>(clustersX[i]), 
						std::get<0>(clustersY[i]), 
						std::get<1>(clustersX[i])+std::get<1>(clustersY[i]), 
						std::get<2>(clustersX[i])+std::get<2>(clustersY[i]), 
						static_cast<double>(std::get<1>(clustersY[i])) / std::get<1>(clustersX[i])
					);
				
				ratio_sum += static_cast<double>(std::get<1>(clustersY[i])) / std::get<1>(clustersX[i]);
				}
			
			if(ratio_sum<bestRatio){
					bestIndex = index;
					bestRatio = ratio_sum;
				}
			
			ratio_sum = 0;
			index++;
			allClusters.push_back(curXYclusters);
			curXYclusters.clear();
	}while(std::next_permutation(clustersY.begin(), clustersY.end()));


	// Return the best index
	return bestIndex;

	}
*/


void EventLoaderAPV25::finalize(const std::shared_ptr<ReadonlyClipboard>&) {
  LOG(DEBUG) << "Analysed " << m_eventNumber << " events";
}
