macroScript AddAWDCameraModifier
 	category:"AWDCommands"
	toolTip:""
 (
	
	if selection.count==0 then (
		selected=#()
		for i in objects do(
			if superclassof i==camera then(
				for m in i.modifiers do(
					if classof m as string=="AWDCamera" then(
						appendIfUnique selected i
					)				
				)
			)	
		)
		if selected.count>0 then(
			answer=queryBox ("Nothing is selected.\n\n "+selected.count  as string+" AWDCamera modifiers found in the scene.\n\n Select the Objects that are holding the modifiers ?")
			if answer==true then(
				select selected
			)
		)
		else	messageBox ("No Object is selected.\n\n No AWDCamera modifiers found in the scene.\n\n To create a AWDCamera modifier, select one camera.")
			
	)
	else if selection.count>=1 then (
		createdAny=false
		lightSelected=false
		for i in selection do(
			if superclassof i==camera then(
				lightSelected=true
				print (superclassof i)
				hasModifier=false
				for m in i.modifiers do(
					if classof m as string=="AWDCamera" then(
						hasModifier=true
					)				
				)
				if not hasModifier then (
					theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCamera")
					theobj.text ="Add name for new AWDCamera"
					DialogResult = theObj.Showmodal()
					createdAny=true
					dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
					result = theobj.InstanceName
					newShadowMethod=AWDCamera()
					newShadowMethod.name=result
					addModifier selection[1] newShadowMethod
				)
			)
		)
		if lightSelected then (
			if not createdAny then(
				messageBox ("No AWDCamera-modifer created, because all selected camera allready have one applied.")
			)
		)
		else messageBox ("To create a AWDCamera-modifier, you need to select a camera-object.")
			
		max modify mode 	
	)	
	
)