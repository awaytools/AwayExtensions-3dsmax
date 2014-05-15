macroScript AddAWDAnimator
	category:"AWDCommands"
	toolTip:""


(
	meshes = selection
	--only apply AWDAnimator if one object is selected
	if meshes.count == 1 then (
		thismesh=meshes[1]
		thisClassStr=classof thismesh as string
		-- if selected object will be exported as Away3d-Mesh
		if thisClassStr=="Editable_mesh" or thisClassStr=="PolyMeshObject" or thisClassStr=="Editable_Poly" then (	
			hasAnimator=False
			for thisMod in thismesh.modifiers do(
				if (classOf thisMod as string)=="AWDAnimator" then (
					hasAnimator=True)
				)
			if (hasAnimator==True) then(
				messagebox ("Selected Object allready contains a AWDAnimator-modifier.\n\nEach object should only hold one AWDAnimator-modifier.")
			)
			else(
				
				theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDAnimator")
				theobj.text ="Add name for new AWDAnimator"
				DialogResult = theObj.Showmodal()
				check=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
				if not check then return()
				result = theobj.InstanceName
				MyNewModifier = AWDAnimator()
				MyNewModifier.name = result
				addmodifier thismesh MyNewModifier		
			)
		)
		-- if selected object is not a mesh object (might still be a primitve)
		else(
			messagebox ("Selected Object will not be exported as a Away3d-Mesh.\n\nOnly objects that can be exported as Away3d-meshes should hold a AWDAnimator-modifier.")
		)
	)
	--if more than 1 object is selected
	else if meshes.count > 1 then (
			messagebox ("AWDAnimator-Modifier can only be created for a single object.\n\n")
	)
	-- if no object is selected
	else(
		meshes = $*
		selected=#()
		firstmod=undefined
		if meshes!= undefined then (
			for i in meshes do(
				for oneMod in i.modifiers do(
					if (classOf oneMod as string)=="AWDAnimator" then (
						appendIfUnique selected i
					)
				)
			)
		)	
		if selected.count>1 then(
			answer=queryBox ("Nothing is selected.\n\nScene contains "+selected.count  as string+" objects that have AWDAnimator-modifier applied.\n\nSelect the Objects ?")
			if answer==true then(
				select selected
				)
			)
		else if selected.count==1 then(
			answer=queryBox ("Nothing is selected.\n\n Scene contains "+selected.count  as string+" object that has AWDAnimators-modifier applied.\n\nSelect the Object ?")
			if answer==true then(
				select selected
				)
			)
		else (
			messagebox ("Nothing is selected.\n\n Scene contains no objects that have AWDAnimators-modifier applied.\n\n")

			)
		)
	
)