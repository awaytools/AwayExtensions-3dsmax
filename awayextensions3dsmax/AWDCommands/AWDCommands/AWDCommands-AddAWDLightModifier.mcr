macroScript AddAWDLightModifier
 	category:"AWDCommands"
	toolTip:""
 (
	
	if selection.count==0 then (
		selected=#()
		for i in objects do(
			if superclassof i==light then(
				for m in i.modifiers do(
					if classof m as string=="AWDShadowMethod" then(
						appendIfUnique selected i
					)				
				)
			)	
		)
		if selected.count>0 then(
			answer=queryBox ("Nothing is selected.\n\n "+selected.count  as string+" AWDShadowMethod modifiers found in the scene.\n\n Select the Objects that are holding the modifiers ?")
			if answer==true then(
				select selected
			)
		)
		else	messageBox ("No Object is selected.\n\n No AWDShadowMethod modifiers found in the scene.\n\n To create a AWDShadowMethod modifier, select one light.")
			
	)
	else if selection.count>=1 then (
		createdAny=false
		lightSelected=false
		for i in selection do(
			if superclassof i==light then(
				lightSelected=true
				print (superclassof i)
				hasModifier=false
				for m in i.modifiers do(
					if classof m as string=="AWDShadowMethod" then(
						hasModifier=true
					)				
				)
				if not hasModifier then (
					theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDShadowMethod")
					theobj.text ="Add name for new AWDShadowMethod"
					DialogResult = theObj.Showmodal()
					createdAny=true
					dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
					result = theobj.InstanceName
					newShadowMethod=AWDShadowMethod()
					newShadowMethod.name=result
					addModifier selection[1] newShadowMethod
				)
			)
		)
		if lightSelected then (
			if not createdAny then(
				messageBox ("No AWDShadowMethod-modifer created, because all selected lights allready have one applied.")
			)
		)
		else messageBox ("To create AWDShadowMethods, you need to select a light-object.")
			
		max modify mode 	
	)	
	
)