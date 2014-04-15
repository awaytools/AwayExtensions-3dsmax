macroScript GotoAway3dForum
	category:"AWDCommands"
	toolTip:""
(
	shellLaunch "http://www.away3d.com" ""
)
macroScript ExportAWD
	category:"AWDCommands"
	toolTip:""
(
	onlyFileName=filterString maxFileName "." 
	if onlyFileName.Count==0 then onlyFileName=#("NewAWDFile")
		
	newAWDFileName=maxFilePath + onlyFileName[1] + ".awd"
	newFileName=getSaveFileName caption:"Save as AWD-File" fileName:newAWDFileName
	if newFileName!=undefined then(
		exportFile newFileName -- [ #noPrompt ] [ selectedOnly:<boolean> ] [ using:<maxclass> ] 
	)
)
macroScript ImportAWD
	category:"AWDCommands"
	toolTip:""
(
	messageBox("AWD-Importer is not in a working state yet.")
)
macroScript AWDCheckForUpdate
	category:"AWDCommands"
	toolTip:""
(

	test = internet.CheckConnection url:"http://www.awaytools.com/awayextensions/awayextensions3dsmax/awayextensions3dsmaxversion.xml" force:true
	if test then (
		targetFile=(getdir #maxroot) + "\\plugins\\AwayExtensions3dsMax\\3dsmaxversion.xml"
		dragAndDrop.DownloadUrlToDisk "http://awaytools.com/awayextensions/awayextensions3dsmax/awayextensions3dsmaxversion.xml" targetFile 1
		if (getfiles targetFile).count != 0 then(
			xmldoc = dotnetobject "System.XML.XMLDocument"
			isloaded=xmldoc.load targetFile
			print isLoaded
			itemIter = (xmldoc.selectnodes "//AWDMAXVERSION").GetEnumerator()
			thisVersion = "0.9.7"
			versionComplete =""
			while itemIter.MoveNext() do
			(
				attrIter = itemIter.Current.Attributes.GetEnumerator()
				major=itemIter.Current.getAttribute("Major")
				minor=itemIter.Current.getAttribute("Minor")
				revision=itemIter.Current.getAttribute("Revision")
				versionComplete=major+"."+minor+"."+revision
				message=itemIter.Current.getAttribute("Message")
			)
			if versionComplete=="" then (
				messageBox("Could not read the version-file from the internet.")
			)
			else(
				if versionComplete==thisVersion then (
					messageBox("Your AwayExtensions3dsmax is up to date.")
				)
				else (
					messageStr="Your AwayExtensions3dsmax is outdated.\n\n"
					messageStr+="Your version: "+ thisVersion+"\n"
					messageStr+="Latest version: "+ versionComplete+"\n\n"
					messageStr+="Release-message: "+ message+"\n"					
					messageBox(messageStr)
					shellLaunch "https://github.com/awaytools/AwayExtensions-3dsmax/releases" ""
				)
			)
		)
		else(
			messageBox("Error reading the version-file ().")
		)
	)
	else(
		messageBox("Could not connect to the internet.")
	)
		
)
macroScript OpenAWDHelperMenu
	category:"AWDCommands"
	toolTip:""
(
	rollout awdHelperMenuRollOut "AWD Helper Menu" --define a rollout
	(
		button btn_forum "www.away3d.com" width:190 pos:[10,10]
		button btn_update "Check for Update" width:190 pos:[10,35]
		button btn_about "About/Help" width:190 pos:[10,60]
		button btn_import "Import AWD-File" width:190 pos:[10,85]
		button btn_export "Export AWD-File" width:190 pos:[10,110]
		button btn_include "Include selected Nodes" width:190 pos:[10,150]
		button btn_exclude "Exclude selected Nodes" width:190 pos:[10,175]
		button btn_matSettings "Create Material Settings" width:190 pos:[10,210]
		button btn_TextureSettings "Create Texture Settings" width:190 pos:[10,235]
		button btn_SkyBox "Create SkyBox" width:190 pos:[10,270]
		button btn_Cubetex "Create CubeTexture" width:190 pos:[10,295]
		
		button btn_addSkeleton "Create SkeletonModifier" width:190 pos:[10,330]
		button btn_addVertex "Create Vertex-Anim-Source-Modifier" width:190 pos:[10,355]
		button btn_addAnimator "Create Animator-Modifier" width:190 pos:[10,380]
		button btn_addAnimSet "Create AnimationSet-Modifier" width:190 pos:[10,405]
		
		on btn_forum pressed do macros.run "AWDCommands" "GotoAway3dForum" 
		on btn_update pressed do macros.run "AWDCommands" "GotoAway3dForum" 
		on btn_about pressed do macros.run "AWDCommands" "GotoAway3dForum" 
		on btn_import pressed do macros.run "AWDCommands" "ImportAWD"
		on btn_export pressed do macros.run "AWDCommands" "ExportAWD"
		on btn_include pressed do macros.run "AWDCommands" "IncludeSelectedObjects"
		on btn_exclude pressed do macros.run "AWDCommands" "ExcludeSelectedObjects"
		on btn_matSettings pressed do macros.run "AWDCommands" "AddAWDMaterialSettings"
		on btn_TextureSettings pressed do macros.run "AWDCommands" "AddAWDTextureSettings"
		on btn_SkyBox pressed do macros.run "AWDCommands" "AddAWDSkyBox"
		on btn_Cubetex pressed do macros.run "AWDCommands" "AddAWDCubeTexture"
		on btn_addSkeleton pressed do macros.run "AWDCommands" "AddAWDSkeleton"
		on btn_addVertex pressed do macros.run "AWDCommands" "AddAWDVertexAnimSource"
		on btn_addAnimator pressed do macros.run "AWDCommands" "AddAWDAnimator"
		on btn_addAnimSet pressed do macros.run "AWDCommands" "AddAWDAnimationSet"
	)
	on isChecked do awdHelperMenuRollOut.open --return true if rollout is open --if isChecked returns false (the rollout is not open in a dialog), --the on execute handler will be called and will create the dialog.

	on execute do (		
			try destroyDialog awdHelperMenuRollOut catch (print ("couldfind it"))
				createDialog awdHelperMenuRollOut width:210
			)
	  --If isChecked returns true and the user pressed the button again, --instead of calling the on execute handler, the macroScript will call --the on closeDialogs handler and destroy the dialog.

	on closeDialogs do destroyDialog awdHelperMenuRollOut
)