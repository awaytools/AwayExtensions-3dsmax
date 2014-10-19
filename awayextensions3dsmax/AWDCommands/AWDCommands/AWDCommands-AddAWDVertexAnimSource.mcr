macroScript AddAWDVertexAnimSource
	category:"AWDCommands"
	toolTip:""
(
	create=true
	meshes=selection
	if meshes.Count==1 then (
		obj=meshes[1]
		thisClassStr=classof obj as string
		if thisClassStr=="Editable_mesh" or thisClassStr=="PolyMeshObject" or thisClassStr=="Editable_Poly" then (
			
			)
		else(
			messageBox("To create a AWDVertexAnimationModifier, you need to select one object that will be exported as a Away3d Mesh.")
			return()
		)
			
		for m in obj.modifiers do(
			if (classof m as string) =="AWDVertexAnimSource" or (classof m as string) =="AWDVertexAnimSourceClone" then(
				messageBox("The selected Object allready containes a AWDVertexAnimtionSource-Modifier.")
				return()
			)
		)
		theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDVertexAnimSource")
		theobj.text ="Add name for new AWDVertexAnimSource"
		DialogResult = theObj.Showmodal()
		check=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
		if not check then return()
		result = theobj.InstanceName
		thisVertexAnimSource=AWDVertexAnimSource()
		thisVertexAnimSource.name = result
		addmodifier obj thisVertexAnimSource
	)
	
	else if meshes.count>1 then messagebox("AWDVertexAnimSource Modifier can only be attached to one object at a time.\n")		
	--if nothing is selected we check if nodes are in the scene
	else(
			meshes = $*
			selected=#()
			if meshes!= undefined then (
				for i in meshes do(
					for m in i.modifiers do(
						if classof m as string=="AWDVertexAnimSource" or classof m as string=="AWDVertexAnimSourceClone" then(
							appendIfUnique selected i
						)				
					)
				)	
			if selected.count>0 then(
				answer=queryBox ("Nothing is selected.\n\n "+selected.count  as string+" AWDVertexAnimSource-modifier found in the scene.\n\n Select the Objects that are holding the modifiers ?")
				if answer==true then(
					select selected
					)
				)
			else(
				answer=messageBox ("No Object is selected.\n\n No AWDVertexAnimSource modifiers found in the scene.\n\n To create a AWD AWDVertexAnimSource modifier, select the animated mesh.")
				)
			)
		)
)
	