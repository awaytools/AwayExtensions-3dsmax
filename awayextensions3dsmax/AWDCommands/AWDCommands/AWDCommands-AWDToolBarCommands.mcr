macroScript GotoAway3dForum
	category:"AWDCommands"
	toolTip:"Open the Away3d forum in the browser"
(
	shellLaunch "http://www.away3d.com/forum" ""
)
macroScript OpenAWDAboutDialog
	category:"AWDCommands"
	toolTip:""
(
	
	targetLocalFile=(getdir #maxroot) + "\\plugins\\AwayExtensions3dsMax\\3dsmaxversionLocal.xml"
	xmldoc = dotnetobject "System.XML.XMLDocument"
	isloaded=xmldoc.load targetLocalFile
	itemIter = (xmldoc.selectnodes "//AWDMAXVERSION").GetEnumerator()
	global versionCompleteLocal ="AWDExtensions3dsmax_v"
	while itemIter.MoveNext() do
	(
		attrIter = itemIter.Current.Attributes.GetEnumerator()
		major=itemIter.Current.getAttribute("Major")
		minor=itemIter.Current.getAttribute("Minor")
		revision=itemIter.Current.getAttribute("Revision")
		append versionCompleteLocal (major as string +"."+minor as string+"."+revision as string)
		message=itemIter.Current.getAttribute("Message")
	)
	if versionCompleteLocal=="AWDExtensions3dsmax_v" then (
		messageBox("Could not read the local version-file.")
		return()
	)	
	
rollout rl_install "About AWDExtensions3dsmax" width:306 height:300
(
	
	bitmap bmp1 "Bitmap" pos:[3,3] width:300 height:300 fileName:"AWDAbout.bmp"	
	label txt_stats versionCompleteLocal pos:[63,260] width:180 height:16 textAlign:#right
	
)


CreateDialog rl_install 306 300 modal:true
)
macroScript OpenAWDHelpFile
	category:"AWDCommands"
	toolTip:""
(
	targetFile=(getdir #maxroot) + "\\plugins\\AwayExtensions3dsMax\\AWDExtensions3dsmax_docs.pdf"
	shellLaunch targetFile ""
)
macroScript ExportAWD
	category:"AWDCommands"
	toolTip:""
(
	onlyFileName=filterString maxFileName "." 
	if onlyFileName.Count==0 then onlyFileName=#("NewAWDFile")
		
	newAWDFileName=maxFilePath + onlyFileName[1] + ".awd"
	newFileName=getSaveFileName caption:"Save as AWD-File" types:"Away3d(*.awd)|*.awd" fileName:newAWDFileName
	if newFileName!=undefined then(
		exportFile newFileName --#noPrompt -- [ selectedOnly:<boolean> ] [ using:<maxclass> ] 
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

	targetLocalFile=(getdir #maxroot) + "\\plugins\\AwayExtensions3dsMax\\3dsmaxversionLocal.xml"
	xmldoc = dotnetobject "System.XML.XMLDocument"
	isloaded=xmldoc.load targetLocalFile
	itemIter = (xmldoc.selectnodes "//AWDMAXVERSION").GetEnumerator()
	versionCompleteLocal =""
	while itemIter.MoveNext() do
	(
		attrIter = itemIter.Current.Attributes.GetEnumerator()
		major=itemIter.Current.getAttribute("Major")
		minor=itemIter.Current.getAttribute("Minor")
		revision=itemIter.Current.getAttribute("Revision")
		versionCompleteLocal=major+"."+minor+"."+revision
		message=itemIter.Current.getAttribute("Message")
	)
	if versionCompleteLocal=="" then (
		messageBox("Could not read the local version-file.")
		return()
	)	
	hasConnection = internet.CheckConnection url:"http://www.awaytools.com/awayextensions/awayextensions3dsmax/awayextensions3dsmaxversion.xml" force:true
	if hasConnection then (
		targetFile=(getdir #maxroot) + "\\plugins\\AwayExtensions3dsMax\\3dsmaxversion.xml"
		downloadSuccess=dragAndDrop.DownloadUrlToDisk "http://awaytools.com/awayextensions/awayextensions3dsmax/awayextensions3dsmaxversion.xml" targetFile 1
		if downloadSuccess then(
			xmldoc = dotnetobject "System.XML.XMLDocument"
			isloaded=xmldoc.load targetFile
			itemIter = (xmldoc.selectnodes "//AWDMAXVERSION").GetEnumerator()
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
				if versionComplete==versionCompleteLocal then (
					messageBox("Your AwayExtensions3dsmax is up to date.")
				)
				else (
					messageStr="Your AwayExtensions3dsmax is outdated.\n\n"
					messageStr+="Your version: "+ versionCompleteLocal+"\n"
					messageStr+="Latest version: "+ versionComplete+"\n\n"
					messageStr+="Release-message: "+ message+"\n"					
					messageBox(messageStr)
					shellLaunch "https://github.com/awaytools/AwayExtensions-3dsmax/releases" ""
				)
			)
		)
		else(
			messageBox("Error downloading the version-file.")
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
		button btn_about "About" width:190 pos:[10,60]
		button btn_help "Help" width:190 pos:[10,85]		
		
		button btn_import "Import AWD-File" width:190 pos:[10,125]
		button btn_export "Export AWD-File" width:190 pos:[10,150]
		button btn_include "Include selected Nodes" width:190 pos:[10,190]
		button btn_exclude "Exclude selected Nodes" width:190 pos:[10,215]
		button btn_matSettings "Create Material Settings" width:190 pos:[10,255]
		button btn_TextureSettings "Create Texture Settings" width:190 pos:[10,280]
		button btn_Cubetex "Create CubeTexture" width:190 pos:[10,305]
		button btn_effectMethod "Create Effect-Method-Modifier" width:190 pos:[10,330]
		button btn_SkyBox "Create SkyBox" width:190 pos:[10,355]
		
		button btn_addVertex "Create Vertex-Anim-Source-Modifier" width:190 pos:[10,395]
		button btn_addSkeleton "Create SkeletonModifier" width:190 pos:[10,420]
		button btn_addAnimSet "Create AnimationSet-Modifier" width:190 pos:[10,470]
		button btn_addAnimator "Create Animator-Modifier" width:190 pos:[10,445]
		
		on btn_forum pressed do macros.run "AWDCommands" "GotoAway3dForum" 
		on btn_update pressed do macros.run "AWDCommands" "AWDCheckForUpdate" 
		on btn_about pressed do macros.run "AWDCommands" "OpenAWDAboutDialog" 
		on btn_help pressed do macros.run "AWDCommands" "OpenAWDHelpFile" 
			
		on btn_import pressed do macros.run "AWDCommands" "ImportAWD"
		on btn_export pressed do macros.run "AWDCommands" "ExportAWD"
		on btn_include pressed do macros.run "AWDCommands" "IncludeSelectedObjects"
		on btn_exclude pressed do macros.run "AWDCommands" "ExcludeSelectedObjects"
		on btn_matSettings pressed do macros.run "AWDCommands" "AddAWDMaterialSettings"
		on btn_TextureSettings pressed do macros.run "AWDCommands" "AddAWDTextureSettings"
		on btn_effectMethod pressed do macros.run "AWDCommands" "AddAWDEffectModifier"
		on btn_SkyBox pressed do macros.run "AWDCommands" "AddAWDSkyBox"
		on btn_Cubetex pressed do macros.run "AWDCommands" "AddAWDCubeTexture"
		on btn_addVertex pressed do macros.run "AWDCommands" "AddAWDVertexAnimSource"
		on btn_addSkeleton pressed do macros.run "AWDCommands" "AddAWDSkeleton"
		on btn_addAnimSet pressed do macros.run "AWDCommands" "AddAWDAnimationSet"
		on btn_addAnimator pressed do macros.run "AWDCommands" "AddAWDAnimator"
	)
	on isChecked do awdHelperMenuRollOut.open --return true if rollout is open --if isChecked returns false (the rollout is not open in a dialog), --the on execute handler will be called and will create the dialog.

	on execute do (		
			try destroyDialog awdHelperMenuRollOut catch (print ("couldfind it"))
				createDialog awdHelperMenuRollOut width:210
			)
	  --If isChecked returns true and the user pressed the button again, --instead of calling the on execute handler, the macroScript will call --the on closeDialogs handler and destroy the dialog.
		
	on closeDialogs do (
		destroyDialog awdHelperMenuRollOut
	)
)