macroScript AddAWDSkyBox
	category:"AWDCommands"
	toolTip:""
(
	create=true
	for obj in objects do(
		if (classof obj as string) =="AWDSkyBox" then(
			create=false
			exit
		)
	)
	
	if not create then (
		answer=queryBox("Scene allready contains a AWDSkyBox.\n\nStill add another one ?")
		if answer then create=true
		)
	if create then (
		theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDSkyBox")
		theobj.text ="Add name for new AWDSkyBox"
		DialogResult = theObj.Showmodal()
		check=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
		if not check then return()
		result = theobj.InstanceName
		print "dsfds"+(check as string)
		print result
		if result!="" then(
			thisSkyBox=AWDSkyBox()
			thisSkyBox.size=100
			thisSkyBox.name = result
			select thisSkyBox
			max modify mode
		)
	)
	

)