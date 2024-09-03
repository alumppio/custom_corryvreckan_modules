---
# SPDX-FileCopyrightText: 2017-2023 CERN and the Corryvreckan authors
# SPDX-License-Identifier: CC-BY-4.0 OR MIT
---
# EventLoaderAPV25
**Maintainer**: Antti Lumppio (antti.lumppio@helsinki.fi)
**Module Type**: *DETECTOR* **Detector Type**: *<GEM detector>*  
**Status**: Immature

### Description
This module reads in APV25 data in AMORE THits TTree format. From THits this module first reconstructs the clusters for each plane, and after this matches the X- and Y-plane clusters. The matching is done with the logic that the charge sharing should be equal between the readout planes. In summary, the algorithm makes all of the XY cluster pairs and chooses the cluster pairs that have the lowest Y/X charge ratio between all pairs. This ratio also has to be inbetween cuts >0.5 and <1.5. From the matched clusters, this module creates Pixel objects with the charge of the pixels being the sum of all strip charges from both planes. 

### Parameters
* `file_input`: The input data file that contains the THits TTree.

### Plots produced
* Sum of all waveforms for the peak signal for both planes of every detector



### Usage
```toml
[EventLoaderAPV25]
file_input = data_file


```
