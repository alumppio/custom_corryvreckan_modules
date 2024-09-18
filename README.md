# Default corryvreckan installation
To install the default version of corryvreckan you need to have [ROOT](https://root.cern.ch/building-root) installed, with the GenVector component.

**First you need to download the source code** 
```
$ git clone https://gitlab.cern.ch/corryvreckan/corryvreckan.git
```
**Then you need to compile, but not install it until you have added my custom modules**
```
$ cd corryvreckan
$ mkdir build && cd build/
$ cmake ..
```

## To use my custom modules

I have made quite a lot of changes to the default corryvreckan installation. I don't remember all of them, but I included a folder that contains most (or almost all) of the modules I modified or created. You need to copy this to some folder presumably not inside corryvreckan, as I don't known if this will cause errors. Then you have to go to the source code modules folder *corryvreckan/src/modules/* and copy the modules that I have created. If the module is already present in the default version you have created with cmake, then the module I have in my folder is a modified one and you need to overwrite it in the default version.

### Copy my repository
```
$ git clone https://github.com/alumppio/custom_corryvreckan_modules.git <desired_path>
```

### Copying the modified modules and the new modules to <corryvreckan/src/modules>
```
>> For the modified modules
$ cd corryvreckan/src/modules
$ rm -r Tracking4D
$ cp <path_to_modified_Tracking4D> .
>> For the new modules
$ cp <all_other_modules> . 
```

### Installing the custom corryvreckan vesrion

**After this you need to install corryvreckan!** This means that you only need to go to the build folder and run *make install*. Also everytime you change something in the modules, you need run *make install* so the changes will be applied. You should alias this *make install* in the build folder to some command, makes life easier.


> If you are using lxplus remember to also source etc/setup\_lxplus.sh in corryvreckan folder

```
>> If you are using lxplus, in <corryvreckan>
$ source etc/setup_lxplus.sh
>> Installing in <corryvreckan/build>
$ make install
```

Now you should have a custom version of corryvreckan install and you are ready to use it. 

## Getting started with corryvreckan

There is a list of tutorials and references on the [corryvreckan project website](https://project-corryvreckan.web.cern.ch/project-corryvreckan/page/publications/). You can also find the detailed user manual that describes the whole framework under the documentations and references tab ([link for manual-v2.0.1](https://project-corryvreckan.web.cern.ch/project-corryvreckan/usermanual/corryvreckan-manual-v2.0.1.pdf)).

There is also a quite detailed [lab course on silicon sensors](https://www.physi.uni-heidelberg.de/Einrichtungen/FP/anleitungen/F96.pdf) where corryvreckan is used.

 [My lab report](Lab_report_CERN.pdf) describes quite well what I did in Corryvreckan.


## Using my modules to get tracking data with gaseous strip detectors

I found two approaches that can successfully create tracking data using corryvreckan: by creating Cluster objects from reconstructed gaseous strip detector clusters or by creating Pixel objects from matched reconstructed XY clusters in Corryvreckan. I found the method of creating Pixel objects out of matched clusters to be the better one.

The approaches are explained in more detail in [a lab report of my summer work at CERN].


## Approach 1: creating Cluster objects from cluster data

In this approach I used [ClusteringGeneric] module to read in cluster data reconstructed with AMORE. This cluster data is in a TCluster ROOT TTree and from this [ClusteringGeneric] reads in *evtID, clustPos, clustADCs, clustTimebin, planeID, detID*. From this data the module creates corryvreckan Cluster objects at the cartesian point given in the TCluster TTree that is corrected with the alignments made by corryvreckan. In this module you need to give the noise\_cut you want to apply for the clusters and the input file name.  

> TCluster format: using TTree-\>Print()
```
******************************************************************************
*Tree    :TCluster  : GEM Cluster Data Rootfile                              *
*Entries :      998 : Total =          286787 bytes  File  Size =     108735 *
*        :          : Tree compression factor =   2.62                       *
******************************************************************************
*Br    0 :evtID     : evtID/I                                                *
*Entries :      998 : Total  Size=       4553 bytes  File Size  =       1517 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   2.68     *
*............................................................................*
*Br    1 :nclust    : nclust/I                                               *
*Entries :      998 : Total  Size=       4558 bytes  File Size  =        957 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.25     *
*............................................................................*
*Br    2 :clustPos  : clustPos[nclust]/F                                     *
*Entries :      998 : Total  Size=      42382 bytes  File Size  =      36803 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   1.14     *
*............................................................................*
*Br    3 :clustSize : clustSize[nclust]/I                                    *
*Entries :      998 : Total  Size=      42381 bytes  File Size  =       8612 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   4.85     *
*............................................................................*
*Br    4 :clustADCs : clustADCs[nclust]/F                                    *
*Entries :      998 : Total  Size=      42388 bytes  File Size  =      36437 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   1.15     *
*............................................................................*
*Br    5 :clustTimebin : clustTimeBin[nclust]/I                              *
*Entries :      998 : Total  Size=      42399 bytes  File Size  =       7945 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   5.26     *
*............................................................................*
*Br    6 :detID     : detID[nclust]/I                                        *
*Entries :      998 : Total  Size=      42357 bytes  File Size  =       5101 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   8.19     *
*............................................................................*
*Br    7 :planeID   : planeID[nclust]/I                                      *
*Entries :      998 : Total  Size=      42369 bytes  File Size  =       5475 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   7.63     *
*............................................................................*
*Br    8 :etaSector : etaSector[nclust]/S                                    *
*Entries :      998 : Total  Size=      23484 bytes  File Size  =       4648 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.93     *
*............................................................................*
```
### Detectors file

In this approach, I defined the detector as a pixel detector with 256x256 pixels and the pixel pitches corresponded to the strip lengths 400um,400um.
> Example GEMXY detector
```
[GEMXY3]
coordinates = "cartesian"
material_budget = 0.01068
number_of_pixels = 256, 256
orientation = 2.53253deg,-6.1711deg,-0.501625deg
orientation_mode = "xyz"
pixel_pitch = 400um,400um
position = -1.62954mm,-492.066um,-1365mm
spatial_resolution = 400um,400um
time_resolution = 200ns
type = "gemrd51"
```


### Alignment 

Before tracking the telescope needs to be aligned. This is done using default modules [Prealignment] and [AlignmentTrackChi2]. First you do a run with the [Prealignment]. In this you only need to have [Metronome], [ClusteringGeneric] and the [Prealignment] modules. After the prealignment run, you run another run with [Metronome], [ClusteringGeneric], [Tracking4D] and [AlignmentTrackChi2] modules using the updated detectors file you got from the prealignment. If you have a DUT, this also needs to be aligned. DUT is automatically excluded from tracking so the detector need to associated with the tracking using [DUTAssociation] module. DUT alignment run is done using [Metronome], [ClusteringGeneric], [Tracking4D], [DUTAssociation], [AlignmentDUTResidual] modules. The alignment can be checked using [Correlations] module which provides correlation plots.





### Some tips for troubleshooting

If your are looking at plots and they seem weird, check the binning and how it is filled in the source code of the module. 
