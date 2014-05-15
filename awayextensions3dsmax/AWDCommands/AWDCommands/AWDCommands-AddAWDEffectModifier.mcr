macroScript AddAWDEffectModifier
 	category:"AWDCommands"
	toolTip:""
(
	
	if selection.count==0 then (
		selected=#()
		for i in objects do(
				for m in i.modifiers do(
					if classof m as string=="AWDEffectMethod" then(
						appendIfUnique selected i
					)				
				)
			)	
		if selected.count>0 then(
			answer=queryBox ("Nothing is selected.\n\n "+selected.count  as string+" AWDEffectMethod modifiers found in the scene.\n\n Select the Objects that are holding the modifiers ?")
			if answer==true then(
				select selected
			)
		)
		else	messageBox ("No Object is selected.\n\n No AWDEffectMethod modifiers found in the scene.\n\n To create a AWDEffectMethod modifier, select one object.")
			
	)
	else if selection.count>1 then messageBox ("Multiple objects selected.\n\nTo create a AWDEffectMethod modifier, select one object.")
	else if selection.count==1 then (
		theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDEffectMethod")
		theobj.text ="Add name for new AWDEffectMethod"
		DialogResult = theObj.Showmodal()
		dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
		result = theobj.InstanceName
		newEffectMethod=AWDEffectMethod()
		newEffectMethod.name=result
		addModifier selection[1] newEffectMethod
		max modify mode 	
	)	
	
)