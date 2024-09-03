---
# SPDX-FileCopyrightText: 2017-2023 CERN and the Corryvreckan authors
# SPDX-License-Identifier: CC-BY-4.0 OR MIT
---
# ClusteringGeneric
**Maintainer**: Diego Figueiredo (dmf@cern.ch), Antti Lumppio (antti.lumppio@helsinki.fi)
**Module Type**: *DETECTOR* **Detector Type**: *<GEM detector>*  
**Status**: Immature

### Description
This module takes in APV25 data in AMORE TCluster format. From this cluster data, this module creates Cluster objects. Events with only a single hit per telescope are read in. 

### Parameters
* `file_input`: The ROOT file name that contains the TCluster TTree.
* `noise_cut`: The noise cut applied to read in Clusters. If the charge of cluster doesn't go above this value, the cluster isn't read in.


### Plots produced
* Histogram of event numbers

For each detector the following plots are produced:

* 2D histogram of pixel hit positions

### Usage
```toml
[ClusteringGeneric]
file_input = "data_file"

```
