---
# SPDX-FileCopyrightText: 2017-2023 CERN and the Corryvreckan authors
# SPDX-License-Identifier: CC-BY-4.0 OR MIT
---
# TreeWriter
**Maintainer**: Antti Lumppio (antti.lumppio@helsinki.fi), Diego Figueiredo (dmf@cern.ch)
**Detector Type**: *<GEM detector>*  
**Status**: Immature

### Description
This module takes inspiration from the default `[DUTTreeWriter]` module. This module writes out the cluster data in a ROOT TTree of the clusters that are part of Track objects. Best used with [ClusteringGeneric], as this module utilizes data associated with EventIDs, like APV25 data. This module can be used to create DUT track residuals. 

For each track the following information is written out:
* EventID
* Cluster positions in X and Y of the clusters that are part of the track
* Cluster charges of the clusters that are part of the track
* Track intercepts for each detector


### Parameters
* `file_name`: Name of the data file to create, relative to the output directory of the framework. The file extension `.root` will be appended if not present. Default value is `outputTuples.root`.
* `tree_name`: Name of the tree inside the output ROOT file. Default value is `tree`.

### Usage
```toml
[TreeWriterDUT]
file_name = "myOutputFile.root"
tree_name = "myTree"
```
