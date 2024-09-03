---
# SPDX-FileCopyrightText: 2017-2023 CERN and the Corryvreckan authors
# SPDX-License-Identifier: CC-BY-4.0 OR MIT
---
# VMM3aStripDataPreserver
**Maintainer**: alumppio (lxplus915.cern.ch)
**Module Type**: *GLOBAL*  
**Status**: Immature

### Description
This module recovers strip data that was lost while constructing tracks inside corryvreckan. Used together with [ClusterLoaderVMM3a]. From the same vmm-sdat clusters\_detector TTree that data was read in from, cluster strip data is read in. The strip data of the matched clusters that are part of corryvreckan tracks is read in. The clusters have to be inbetween the clustering time determine in this module. The search of clusters is done using this clustering\_time variable and the timestamp of the track.


### Parameters

* `file_input`: The same input data file used in [ClusterLoaderVMM3a], that contains clusters\_detector TTree. 
* `clustering_time`: Determines the clustering time used to make the clusters in one event. This time is used to recover the clusters that are part of the track. Time is given in nanoseconds. 

      m_inputFile = config_.get<std::string>("file_input");
      LOG(DEBUG) << "Input file name: " << m_inputFile;
      clustering_time = config_.get<double>("clustering_time");
      m_treefileName = config_.get<std::string>("output_tree_name");


### Plots produced
No plots are produced.



### Usage
```toml
[VMM3aStripDataPreserver]
file_input = "data_file"
clustering_time = 200



```
