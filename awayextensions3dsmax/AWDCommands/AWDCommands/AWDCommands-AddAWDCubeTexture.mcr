macroScript AddAWDCubeTexture
	category:"AWDCommands"
	toolTip:"Create a AWDCubeTexture"
(

	theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCubeTexture")
	theobj.text ="Add name for new AWDCubeTexture"
	DialogResult = theObj.Showmodal()
	check=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
	if not check then return()
	result = theobj.InstanceName
	newCubeMat=AWDCubeMaterial()
	newCubeMat.name=result
	if not MatEditor.isOpen() then MatEditor.open() 
	--if material mode is "Compact-Mode"
	if MatEditor.mode ==#basic then (
		meditMaterials[activeMeditSlot] = newCubeMat
	)
	--if material mode is "Slate-Mode"
	else if MatEditor.mode ==#advanced then (
		cleanNameBool=false
		cleanNameCnt =  1
		cleanName = "AWDCubetexture"
		mainName = "AWDCubetexture"
		while not cleanNameBool do (
			thisView=sme.GetViewByName cleanName
			if thisView<=0 then cleanNameBool=true
			else (
				cleanName=mainName+"_"+(cleanNameCnt as string)
				cleanNameCnt+=1
			)				
		)
		sme.CreateView cleanName
		thisView=sme.GetView (sme.GetNumViews() as integer)
		sme.activeView=(sme.GetNumViews() as integer)
		thisView.CreateNode newCubeMat [0,0]
	)
	return newCubeMat	
)