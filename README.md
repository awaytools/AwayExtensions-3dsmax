AwayExtensions-3dsmax
=====================


## What are Away Extensions 3dsmax?

Away Extensions 3dsmax are a collection of complimentary tools for use in an Away3D project workflow. 
The current extensions list includes an AWD exporter plugin, that enables the export of 3dsmax-scenes as AWD-files, 
and a set of custom 3dsmax notes to define object for export to Away3d.
Like all other Away Extensions, it is free and open source. 


## How should Away Extensions 3dsmax be used ?

Away Extensions 3dsmax can currently be used to create AWD files that can be loaded and used within the Away3D Engine. 

For best results one should use the AwayExtensions with the [latest dev-branch of the away3d-engine](https://github.com/away3d/away3d-core-fp11/tree/dev).

A recommended workflow is to inspect and finalize the exported files using [Away Builder](http://www.awaytools.com/awaybuilder).

## Which 3dsmax versions are supported ?

The current installer provides support for this versions:
* 3dsmax 2011 32/64
* 3dsmax 2012 32/64
* 3dsmax 2013 32/64
* 3dsmax 2014 64

## How to install Away Extensions 3dsmax ?

* For each supported 3dsmax version, a sepperate Setup-file is provided. Just run this file, and follow the instructions.

In 3dsmax, you should see following 
* the MaxAWDExporter-plugin should be listet in the plugin manager.
* multiple Custom-Modifier should be a listed in the modifier-list.
* multiple Custom Commands should be listed in the Command list of the Customize-User-Interface-Panel.

The Awayextensions-3dsmax includes a AWDToolBar that should allready have been copied to your 3dsmax-User-UI directory.
All there is left to do, is to go to Customize -> Customize-User-Interface -> Tab:Toolbars and load the AWDToolBar-file 
(*.CUI for 3dsmax 2011/2012, *.cuix for 3dsmax 2013/2014).

 
## What is the current state of development for the Away Extensions 3dsmax ?

AwayExtensions3dsmax is quite evolved and only a few features are missing before it will have full support for AWD2.1. 

The missing features are:
* UVAnimation
* Shadowmethods

A Importer is also under development, but not clear when this will be in a shareable state.


## Where can i get more information/documentation about Away Extensions 3dsmax ?

There is a quick-documentation included with the plugin.
Either unzip the *.mzp file as if it where a *.zip file, and find the folder "AWDExtensions3dsmax",
or browse to your 3dsmax-plugin folder and find the folder "AWDExtensions3dsmax".
This folder will contain the "AWDExtensions3dsmax_quickstart.pdf".

More documentation will be provided on the soon.

